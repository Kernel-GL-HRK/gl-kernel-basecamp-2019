#include <linux/module.h>
#include <linux/moduleparam.h>  /* command-line args */
#include <linux/kernel.h>       /* printk() */
#include <linux/slab.h>			/* kfree(), kmalloc() */
#include <linux/fs.h>           /* file_operations */
#include <linux/types.h>        /* dev_t */
#include <linux/cdev.h>
#include <asm/uaccess.h>        /* copy_*_user */

#include "chrdev_driver.h"

#ifndef MODULE_NAME
#define MODULE_NAME "chrdev_driver"
#endif

#define MODULE_MIN_BUFF_SIZE 1024

int chrdev_major = 0;		/* MAJOR */
int chrdev_minor = 0;		/* MINOR */
int chrdev_nr_devs = 1;		/* Number device for register */

unsigned int chrdev_buffer_size = 0;	
module_param(chrdev_buffer_size, uint, S_IRUGO);

struct cdev *cdev;
static char *chrdev_buffer;

/*
 * Open device
 */
int chrdev_open(struct inode *inode, struct file *filp)
{
	printk(KERN_INFO " chrdev: success open \n");
	return 0;
}

/*
 * Free device
 */
int chrdev_release(struct inode *inode, struct file *filp)
{
	printk(KERN_INFO " chrdev: success close \n");
	return 0;
}

/*
 * Read device
 */
ssize_t chrdev_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos)
{
	// return cmd
	ssize_t retval = 0;
	
	// number of bytes left to read
	int remain = chrdev_buffer_size - (int) (*f_pos); 
	
	if (remain == 0) 
		return 0;	
	if (count > remain)
		count = remain;

	if (raw_copy_to_user(buf, chrdev_buffer + *f_pos, count)) {
		retval = -EFAULT;
		printk(KERN_ERR "chrdev: bad address on copy to user");
		goto out;
	}
	*f_pos += count;
	retval = count;

	out:
		return retval;
}

/*
 * Write device
 */
ssize_t chrdev_write(struct file *filp, const char __user *buf, size_t count, loff_t *f_pos)
{
	// return cmd
	ssize_t retval = 0;
	
	// number of bytes left to write
	int remain = chrdev_buffer_size - (int) (*f_pos); 

	if (count > remain) {
		printk(KERN_ERR "chrdev: I/O error on write data");
		return -EIO;
	}

	if (raw_copy_from_user(chrdev_buffer + *f_pos, buf, count)) {
		retval = -EFAULT;
		printk(KERN_ERR "chrdev: bad address on copy from user");
		goto out;
	}
	
	*f_pos += count;
	retval = count;

	out:
		return retval;
}

/*
 * Driver for work with device
 */
struct file_operations chrdev_fops = {
	.owner =    THIS_MODULE,
	.read =     chrdev_read,
	.write =    chrdev_write,
	.open =     chrdev_open,
	.release =  chrdev_release,
};

/*
 * Free device and module
 */
void __exit chrdev_exit(void)
{
	dev_t devno = MKDEV(chrdev_major, chrdev_minor);
	if (cdev) {
		cdev_del(cdev);
		printk(KERN_INFO "chrdev: cdev removed from the system \n");
	}

	if (chrdev_buffer){ 
		kfree(chrdev_buffer);
		printk(KERN_INFO "chrdev: memory is free \n");
	}
	unregister_chrdev_region(devno, chrdev_nr_devs);

	printk(KERN_INFO "chrdev: exit \n");
}

/*
 * Init module and driver
 */
int __init chrdev_init(void)
{
	int result = 0;
	dev_t dev = 0;

	result = alloc_chrdev_region(&dev, chrdev_minor, chrdev_nr_devs, MODULE_NAME);
	chrdev_major = MAJOR(dev);

	if (result) {
		printk(KERN_ERR "chrdev: can't get major %d\n", chrdev_major);
		return result;
	}

	cdev = cdev_alloc();
 	
	 if (!cdev) {
		result = -ENOMEM;
		printk(KERN_ERR "chrdev: cdev is out of memory");
		goto fail; 
	}
	cdev_init(cdev, &chrdev_fops);
	cdev->owner = THIS_MODULE;
	if (cdev_add(cdev, dev, chrdev_nr_devs)) { 
		printk(KERN_ERR "chrdev: cdev_add error \n");
		goto fail;
	}
	printk(KERN_INFO "chrdev: %d:%d \n", chrdev_major, chrdev_minor);


	if(chrdev_buffer_size < MODULE_MIN_BUFF_SIZE){
		printk(KERN_ERR "chrdev: chrdev_buffer is minimal size 1024 byte");
		printk(KERN_INFO "chrdev: chrdev_buffer size is set 1024 byte");
		chrdev_buffer_size = MODULE_MIN_BUFF_SIZE;
	}
		chrdev_buffer = kzalloc(chrdev_buffer_size * sizeof (*chrdev_buffer), GFP_KERNEL);
	if (!chrdev_buffer) {
		result = -ENOMEM;
		printk(KERN_ERR "chrdev: chrdev_buffer is out of memory");
		goto fail;
	}
	
	return 0;
	
	fail:
		chrdev_exit();
		printk(KERN_ERR "chrdev: register error \n");
		return result;
}


/*
 * Free and Init module
 */
module_init(chrdev_init);
module_exit(chrdev_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR ("Alexandr.Datsenko<datsenkoalexander@gmail.com>");
MODULE_DESCRIPTION("Character driver");
MODULE_VERSION("0.1");

