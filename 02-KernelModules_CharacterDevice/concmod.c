#include <linux/kernel.h>
#include <linux/module.h>
#include "concdev.c"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Maxim Primerov <primerovmax@gmail.com");
MODULE_DESCRIPTION("Testing linux character device");
MODULE_VERSION("0.1");

static int major = 0;
static int minor = 0;
static int count = 1;
module_param(major, int, S_IRUGO);
module_param(minor, int, S_IRUGO);
module_param(count, int, S_IRUGO); 

static void __exit cmod_exit(void)
{
	remove_cdevice();
	printk("INFO (CONCDEV): module exit\n");
}

static int __init cmod_init(void)
{
	int status;
	printk("INFO (CONCDEV): module init\n");

	status = create_cdevice(major, minor, count);

	if (status < 0) {
		goto err;
	}

	return 0;

	err:
		cmod_exit();
		return status;
}

module_init(cmod_init);
module_exit(cmod_exit);