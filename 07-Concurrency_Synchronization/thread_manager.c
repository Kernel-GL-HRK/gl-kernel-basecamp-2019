#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/slab.h>
#include <linux/sched.h>
#include <linux/uaccess.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Primerov Maxim <primerovmax@gmail.com>");
MODULE_VERSION("0.1");

#define BUFFER_SIZE 20
#define PROC_DIR "thread_manager"
#define PROC_FILENAME "thm"

static char *buff;

static struct proc_dir_entry *proc_dir;
static struct proc_dir_entry *proc_file;
static size_t size = 0;
static struct file_operations proc_fops;

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

        pr_info("thread_manager: memory allocated\n");

        return 0;

out:
        pr_err("thread manager: failed to allocate memory\n");
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

        pr_info("thread manager: created procfs interface\n");

        return 0;

out:
        pr_err("thread manager: failed to create procfs interface\n");
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

        if (raw_copy_from_user(buff, user_buffer, size)) {
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

        ret = raw_copy_to_user((void*)buffer, msg - *offp, len);

        *offp += len;

        return len;

out:
        return ret;
}

void __exit thread_manager_exit(void)
{
        remove_proc();

        buffer_clean();

        pr_info("thread manager: module exited\n");
}

int __init thread_manager_init(void)
{
        buffer_create();

        proc_fops.read = proc_read;
        proc_fops.write = proc_write;

        create_proc();

        pr_info("thread manager: module loaded\n");

        return 0;
}

module_init(thread_manager_init);
module_exit(thread_manager_exit);
