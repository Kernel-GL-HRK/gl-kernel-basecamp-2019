#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/sched.h>
#include <asm/uaccess.h>
#include <linux/slab.h>
#include <linux/moduleparam.h>
#include <linux/device.h>
#include <linux/module.h>

#ifndef MODULE_NAME
#define MODULE_NAME 	"ChD"
#endif

#define BUFFER_SIZE 		1024 //обьем буфера
#define DEVICE_NAME 	"charDevice"
#define CLASS_NAME		"chDClass"

int buffer_increase = 0;

module_param(buffer_increase, int, 0220);

static int buffer_size = BUFFER_SIZE;
static char *data_buff;
static int is_open = 0;
static int used_memory = 0;

//===Buffer operations===

static int buffer_create(void)
{
    data_buff = kmalloc(buffer_size+buffer_increase, GFP_KERNEL);

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

//===/dev file operations===

int dev_open(struct inode *inode, struct file *filp)
{	
	if(is_open){
		printk(KERN_ERR "ChD: Filed to open - device is busy");
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

ssize_t dev_read(struct file *filep, char *buffer, size_t len, loff_t *offset){
	int error = 0;

	int remain = buffer_size - (int) (*offset);
	
	if(0 == remain)
		return 0;
	if(len > remain)
		len = remain;
	
	error = raw_copy_to_user(buffer, data_buff, len);
	
	
	if(error){
		printk(KERN_INFO "ChD: Failed to send %d characters\n", error);
		return -EFAULT;
	}
	else{
		*offset += len;
		return len;
	}
}

ssize_t dev_write(struct file *filep, const char *buffer, size_t len, loff_t *offset){

	int error = 0;
	int remain = buffer_size - (int) (*offset);
	
	used_memory = len;
	
	if(0 == remain)
		return 0;
	if(used_memory > remain)
		return -EIO;
	
	error = raw_copy_from_user(data_buff, buffer, used_memory);
	
	if(error){
		printk(KERN_INFO "ChD: Failed to receive %zu characters from the user\n", used_memory);
		return -EFAULT;
	}
	else{
		printk(KERN_INFO "ChD: Received %zu characters from the user\n", used_memory);
		offset += used_memory;
		return used_memory;

	}
}

static struct file_operations fops = {
	.open = dev_open,
	.read = dev_read,
	.write = dev_write,
	.release = dev_release,
};
