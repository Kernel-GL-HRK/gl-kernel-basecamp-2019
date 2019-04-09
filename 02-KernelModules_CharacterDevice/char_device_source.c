#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/sched.h>
#include <asm/uaccess.h>
#include <linux/slab.h>
#include <linux/moduleparam.h>
#include <linux/device.h>
#include <linux/module.h>
#include <linux/sysfs.h>

#ifndef MODULE_NAME
#define MODULE_NAME 	"ChD"
#endif

#define BUFFER_SIZE	1024 
#define DEVICE_NAME 	"charDevice"
#define CLASS_NAME	"chDClass"
#define PROC_DIRECTORY 	"charDevice"
#define PROC_FILENAME 	"chd"

int buffer_increase = 0;

module_param(buffer_increase, int, 0220);

static int buffer_size = BUFFER_SIZE;
static char *data_buff;
static int is_open = 0;
static int used_memory = 0;
static int major;
static int clear_memory = 0;

static struct class* chdClass = NULL;
static struct device* chDevice = NULL;
static struct proc_dir_entry *proc_dir;
static struct proc_dir_entry *proc_file;
static struct kobject *chd_kobject;

static int proc_read(struct file *file_p, char __user *buffer, size_t length, loff_t *offset);

static struct file_operations proc_fops = {
	.read  = proc_read,
};

//===Buffer operations===

static int buffer_create(void)
{
    data_buff = kzalloc(buffer_size+buffer_increase, GFP_KERNEL);

    if (NULL == data_buff) 
        return -ENOMEM;

    return 0;
}

static void buffer_clean(void)
{
    if (data_buff) {
        kfree(data_buff);
        data_buff = NULL;
    }
}

//===/sys file operations===

static ssize_t sysfs_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buffer, size_t len)
{
	sscanf(buffer, "%d\n", &clear_memory);
	printk(KERN_INFO "ChD: clear_memory now = %d\n", clear_memory);
	
	if(clear_memory){
		memset(data_buff, 0, (int) (sizeof data_buff));
		kfree(data_buff);
		data_buff = kzalloc(buffer_size+buffer_increase, GFP_KERNEL);
		used_memory=0;
		printk(KERN_INFO "ChD: Cleared memory\n");
	}
	return len;
}

static struct kobj_attribute sysfs_attribute =__ATTR(clear_memory, 0220, NULL, sysfs_store);

//===/proc file operations===

static int create_proc(void)
{
    proc_dir = proc_mkdir(PROC_DIRECTORY, NULL);
    if (NULL == proc_dir)
        return -EFAULT;

    proc_file = proc_create(PROC_FILENAME, S_IFREG | S_IRUGO | S_IWUGO, proc_dir, &proc_fops);
    if (NULL == proc_file)
        return -EFAULT;

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

static int proc_read(struct file *filp, char *buffer, size_t len, loff_t *offset )
{
	int result;
	char msg[124];
	
	sprintf(msg, "%d %d\n", buffer_size+buffer_increase, buffer_size+buffer_increase-used_memory);

	
	if(*offset >= strlen(msg)) {
		*offset=0;
		return 0;
	}
	
	if(len > strlen(msg) - *offset)
		len = strlen(msg) - *offset;

	result = raw_copy_to_user((void*)buffer, msg - *offset, len);
	
	*offset += len;
	
	return len;
}

//===/dev file operations===

int dev_open(struct inode *inode, struct file *filp)
{
	if(is_open){
		printk(KERN_ERR "ChD: Failed to open - device is busy");
		return -EBUSY;
	}
	is_open = 1;
	return 0;
}

int dev_release(struct inode *inode, struct file *filp)
{
	is_open = 0;
	return 0;
}

ssize_t dev_read(struct file *filep, char *buffer, size_t len, loff_t *offset)
{
	ssize_t retval = 0;

	int remain = buffer_size - (int) (*offset);
	if (remain == 0) {
		goto out;
	}

	if (len > remain) {
		len = remain;
	}

	if (raw_copy_to_user(buffer, data_buff + *offset, len)) {
		retval = -EFAULT;
		goto out;
	}
	*offset += len;
	retval = len;

	out:
		return retval;
}

ssize_t dev_write(struct file *filep, const char *buffer, size_t len, loff_t *offset)
{
	int error = 0;
	int remain = buffer_size - (int) (*offset);
	
	used_memory += len;
	
	if(0 == remain)
		return 0;
	if(used_memory > remain)
		return -EIO;
	
	error = raw_copy_from_user(data_buff + *offset, buffer, len);
	
	if(error){
		printk(KERN_INFO "ChD: Failed to receive %zu characters from the user\n", len);
		return -EFAULT;
	}
	else{
		printk(KERN_INFO "ChD: Received %zu characters from the user\n", len);
		offset += len;
		return len;

	}
}

static struct file_operations fops = {
	.open = dev_open,
	.read = dev_read,
	.write = dev_write,
	.release = dev_release,
};

//===init/exit functions===

static int __init ChD_init(void)
{
	int err;
	
	printk(KERN_INFO "Loading module...");

	major = register_chrdev(0, DEVICE_NAME, &fops);
	
	if(major < 0){
		printk(KERN_ALERT "ChD: Failed to register major number\n");
		return major;
	}
	else{
		printk(KERN_INFO "ChD: Registered major number %d\n", major);
	}
	
	chdClass = class_create(THIS_MODULE, CLASS_NAME);

	chDevice = device_create(chdClass, NULL, MKDEV(major, 0), NULL, DEVICE_NAME);

	printk(KERN_INFO "ChD: Device initialized succesfully\n");

	buffer_create();
	
	create_proc();
	
	chd_kobject = kobject_create_and_add(MODULE_NAME, kernel_kobj);
	
	err = sysfs_create_file(chd_kobject, &sysfs_attribute.attr);
	
	if(err)
		return err;

	return 0;

}

static void __exit ChD_exit(void)
{
	kobject_put(chd_kobject);
	
	cleanup_proc();
	
	buffer_clean();
	
	device_destroy(chdClass, MKDEV(major,0));
	
	class_unregister(chdClass);
	
	class_destroy(chdClass);
	
	unregister_chrdev(major, DEVICE_NAME);
	
	printk(KERN_INFO "ChD: Module unloaded\n");
}

module_init(ChD_init);
module_exit(ChD_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Bogdan Shchogolev <bogdan993000@gmail.com>");
MODULE_DESCRIPTION("Simple character device module");
MODULE_VERSION("1.0");

