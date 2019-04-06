#include "concdev.h"
#include <linux/fs.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/cdev.h>

#define DEVICE_NAME "concdev"

static int major_number;
static int minor_number;
static int count_dev;

static struct cdev *cdev;

static struct file_operations fops =
{
	.owner = THIS_MODULE,
	.open = cdev_open,
	.release = cdev_release,
};

int  create_cdevice(int major, int minor, int count)
{
	major_number = major;
	minor_number = minor;
	count_dev = count;

	int result = 0;
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
