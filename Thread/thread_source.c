#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/sched.h>
#include <asm/uaccess.h>
#include <linux/slab.h>
#include <linux/device.h>
#include <linux/kthread.h>
#include <linux/mutex.h>
#include <linux/delay.h>
#include <linux/string.h>
#include <linux/semaphore.h>
#include <linux/module.h>

#ifndef MODULE_NAME
#define MODULE_NAME 	"ThreadModule"
#endif

#define PROC_DIRECTORY 	"threadDevice"
#define PROC_FILENAME 	"thm"
#define POOL_SIZE 3
#define BUFFER_SIZE 40
#define N 6
static char *data_buff;
static char *workman;

static struct proc_dir_entry *proc_dir;
static struct proc_dir_entry *proc_file;
static size_t data_size = 0;

static int proc_read(struct file *file_p, char __user *buffer, size_t length, loff_t *offset);
static int stay = 0;
static int i = POOL_SIZE;

static struct file_operations proc_fops;
static struct semaphore operator;
static struct mutex data_lock;
static struct task_struct *master_thread;
static struct task_struct *operating_thread[POOL_SIZE];
static struct completion new_data;
static struct mutex count_lock;
static struct mutex task_lock;


static int buffer_create(void)
{
    data_buff = kzalloc(BUFFER_SIZE, GFP_KERNEL);

    if (NULL == data_buff) {
        return -ENOMEM;
    }
 	printk(KERN_INFO "THM: Memory allocated\n");
	return 0;
}

/==================================================================
//==========================TASK FUNCTIONS==========================
//==================================================================

static void print_task(char *symbol, int block_len, int block_count)
{
	int i;
	int j;
	printk("Worker PID : %d", current->pid);

	for(i = 0; i < block_count; i++) {
		printk("blocks %d:", i+1);
		for(z = 0; z < block_len; z++) {
			printk("%c", *symbol);
		}
		printk("\n");
	}
	up(&operator);

	i++;
}

static void implement_task(char *task)
{
	char symbol[1];
	long block_len;
	long block_count;
	char tmp[5];

	printk("%s", task);

	strncpy(symbol, task, 1);
	printk("%s", symbol);

	strncpy(tmp, task + 2, 1);
	printk("%s", tmp);
	block_len = simple_strtol(tmp, NULL, 10);

	strncpy(tmp, task + 4, 1);
	block_count = simple_strtol(tmp, NULL, 10);
	printk("%s", tmp);

	print_task(symbol, block_len, block_count);
}

//==================================================================
//=======================PROC FS OPERATIONS=========================
//==================================================================
//==================================================================
//==========================THREADS FUNCTIONS=======================
//==================================================================

static int WORKER(void *args)
{
	mutex_lock(&task_lock);
	workman = (char *)args;
	implement_task(workman);
	mutex_unlock(&task_lock);
	return 0;
}

//==========================MASTER FUNCTIONS=======================
//==================================================================

static int MASTER(void *args)
{
	char task[N];
	
	printk(KERN_INFO "ThM: MASTER THREAD TO INITIALIZED\n");

	while(!kthread_should_stop()) {
		wait_for_completion(&new_info);
		reinit_completion(&new_info);
		if(stay)
		break;
		printk("MASTER THREAD: Data Received");
		mutex_lock(&data_lock);
		strncpy(task, data_buff, N);
		mutex_unlock(&data_lock);
		printk("MASTER THREAD: TO CALL Worker");
		down(&operator);
		operating_thread[j] = kthread_run(WORKER, task, "operating_thread");
		mutex_lock(&count_lock);
		j--;
		mutex_unlock(&count_lock);
		printk("MASTER THREAD: DATA PROCCESSED");
	}
	return 0;
}

static void STOP(void)
{
	stay = 1;

	complete(&new_info);

	kthread_stop(master_thread);

	printk(KERN_INFO "ThM: MASTER TH STOPPED \n");
}



static void buffer_clean(void)
{
    if (data_buff) {
        kfree(data_buff);
        data_buff = NULL;
    }
	printk(KERN_INFO "ThM: Memory freed\n");
}

static int create_proc(void)
{
    proc_dir = proc_mkdir(PROC_DIRECTORY, NULL);
    if (NULL == proc_dir){
        return -EFAULT;
	}

    proc_file = proc_create(PROC_FILENAME, S_IFREG | S_IRUGO | S_IWUGO, proc_dir, &proc_fops);

    if (NULL == proc_file){
        return -EFAULT;
	}
	printk(KERN_INFO "ThM: Created procfs interface\n");
	return 0;
}

static void cleanup_proc(void)
{
    if (proc_file)
    {
        remove_proc_entry(PROC_FILENAME, proc_dir);
        proc_file = NULL;
    }
    if (proc_dir)
    {
        remove_proc_entry(PROC_DIRECTORY, NULL);
        proc_dir = NULL;
    }
	printk(KERN_INFO "ThM: Removed procfs interface\n");
}

static int proc_write(struct file *filp, const char *buf, size_t count, loff_t *offp)
{
	int err;
	data_size = count;

	if(data_size > BUFFER_SIZE){
		data_size = BUFFER_SIZE;
	}
	mutex_lock(&data_lock);
	err = raw_copy_from_user(data_buff, buf, data_size);
	complete(&new_info);
	mutex_unlock(&data_lock);
	if(err) {
	return -EFAULT;
	}
	
	return data_size;
}

static int proc_read(struct file *filp, char *buffer, size_t len, loff_t *offset)
{
	int result;
	char msg[BUFFER_SIZE];

	sprintf(msg, data_buff);

	if(*offset >= strlen(msg)){
		*offset = 0;
		return 0;
	}

	if(len > strlen(msg) - *offset){
		len = strlen(msg) - *offset;
	}
	mutex_lock(&data_lock);
	result = raw_copy_to_user((void*)buffer, msg - *offset, len);
	mutex_unlock(&data_lock);

	*offset += len;

	return len;
}

void threadModule_exit(void)
{

	STOP();

	cleanup_proc();

	buffer_clean();

	printk(KERN_INFO "ThM: Module unloaded\n");
	
}

int threadModule_init(void)
{
	int err;

	printk(KERN_INFO "ThM: Loading module...\n");

	err = buffer_create();

	if(err < 0){
		printk(KERN_ALERT "ThM: Failed to allocate memory\n");
		goto abort;
	}

	proc_fops.read = proc_read;
	proc_fops.write = proc_write;

	err = create_proc();

	if(err < 0){
		printk(KERN_ALERT "ThM: Failed to create procfs interface\n");
		goto abort;
	}

	sema_init(&operator, POOL_SIZE);
	printk(KERN_INFO "ThM: Semaphore initialized\n");

	mutex_init(&data_lock);

	init_completion(&new_info);

	master_thread = kthread_run(MASTER, NULL, "master_thread");

	printk(KERN_INFO "ThM: Module loaded\n");

	return 0;
	abort:

	threadModule_exit();

	return -1;

}

module_init(threadModule_init);
module_exit(threadModule_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Dmytro Kalynenko <dmytrokalynenko@gmail.com>");
MODULE_VERSION("1.0");
