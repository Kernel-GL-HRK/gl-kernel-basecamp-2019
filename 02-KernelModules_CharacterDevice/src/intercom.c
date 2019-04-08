#include <linux/module.h>
#include <linux/init.h>
#include <linux/version.h>
#include <linux/types.h>
#include <linux/cdev.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <linux/kernel.h>
#include <linux/device.h>
#include <linux/slab.h>
#include <linux/proc_fs.h>
#include <linux/sched.h>
#include <linux/sysfs.h>
#include <linux/string.h>

#define DEV_NAME	"intercom"
#define CLASS_NAME	"GL"
#define ATTR_NAME	clear_buf
#define PROC_FNAME	"bufstate"
#define PROC_BUF_SIZE	32
#define MIN_BUF_SIZE	1024
#define ASCII_MIN	0
#define ASCII_MAX	127

static size_t buf_size = MIN_BUF_SIZE;
module_param(buf_size, int, 0660);

static char *message;
static short msg_size;

static dev_t chrdev;
static struct class *intercom_cls;
static struct cdev intercom_dev;

static int create_buffer(void)
{
	if (buf_size < MIN_BUF_SIZE) {
		pr_warn("Increasing buffer size to %u bytes\n", MIN_BUF_SIZE);
		buf_size = MIN_BUF_SIZE;
	}
	message = kmalloc(buf_size, GFP_KERNEL);
	if (message == NULL)
		return -ENOMEM;
	msg_size = 0;
	return 0;
}

static void cleanup_buffer(void)
{
	if (message) {
		kfree(message);
		message = NULL;
	}
	msg_size = 0;
}

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
