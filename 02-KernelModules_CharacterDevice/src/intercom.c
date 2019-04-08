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
