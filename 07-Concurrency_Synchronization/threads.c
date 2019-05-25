#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/slab.h>
#include <linux/sched.h>
#include <linux/uaccess.h>
#include <linux/string.h>
#include <linux/kthread.h>
#include <linux/mutex.h>
#include <linux/semaphore.h>
#include <linux/delay.h>


#define BUFFER_SIZE 512
#define PROC_DIR "threads"
#define PROC_FILENAME "Master"
#define POOL_SIZE 3

static char *buff;

static struct proc_dir_entry *proc_dir;
static struct proc_dir_entry *proc_file;
static size_t size = 0;
static struct file_operations proc_fops;

static struct semaphore workersS;
static struct mutex lock;
static struct mutex count_lock;
static struct mutex task_lock;
static struct task_struct *masterT;
static struct task_struct *workersT[POOL_SIZE];
static struct completion data;

static char *work;
static int stop = 0;
static int i = POOL_SIZE;

static void output(char symb, u32 len, u32 count)
{
        u32 i;
        u32 j;

        pr_info("thread : worker thread pid %d", current->pid);

        for (i = 0; i < count; ++i) {
                pr_info("thread : block %d: ", i + 1);
                for (j = 0; j < len; ++j) {
                        pr_info("%c", symb);
                }
                pr_info("\n");
        }

        up(&workersS);
        ++i;
}

static int implement(void *task)
{
        char symb;
        u64 len;
        u64 count;
        char temp[5];

        strncpy(&symb, (char*)task, 1);

        strncpy(temp, (char*)task + 2, 1);
        len = simple_strtol(temp, NULL, 10);

        strncpy(temp, (char*)task + 4, 1);
        count = simple_strtol(temp, NULL, 10);

        output(symb, len, count);

        return 0;
}

static int worker(void *args)
{
        mutex_lock(&task_lock);

        work = (char*)args;
        implement(work);

        mutex_unlock(&task_lock);

        return 0;
}

static int master(void *args)
{
        char task[6];
        pr_info("thread: master thread init\n");

        while (!kthread_should_stop()) {
                wait_for_completion(&data);
                reinit_completion(&data);
                if (stop) {
                        break;
                }
                pr_info("thread : thread data received\n");

                mutex_lock(&lock);
                strncpy(task, buff, 6);
                mutex_unlock(&lock);

                pr_info("thread manager: master thread calling worker\n");
                down(&workersS);
                workersT[i] = kthread_run(worker, task, "worker_thread");
                mutex_lock(&count_lock);
                --i;
                mutex_unlock(&count_lock);

                pr_info("thread : data proccessed\n");
        }

        return 0;
}

static void kthreads_stop(void)
{
        stop = 1;

        complete(&data);
        kthread_stop(masterT);

        pr_info("thread : thread stopped\n");
}


static void buffer_clean(void)
{
        if (buff) {
                kfree(buff);
                buff = NULL;
        }
}

static int buffer_create(void)
{
        int ret;

        buff = kzalloc(BUFFER_SIZE, GFP_KERNEL);

        if (NULL == buff) {
                ret = -ENOMEM;
                goto out;
        }

        pr_info("thread: memory allocated\n");

        return 0;

out:
        pr_err("thread : failed to allocate memory\n");
        buffer_clean();
        return ret;
}

static void remove_proc(void)
{
        if (proc_file) {
                remove_proc_entry(PROC_FILENAME, proc_dir);
                proc_file = NULL;
        }

        if (proc_dir) {
                remove_proc_entry(PROC_DIR, NULL);
                proc_dir = NULL;
        }
}

static int create_proc(void)
{
        int ret;

        proc_dir = proc_mkdir(PROC_DIR, NULL);
        if (NULL == proc_dir) {
                ret = -EFAULT;
                goto out;
        }

        proc_file = proc_create(PROC_FILENAME, S_IFREG | S_IRUGO | S_IWUGO, proc_dir, &proc_fops);
        if (NULL == proc_file) {
                ret = -EFAULT;
                goto out;
        }

        pr_info("thread: created procfs \n");

        return 0;

out:
        pr_err("thread: failed to create procfs \n");
        remove_proc();
        return ret;
}

static int proc_write(struct file *filp, const char *user_buffer, size_t count, loff_t *offp)
{
        int ret;
        size = count;

        if (size > BUFFER_SIZE) {
                size = BUFFER_SIZE;
        }
	mutex_lock(&lock);
	ret = raw_copy_from_user(buff,user_buffer,size);
	complete(&data);
	mutex_unlock(&lock);
        if (ret) {
                ret = -EFAULT;
                goto out;
        }

        return size;

out:
        return ret;
}

static int proc_read(struct file *filp, char *buffer, size_t len, loff_t *offp)
{
        int ret;
        char msg[BUFFER_SIZE];

        sprintf(msg, buff);

        if (*offp >= strlen(msg)) {
                *offp = 0;
                ret = 0;
                goto out;
        }

        if (len > strlen(msg) - *offp) {
                len = strlen(msg) - *offp;
        }
	
	mutex_lock(&lock);
        ret = raw_copy_to_user((void*)buffer, msg - *offp, len);
	mutex_unlock(&lock);

        *offp += len;

        return len;

out:
        return ret;
}

void __exit thread_exit(void)
{

	kthreads_stop();
        remove_proc();

        buffer_clean();

        pr_info("thread : module exited\n");
}

int __init thread_init(void)
{
	
	int ret;

	ret = buffer_create();
	if (ret) {
		goto out;
	}

	proc_fops.read = proc_read;
	proc_fops.write = proc_write;

	create_proc();
	ret = create_proc();
	if (ret) {
		goto out;
        }

	sema_init(&workersS, POOL_SIZE);
	pr_info("thread : semaphore init\n");

	mutex_init(&lock);

	init_completion(&data);

	masterT = kthread_run(master, NULL, "master_thread");

	pr_info("thread : module loaded\n");

        return 0;

out:
        return ret;
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Rubinshteyn Mark,mark.rubinshteyn@meta.ua");
MODULE_VERSION("1");

