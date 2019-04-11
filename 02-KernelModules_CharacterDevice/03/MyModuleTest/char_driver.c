#include <linux/module.h>	//for module_init/module_exit

static int __init init(void)
{
	printk("Driver loaded!\n");
	return 0;
}

static void __exit exit(void)
{
	printk("Driver removed!\n");
}

module_init(init);
module_exit(exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Ivan Hudzinskyi <ivanhudzinskyi2000@gmail.com>");
MODULE_DESCRIPTION("Simple character device driver");
MODULE_VERSION("0.1");
