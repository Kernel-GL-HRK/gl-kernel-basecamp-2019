#include <linux/module.h>	//for module_init/module_exit
#include <linux/module.h>	//module_init/module_exit
#include <linux/device.h>	//class, device structs
#include <linux/types.h>	//dev_t
#include <linux/fs.h>		//alloc_chrdev_region, unregister_chrdev_region

#define CLASS_NAME "chdev_class"
#define DEVICE_NAME "chdev"
#define NUMBER_OF_DEVICES 1

static struct class *chdev_class;
static struct device *chdev;
static dev_t dev_num;

static int __init init(void)
{
	alloc_chrdev_region(&dev_num, 0, NUMBER_OF_DEVICES, DEVICE_NAME);
	chdev_class = class_create(THIS_MODULE, CLASS_NAME);
	chdev = device_create(chdev_class, NULL, dev_num, NULL, DEVICE_NAME);

	printk("Driver loaded!\n");
	return 0;
}

static void __exit exit(void)
{
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

