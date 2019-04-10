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

/*
 * Open device
 */
int chrdev_open(struct inode *inode, struct file *filp)
{
	return 0;
}

/*
 * Free device
 */
int chrdev_release(struct inode *inode, struct file *filp)
{
	return 0;
}

/*
 * Read device
 */
ssize_t chrdev_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos)
{
	return 0;
}

/*
 * Write device
 */
ssize_t chrdev_write(struct file *filp, const char __user *buf, size_t count, loff_t *f_pos)
{
	return 0;
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

}

/*
 * Init module and driver
 */
int __init chrdev_init(void)
{
	return 0;
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
