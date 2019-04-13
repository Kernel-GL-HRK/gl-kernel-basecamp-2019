#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/err.h>
#include <linux/uaccess.h>
#include <linux/kdev_t.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/stat.h>
#include <linux/proc_fs.h>
#include <linux/proc_fs.h>
#include <linux/sched.h>
#include <asm/uaccess.h>
#include <linux/slab.h>

/********************************* Global varables and definitions ********************************/

#define CLASS_NAME  "my_chardev_class"
#define DEVICE_NAME "my_chardev"
#define DEFAULT_SIZE_BUFF 1024
#define DEVICE_OPEN 	  1
#define DEVICE_CLOSE	  0


static int is_device_open;
static int major;

static struct class *pclass;
static struct device *pdev;

static int buffer_size;

static struct data_info{
	int   crnt_size_buff;
	int   size_buff;
	char* data_buff;
} dev_data_file;

static struct proc_info{
	int  data_length;
	char data_buff[10];
} proc_data_file;

static struct device_attribute my_dev_attr;

static struct proc_dir_entry *proc_file;

/**************************************************************************************************/


/************************************** Interface for /proc ***************************************/

static int proc_read(struct file *file_p, char __user *buffer, size_t length, loff_t *offset)
{ 
	int len = sprintf(proc_data_file.data_buff,"%d \n", dev_data_file.crnt_size_buff);
    if( length < len ) return -EINVAL; 
    if( *offset != 0 ) { 
       return 0; 
    } 
    if( copy_to_user( buffer, proc_data_file.data_buff, len ) ) return -EINVAL; 
    *offset = len; 
    return len; 
}

static struct file_operations proc_fops = {
    .read  = proc_read,
    .write = NULL,
};

/**************************************************************************************************/


/**************************************  Interface for /sys ***************************************/

static ssize_t sysfs_write( struct class *class, struct class_attribute *attr, const char *buf, size_t length )
{
	memset(dev_data_file.data_buff,0,dev_data_file.size_buff);
	dev_data_file.crnt_size_buff = dev_data_file.size_buff;
	printk(KERN_INFO "Buffer was cleanup\n");
    return length;
} 

struct class_attribute sysfs = __ATTR(cleanup, 0664, NULL, sysfs_write);

/**************************************************************************************************/


/**************************************  Interface for /dev ***************************************/

static int dev_open(struct inode *i, struct file *f)
{
	if (is_device_open) {
		pr_err("chardev already open\n");
		return -EBUSY;
	}
	is_device_open = DEVICE_OPEN;
    printk(KERN_INFO "chardev open\n");
    return 0;
}

static int dev_close(struct inode *i, struct file *f)
{
    is_device_open = DEVICE_CLOSE;
	printk(KERN_INFO "chardev closed\n");
	return 0;
}

static ssize_t dev_read(struct file *file_p, char __user *buffer, size_t length, loff_t *offset)
{
	int retval;

	int len = strlen(dev_data_file.data_buff); 
    if( length < len ) return -EINVAL; 
    if( *offset != 0 ) { 
       printk( KERN_INFO "=== read return : 0\n" );  
       return 0; 
    } 
    retval = copy_to_user(buffer, dev_data_file.data_buff, len);
    if(retval){
    	pr_err("chrdev: copy_to_user failed: %d\n", retval);
    	return -EINVAL;
    }
    *offset = len; 
    return len; 
}

static ssize_t dev_write(struct file *f, const char __user *buffer, size_t length, loff_t *offset)
{
   	int retval;

	if (length > dev_data_file.crnt_size_buff) 
		length = dev_data_file.crnt_size_buff;

	retval = copy_from_user(&dev_data_file.data_buff[dev_data_file.size_buff - dev_data_file.crnt_size_buff], buffer, length);
	if (retval) {
		pr_err("chrdev: copy_from_user failed: %d\n", retval);
		return -EFAULT;
	}
	dev_data_file.crnt_size_buff -= length;
	return length;
}

static struct file_operations fops =
{
	.owner = THIS_MODULE,
    .open = dev_open,
    .release = dev_close,
    .read = dev_read,
    .write = dev_write
};

/**************************************************************************************************/

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Daniel.Shami <danismi715@gmail");
MODULE_DESCRIPTION("Character device driver");                                                                                   
MODULE_VERSION("0.1");