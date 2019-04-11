#include <linux/cdev.h>		// For cdev struct
#include <linux/device.h>	// For module class
#include <linux/fs.h>		// Fops
#include <linux/init.h>		// Indicates initialization and cleanup functions	
#include <linux/module.h>	// Module init, exit functions
#include <linux/uaccess.h>	// copy_to_user, copy_from_user functions
#include <linux/slab.h>		// kzalloc, kfree functions

MODULE_AUTHOR("Dima.Moruha <trluxsus@gmail.com>");
MODULE_DESCRIPTION("Character device driver");
MODULE_LICENSE("GPL");
MODULE_VERSION("0.1");

#define NUMBER_OF_DEVICES 1
#define DEVICE_NAME "chrdev-cdev"
#define CLASS_NAME	"chrdev"
#define MIN_BUFFER_SIZE	1024

static int is_open;
static int data_size;
static int BUFFER_SIZE = MIN_BUFFER_SIZE;
static unsigned char* data_buffer = NULL;

module_param(BUFFER_SIZE, int, 0660);

static int chrdev_open(struct inode *inodep, struct file *filep)
{
		if(is_open) {
				printk("chrdev: opened\n");
				goto fail_open;
		}

		is_open = 1;

		printk("chrdev: open\n");
		return 0;

fail_open:
		return -EBUSY;
}

static int chrdev_release(struct inode *inodep, struct file *filep)
{
		is_open = 0;
		printk("chrdev: release\n");
		return 0;
}

static ssize_t chrdev_read(struct file *file, char __user *data, size_t size, loff_t *offset) 
{
		int retval;
		
		printk(KERN_INFO "chrdev: read \n");

		if (count > data_size) count = data_size;

		retval = copy_to_user(buf, data_buffer, count);
		if (retval) {
				printk(KERN_INFO "chrdev: copy_to_user failed: %d\n", retval);
				goto fail_read;
		}
		data_size = 0;

		printk(KERN_INFO "chrdev: %d bytes read\n", count);
		return count;

fail_read:
		return -EFAULT;
}

static ssize_t chrdev_write(struct file *file, const char __user *data, size_t size, loff_t *offset) 
{
		int retval;


		printk(KERN_INFO "chrdev write to file \n");

		data_size = count;
		if (data_size > BUFFER_SIZE) data_size = BUFFER_SIZE;

		retval = copy_from_user(data_buffer, buf, data_size);
		if (retval) {
				printk(KERN_INFO"chrdev copy_from_user failed: %d\n", retval);
				goto fail_write;
		}

		printk(KERN_INFO "chrdev: %d bytes written\n", data_size);
		return data_size;

fail_write:
		return -EFAULT;
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

		data_size = 0;
		is_open = 0;

		data_buffer = kzalloc(buffer_size * sizeof(*data_buffer), GFP_KERNEL);
		if (!data_buffer) {
				result = -ENOMEM;
				printk(KERN_INFO"chrdev: failed to alloc buffer\n");
				goto fail_alloc_buffer;
		}

		printk(KERN_INFO "chrdev: init\n");

		result = alloc_chrdev_region(&dev, 0, NUMBER_OF_DEVICES, DEVICE_NAME);
		if(result < 0) {
				printk(KERN_INFO "chrdev: failed to alloc chrdev region\n");
				goto fail_alloc_chrdev_region;
		}

		chrdev_cdev = cdev_alloc();
		if(!chrdev_cdev) {
				result = -ENOMEM;
				printk(KERN_INFO "chrdev: failed to alloc cdev\n");
				goto fail_alloc_cdev;
		}

		cdev_init(chrdev_cdev, &chrdev_fops);
		
		result = cdev_add(chrdev_cdev, dev, 1);
		if(result < 0) {
				printk(KERN_INFO "chrdev: failed to add cdev\n");
				goto fail_add_cdev;
		}

		chrdev_class = class_create(THIS_MODULE, CLASS_NAME);
		if(!chrdev_class) {
				result = -EEXIST;
				printk(KERN_INFO "chrdev: failed to create class\n");
				goto fail_create_class;
		}
	
		if(!device_create(chrdev_class, NULL, dev, NULL, "chrdev_cdev%d", MINOR(dev))) {
				result = -EINVAL;
				printk(KERN_INFO "chrdev: failed to create device\n");
				goto fail_create_device;
		}

		data_buffer = kmalloc(BUFFER_SIZE, GFP_KERNEL);
		if(!data_buffer) {
				printk(KERN_INFO "failed to alloc cdev\n");
				return -ENOMEM;
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
fail_alloc_buffer:
		return result;
}

static void __exit chrdev_exit(void)
{
		printk(KERN_INFO "chrdev: exit\n");
		device_destroy(chrdev_class, dev);
		class_destroy(chrdev_class);
		cdev_del(chrdev_cdev);
		unregister_chrdev_region(dev, NUMBER_OF_DEVICES);
		kfree(data_buffer);
}

module_init(chrdev_init);
module_exit(chrdev_exit);
