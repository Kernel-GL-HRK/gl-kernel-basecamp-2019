#include <linux/cdev.h>		// For cdev struct
#include <linux/device.h>	// For module class
#include <linux/fs.h>		// Fops
#include <linux/init.h>		// Indicates initialization and cleanup functions	
#include <linux/module.h>	// Module init, exit functions

MODULE_AUTHOR("Dima.Moruha <trluxsus@gmail.com>");
MODULE_DESCRIPTION("Character device driver");
MODULE_LICENSE("GPL");
MODULE_VERSION("0.1");

#define NUMBER_OF_DEVICES 1
#define DEVICE_NAME "chrdev-cdev"
#define CLASS_NAME	"chrdev"

static int chrdev_open(struct inode *inodep, struct file *filep)
{
		printk("chrdev: open\n");
		return 0;
}

static int chrdev_release(struct inode *inodep, struct file *filep)
{
		printk("chrdev: release\n");
		return 0;
}

static ssize_t chrdev_read(struct file *file, char __user *data, size_t size, loff_t *offset) 
{
		printk(KERN_INFO "chrdev: read\n");
		return count;
}

static ssize_t chrdev_write(struct file *file, const char __user *data, size_t size, loff_t *offset) 
{
		printk(KERN_INFO "chrdev: write\n");
		return size;
}

static struct file_operations chrdev_fops = {
		.owner = THIS_MODULE,
		.read = chrdev_read,
		.write = chrdev_write,
		.open = chrdev_open,
		.release = chrdev_release
};


static dev_t dev;
static struct cdev* chrdev_cdev;
static struct class* chrdev_class;

static int __init chrdev_init(void) 
{
		int result;
		
		is_open = 0;
		data_size = 0;

		printk(KERN_DEBUG "chrdev: init\n");

		result = alloc_chrdev_region(&dev, 0, NUMBER_OF_DEVICES, DEVICE_NAME);
		if(result < 0) {
				printk(KERN_ERR "chrdev: failed to alloc chrdev region\n");
				goto fail_alloc_chrdev_region;
		}

		chrdev_cdev = cdev_alloc();
		if(!chrdev_cdev) {
				result = -ENOMEM;
				printk(KERN_ERR "chrdev: failed to alloc cdev\n");
				goto fail_alloc_cdev;
		}

		cdev_init(chrdev_cdev, &chrdev_fops);
		
		result = cdev_add(chrdev_cdev, dev, 1);
		if(result < 0) {
				printk(KERN_ERR "chrdev: failed to add cdev\n");
				goto fail_add_cdev;
		}

		chrdev_class = class_create(THIS_MODULE, CLASS_NAME);
		if(!chrdev_class) {
				result = -EEXIST;
				printk(KERN_ERR "chrdev: failed to create class\n");
				goto fail_create_class;
		}
	
		if(!device_create(chrdev_class, NULL, dev, NULL, "chrdev_cdev%d", MINOR(dev))) {
				result = -EINVAL;
				printk(KERN_ERR "chrdev: failed to create device\n");
				goto fail_create_device;
		}

		return 0;

fail_create_device:
		class_destroy(chrdev_class);
fail_create_class:
		cdev_del(chrdev_cdev);
fail_add_cdev:
fail_alloc_cdev:
		unregister_chrdev_region(dev, 1);
fail_alloc_chrdev_region:
		return result;
}

static void __exit chrdev_exit(void)
{
		printk(KERN_DEBUG "chrdev: exit\n");
		device_destroy(chrdev_class, dev);
		class_destroy(chrdev_class);
		cdev_del(chrdev_cdev);
		unregister_chrdev_region(dev, NUMBER_OF_DEVICES);
}

module_init(chrdev_init);
module_exit(chrdev_exit);
