#include <linux/module.h>

static int __init init(void)
{
	printk("Driver loaded\n");
	return 0;
}

static void __exit exit(void)
{
	printk("Driver deleted\n");
}

module_init(init);
module_exit(exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Oleksandr Korotetskyi <ayrum1158@gmail.com>");
MODULE_DESCRIPTION("myCharDev");
MODULE_VERSION("0.1");
