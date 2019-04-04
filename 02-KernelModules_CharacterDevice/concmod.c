#include <linux/kernel.h>
#include <linux/module.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Maxim Primerov <primerovmax@gmail.com");
MODULE_DESCRIPTION("Testing linux character device");
MODULE_VERSION("0.1");

static int __init cdev_init(void)
{
	printk("Module init\n");
	return 0;
}

static void __exit cdev_exit(void)
{
	printk("Module exit\n");
}

module_init(cdev_init);
module_exit(cdev_exit);