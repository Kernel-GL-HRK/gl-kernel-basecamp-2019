#include <linux/proc_fs.h>
#include <linux/sched.h>
#include <asm/uaccess.h>
#include <linux/slab.h>
#include <linux/device.h>
#include <linux/module.h>
#include <linux/string.h>
#include <linux/kthread.h>
#include <linux/mutex.h>
#include <linux/delay.h>
#include <linux/semaphore.h>
#include <linux/kernel.h>

#ifndef MODULE_NAME
#define MODULE_NAME	"ThreadModule"
#endif

#define PROC_DIRECTORY	"threadDevice"
#define PROC_FILENAME	"thm"
#define BUFFER_SIZE 40
#define POOL_SIZE 3

static char *data_buff;
static char *work;

static struct const proc_dir_entry * proc_dir = proc_mkdir(PROC_DIRECTORY,
								NULL);

static struct const proc_dir_entry * proc_file = proc_create(PROC_FILENAME,
								0644,
								proc_dir,
								&proc_fops);

static size_t data_size;

static int proc_read(struct file *file_p,
					 char __user *buffer,
					 size_t length,
					 loff_t *offset);
static int stop;
static int i = POOL_SIZE;

static struct const file_operations proc_fops = {
	.read = proc_read,
	.write = proc_write
};

static struct semaphore workers;
static struct mutex data_lock;
static struct mutex count_lock;
static struct mutex task_lock;

static struct task_struct *master_thread;
static struct task_struct *worker_thread[POOL_SIZE];

static struct completion new_data;

//=================================================================
//======================BUFFER OPERATIONS=========================
//=================================================================

static int buffer_create(void)
{
	data_buff = kzalloc(BUFFER_SIZE, GFP_KERNEL);

	if (data_buff == NULL)
		return -ENOMEM;

	printk(KERN_INFO "ThM : Memory allocated\n");

	return 0;
}

static void buffer_clean(void)
{
	kfree(data_buff);
	data_buff = NULL;

	printk(KERN_INFO "ThM : Memory freed\n");
}

//==================================================================
//==========================TASK FUNCTIONS==========================
//==================================================================

static void print_task(char *symbol, int block_len, int block_count)
{
	int i;
	int j;
	char *block = kzalloc(sizeof(char) * block_len, GFP_KERNEL);

	printk(KERN_INFO Worker pid : % d", current->pid);

	for (j = 0; j < block_len; j++)
		strcpy(block + j, symbol);

	for (i = 0; i < block_count; i++) {
		strcpy(block + block_len, "\0");
		printk(KERN_INFO Block %d:\n %s", i+1, block);
		printk(KERN_INFO \n");
	}

	up(&workers);

}

static void implement_task(char *task)
{
	char symbol[1];
	long block_len;
	long block_count;
	char *tmp = kzalloc(sizeof(char) * strlen(task), GFP_KERNEL);

	strncpy(symbol, task, 1);

	strncpy(tmp, task + 2, 1);
	block_len = simple_strtol(tmp, NULL, 10);

	strncpy(tmp, task + 4, 1);
	block_count = simple_strtol(tmp, NULL, 10);

	print_task(symbol, block_len, block_count);
}

//==================================================================
//==========================THREADS FUNCTIONS=======================
//==================================================================

static int worker_fun(void *args)
{
	mutex_lock(&task_lock);
	work = (char *)args;
	implement_task(work);
	mutex_unlock(&task_lock);
	return 0;
}

static int master_fun(void *args)
{
	char task[6];
	printk(KERN_INFO "ThM : Master thread initialized\n");

	while (!kthread_should_stop()) {

		wait_for_completion(&new_data);
		mutex_lock(&data_lock);
		strncpy(task, data_buff, 6);

		if (stop)
			break;

		reinit_completion(&new_data);

		printk(KERN_INFO Master Thread : Data received");

		printk(KERN_INFO Master Thread : Calling worker");
		down(&workers);
		worker_thread[i] = kthread_run(worker_fun,
							task,
							"worker_thread");

		mutex_unlock(&data_lock);

		mutex_lock(&count_lock);
		i--;
		mutex_unlock(&count_lock);

		printk(KERN_INFO Master Thread : Data proccessed");
	}
	return 0;
}

static void stop_threads(void)
{
	stop = 1;

	complete(&new_data);

	kthread_stop(master_thread);

	printk(KERN_INFO "ThM: Master thread stopped\n");
}

//==================================================================
//=======================PROC FS OPERATIONS=========================
//==================================================================

static int create_proc(void)
{

	if (proc_dir == NULL)
		return -EFAULT;

	if (proc_file == NULL)
		return -EFAULT;

	printk(KERN_INFO "ThM: Created procfs interface\n");

	return 0;
}

static void cleanup_proc(void)
{
	if (proc_file) {
		remove_proc_entry(PROC_FILENAME, proc_dir);
		proc_file = NULL;
	}
	if (proc_dir) {
		remove_proc_entry(PROC_DIRECTORY, NULL);
		proc_dir = NULL;
	}

	printk(KERN_INFO "ThM: Removed procfs interface\n");
}

static int proc_write(struct file *filp,
					  const char *buf,
					  size_t count,
					  loff_t *offp)
{
	int err;

	data_size = count;

	if (data_size > BUFFER_SIZE)
		data_size = BUFFER_SIZE;

	mutex_lock(&data_lock);
	err = raw_copy_from_user(data_buff, buf, data_size);
	complete(&new_data);
	mutex_unlock(&data_lock);

	if (err)
		return -EFAULT;

	return data_size;
}

static int proc_read(struct file *filp,
					 char *buffer,
					 size_t len,
					 loff_t *offset)
{
	int result;
	char msg[BUFFER_SIZE];

	sprintf(msg, data_buff);

	if (*offset >= strlen(msg)) {
		*offset = 0;
		return 0;
	}

	if (len > strlen(msg) - *offset)
		len = strlen(msg) - *offset;

	mutex_lock(&data_lock);
	result = raw_copy_to_user((void *)buffer, msg - *offset, len);
	mutex_unlock(&data_lock);

	*offset += len;

	return len;
}

//==================================================================
//==========================INIT/EXIT FUNCTIONS=====================
//==================================================================

void threadModule_exit(void)
{

	stop_threads();

	cleanup_proc();

	buffer_clean();

	printk(KERN_INFO "ThM: Module unloaded\n");

}

int threadModule_init(void)
{
	int err;

	printk(KERN_INFO "ThM: Loading module...\n");

	err = buffer_create();

	if (err < 0) {
		printk(KERN_ALERT "ThM: Failed to allocate memory\n");
		goto abort;
	}

	err = create_proc();

	if (err < 0) {
		printk(KERN_ALERT "ThM: Failed to create procfs interface\n");
		goto abort;
	}

	sema_init(&workers, POOL_SIZE);
	printk(KERN_INFO "ThM: Semaphore initialized\n");

	mutex_init(&data_lock);

	init_completion(&new_data);

	master_thread = kthread_run(master_fun, NULL, "master_thread");

	printk(KERN_INFO "ThM: Module loaded\n");

	return 0;

abort:

	threadModule_exit();

	return -1;
}

module_init(threadModule_init);
module_exit(threadModule_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Bogdan Shchogolev <bogdan993000@gmail.com>");
MODULE_VERSION("1.0");
