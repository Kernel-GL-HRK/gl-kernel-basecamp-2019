#include <linux/module.h>
#include <linux/init.h>

static int __init intercom_init(void)
{
	return 0;
}

static void __exit intercom_exit(void)
{

}

module_init(intercom_init);
module_exit(intercom_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Nikita Kvochka <yorkened@gmail.com>");
MODULE_DESCRIPTION("Simple char dev for text messaging between users");
MODULE_VERSION("0.1");
