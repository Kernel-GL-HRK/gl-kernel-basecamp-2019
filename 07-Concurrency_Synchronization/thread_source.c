#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/sched.h>
#include <asm/uaccess.h>
#include <linux/slab.h>
#include <linux/device.h>
#include <linux/module.h>

#ifndef MODULE_NAME
#define MODULE_NAME 	"ThreadModule"
#endif

#define PROC_DIRECTORY 	"threadDevice"
#define PROC_FILENAME 	"thm"
#define BUFFER_SIZE 20

static char *data_buff;

static struct proc_dir_entry *proc_dir;
static struct proc_dir_entry *proc_file;
static size_t data_size = 0;

static int proc_read(struct file *file_p, char __user *buffer, size_t length, loff_t *offset);

static struct file_operations proc_fops;


static int buffer_create(void)
{
    data_buff = kzalloc(BUFFER_SIZE, GFP_KERNEL);

    if (NULL == data_buff) 
        return -ENOMEM;
	
	printk(KERN_INFO "ThM: Memory allocated\n");
	
    return 0;
}

static void buffer_clean(void)
{
    if (data_buff) {
        kfree(data_buff);
        data_buff = NULL;
    }
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
}

static int proc_write(struct file *filp, const char *buf, size_t count, loff_t *offp)
{
	data_size = count;
	
	if(data_size > BUFFER_SIZE){
		data_size = BUFFER_SIZE;
	}
	
	if(raw_copy_from_user(data_buff, buf, data_size)){
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
	
	result = raw_copy_to_user((void*)buffer, msg - *offset, len);
	
	*offset += len;
	
	return len;
}

void threadModule_exit(void)
{
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

