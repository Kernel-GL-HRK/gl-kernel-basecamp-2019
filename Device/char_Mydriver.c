#include <linux/module.h>	//module_init, module_exit
#include <linux/device.h>	//class, device structs
#include <linux/types.h>	//dev_t
#include <linux/fs.h>		//alloc_chrdev_region, unregister_chrdev_region
#include <linux/cdev.h>		//cdev struct
#include <linux/uaccess.h>	//copy_to_user, copy_from_user
#include <linux/slab.h>		//kzalloc, kfree

#define CLASS_NAME "chdev_class"
#define DEVICE_NAME "chdev"
#define NUMBER_OF_DEVICES 1
#define DEFALT_BUFFER_SIZE 1024

static unsigned char *buffer;

static int is_open;

static int alloc_size = DEFALT_BUFFER_SIZE;

module_param(alloc_size, int, 0660);

static int used_size;

static struct class *chdev_class;
static struct device *chdev;
static dev_t dev_num;

static struct cdev dev_struct;

static int chdev_open(struct inode *inode_p, struct file *file_p)
{
	if(is_open) {
		printk("Device alredy opened!");
		goto device_busy;
	}

	is_open = 1;

	printk("Open!\n");
	return 0;

device_busy:
	return -EBUSY;
}

static int chdev_release(struct inode *inode_p, struct file *file_p)
{
	is_open = 0;
	printk("Release!\n");
	return 0;
}

static ssize_t chdev_read(struct file *file_p, char __user *usr_buff, size_t len, loff_t *offset)
{
	if (len > used_size)
		len = used_size;

	if(*offset == 0)
	{
		if (copy_to_user(usr_buff, buffer, len) != 0) {
			goto copy_to_user_fail;
		} else {
			*offset += len;
			goto message_read;
		}
	}

	return 0;

message_read:
	printk("Reading %d characters", len);
	return len;

copy_to_user_fail:
	return -EFAULT;
}

static ssize_t chdev_write(struct file *file_p, const char __user *usr_buff, size_t len, loff_t *offset)
{
	used_size = len;

	if(used_size > alloc_size)
		used_size = alloc_size;

	if(copy_from_user(buffer, usr_buff, used_size) != 0)
		goto copy_from_user_fail;

	printk("Writing %d characters!\n", used_size);

	return len;

copy_from_user_fail:
	return -EFAULT;
}

static struct file_operations fops = {
	.owner = THIS_MODULE,
	.open = chdev_open,
	.release = chdev_release,
	.read = chdev_read,
	.write = chdev_write
};

static int __init init(void)
{
	is_open = 0;
	used_size = 0;

	if(alloc_size < DEFALT_BUFFER_SIZE) {
		printk("Buffer size must be at least 1KB!\n");
		goto buffer_too_small;
	}

	buffer = kzalloc(alloc_size, GFP_KERNEL);

	alloc_chrdev_region(&dev_num, 0, NUMBER_OF_DEVICES, DEVICE_NAME);
	chdev_class = class_create(THIS_MODULE, CLASS_NAME);
	chdev = device_create(chdev_class, NULL, dev_num, NULL, DEVICE_NAME);

	cdev_init(&dev_struct, &fops);
	cdev_add(&dev_struct, dev_num, NUMBER_OF_DEVICES);

	printk("Driver loaded!\n");
	return 0;

buffer_too_small:
	return -1;
}

static void __exit exit(void)
{
	kfree(buffer);

	cdev_del(&dev_struct);

	device_destroy(chdev_class, dev_num);
	class_destroy(chdev_class);
	unregister_chrdev_region(dev_num, NUMBER_OF_DEVICES);
	printk("Driver removed!\n");
}

module_init(init);
module_exit(exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Dmytro Kalynenko  <dmytrokalynenko@gmail.com>");
MODULE_DESCRIPTION("Simple character device driver");

