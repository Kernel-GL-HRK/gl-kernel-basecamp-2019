#include "concdev.h"
#include <linux/fs.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/cdev.h>
#include <asm/uaccess.h>

#define DEVICE_NAME "concdev"

static int major_number;
static int minor_number;
static int count_dev;

static struct cdev *cdev;

static char *buffer;
static size_t size_buffer;

static struct file_operations fops =
{
	.owner = THIS_MODULE,
	.open = cdev_open,
	.release = cdev_release,
	.read = cdev_read,
	.write = cdev_write,
};

int create_cdevice(int major, int minor, int count, size_t size)
{
	major_number = major;
	minor_number = minor;
	count_dev = count;
	size_buffer = size;

	int result;
	dev_t dev = 0;

	if (major_number) {
		dev = MKDEV(major_number, minor_number);
		result = register_chrdev_region(dev, count_dev, DEVICE_NAME);
	} else {
		result = alloc_chrdev_region(&dev, minor_number, count_dev, DEVICE_NAME);
		major = MAJOR(dev);
	}

	if (result < 0) {
		printk("ERROR (CharDev): can't dev a majori\n");
		return result;
	}

	cdev = cdev_alloc();
 	if (!cdev) {
		result = -ENOMEM;
		goto fail; 
	}

	cdev_init(cdev, &fops);
	cdev->owner = THIS_MODULE;
	if (cdev_add(cdev, dev, count)) { 
		printk("ERROR (CharDev): cdev_add error\n");
	}
	printk("INFO (CharDev): major_number %d minor_number %d\n", major, minor);
	
	buffer = kzalloc(size, GFP_KERNEL);
	if (!buffer) {
		result = -ENOMEM;
		goto fail;
	}

	return 0;
	
	fail:
		printk("ERROR (CharDev): register error\n");
		return result;  
}

void remove_cdevice(void)
{
	dev_t devno = MKDEV(major_number, minor_number);
	if (cdev) {
		cdev_del(cdev);
		cdev = NULL;
	}

	if (buffer) {
		kfree(buffer);
		buffer = NULL;
	}

	unregister_chrdev_region(devno, count_dev);

	printk("MESSAGE (CharDev): device remove\n");
}

int cdev_open(struct inode *inode, struct file *filp)
{
	return 0;
}

int cdev_release(struct inode *inode, struct file *filp)
{
	return 0;
}

ssize_t cdev_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos)
{
	ssize_t retval = 0;

	int remain = size_buffer - (int) (*f_pos);
	if (remain == 0) {
		goto out;
	}

	if (count > remain) {
		count = remain;
	}

	if (raw_copy_to_user(buf, buffer + *f_pos, count)) {
		retval = -EFAULT;
		goto out;
	}
	*f_pos += count;
	retval = count;

	out:
		return retval;
}

ssize_t cdev_write(struct file *filp, const char __user *buf, size_t count, loff_t *f_pos)
{
	ssize_t retval = 0;

	int remain = size_buffer - (int) (*f_pos);

	if (count > remain)
	{
		retval = -EIO;
		goto out;
	}

	if (raw_copy_from_user(buffer + *f_pos, buf, count)) {
		retval = -EFAULT;
		goto out;
	}
	*f_pos += count;
	retval = count;

	out:
		return retval;
}