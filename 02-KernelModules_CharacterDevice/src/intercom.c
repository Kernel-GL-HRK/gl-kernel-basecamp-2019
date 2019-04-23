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
static char proc_buffer[PROC_BUF_SIZE];
static const char fmt_str[PROC_BUF_SIZE] = "Buff state: %u/%u bytes\n";

static dev_t chrdev;
static struct class *intercom_cls;
static struct cdev intercom_dev;

static struct proc_dir_entry *proc_dir;
static struct proc_dir_entry *proc_file;

static ssize_t dev_read(struct file *, char __user *, size_t, loff_t *);
static ssize_t dev_write(struct file *, const char __user *, size_t, loff_t *);
static ssize_t procfs_read(struct file *, char __user *, size_t, loff_t *);
static ssize_t sysfs_show(struct class *, struct class_attribute *, char *);
static ssize_t sysfs_store(struct class *, struct class_attribute *, const char *, size_t);

static struct class_attribute sysfs_attr = __ATTR(ATTR_NAME, 0664, sysfs_show, sysfs_store);

static struct file_operations dev_fops = {
	.read = dev_read,
	.write = dev_write
};

static struct file_operations procfs_fops = {
	.read = procfs_read,
};

static int intercom_uevent(struct device *dev, struct kobj_uevent_env *env)
{
	if (add_uevent_var(env, "DEVMODE=%#o", 0666) != 0)
		return -ENOMEM;
	return 0;
}

static int string_checker(const char *string)
{
	int i;
	int len = strlen(string);

	for (i = 0; i < len; i++) {
		if (string[i] < ASCII_MIN)
			return -EINVAL;
	}
	return 0;
}

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

static int create_procfs(void)
{
	proc_dir = proc_mkdir(DEV_NAME, NULL);

	if (proc_dir == NULL)
		return -EFAULT;
	proc_file = proc_create(PROC_FNAME, 0444, proc_dir, &procfs_fops);

	if (proc_file == NULL)
		return -EFAULT;
	return 0;
}

static void cleanup_procfs(void)
{
	if (proc_file) {
		remove_proc_entry(PROC_FNAME, proc_dir);
		proc_file = NULL;
	}
	if (proc_dir) {
		remove_proc_entry(DEV_NAME, NULL);
		proc_dir = NULL;
	}
}

static ssize_t procfs_read(struct file *fileptr, char __user *buf,
	size_t len, loff_t *offset)
{
	len = PROC_BUF_SIZE;

	snprintf(proc_buffer, PROC_BUF_SIZE, fmt_str, msg_size, buf_size);
	return simple_read_from_buffer(buf, len, offset, proc_buffer, PROC_BUF_SIZE);
}

static ssize_t sysfs_show(struct class *cls, struct class_attribute *attr, char *buf)
{
	memset(message, 0, buf_size);
	msg_size = 0;
	pr_info("Buffer successfully cleared\n");
	return 0;
}

static ssize_t sysfs_store(struct class *cls, struct class_attribute *attr,
		const char *buf, size_t count)
{
	memset(message, 0, buf_size);
	msg_size = 0;
	pr_info("Buffer successfully cleared\n");
	return 0;
}

static ssize_t dev_read(struct file *fileptr, char __user *buf,
	size_t len, loff_t *offset)
{
	ssize_t status;

	if (len > msg_size)
		len = msg_size;
	status = simple_read_from_buffer(buf, len, offset, message, msg_size);

	if (status < 0)
		pr_err("Failed to read %u bytes\n", msg_size);
	else if (status > 0) {
		pr_info("Succefully read %d bytes\n", status);
		msg_size -= status;
	}
	return status;
}

static ssize_t dev_write(struct file *fileptr, const char __user *buf,
	size_t len, loff_t *offset)
{
	size_t msg_len;
	ssize_t status;

	status = string_checker(buf);
	if (status < 0) {
		pr_err("You should use only ASCII strings\n");
		return status;
	}

	if (len > buf_size) {
		pr_warn("Redusing message length from %u to %u bytes\n",
			len, buf_size);
		msg_len = buf_size;
	} else
		msg_len = len;
	status = simple_write_to_buffer(message, msg_len, offset, buf, len);

	if (status < 0)
		pr_err("Failed to write %u bytes\n", msg_len);
	else {
		pr_notice("Written %u bytes\n", status);
		msg_size = status;
	}
	return status;
}

static int __init intercom_init(void)
{
	int err;

	pr_info("Intercom: Initializing kernel module\n");
	err = alloc_chrdev_region(&chrdev, 0, 1, DEV_NAME);

	if (err < 0) {
		pr_alert("Failed to register dev major number\n");
		return err;
	}
	pr_info("Major number registered correctly [%d]\n", MAJOR(chrdev));
	intercom_cls = class_create(THIS_MODULE, CLASS_NAME);

	if (IS_ERR(intercom_cls)) {
		unregister_chrdev_region(chrdev, 1);
		pr_alert("Failed to register device class\n");
		return PTR_ERR(intercom_cls);
	}
	intercom_cls->dev_uevent = intercom_uevent;
	err = class_create_file(intercom_cls, &sysfs_attr);

	if (err < 0) {
		pr_alert("Failed to create device class file\n");
		return err;
	}
	pr_info("Device class registered correctly\n");

	if (device_create(intercom_cls, NULL, chrdev, NULL, DEV_NAME) == NULL) {
		class_destroy(intercom_cls);
		unregister_chrdev_region(chrdev, 1);
		pr_alert("Failed to create the device\n");
		return -1;
	}
	cdev_init(&intercom_dev, &dev_fops);
	if (cdev_add(&intercom_dev, chrdev, 1) == -1) {
		device_destroy(intercom_cls, chrdev);
		class_destroy(intercom_cls);
		unregister_chrdev_region(chrdev, 1);
		pr_alert("Failed to add the device\n");
		return -1;
	}
	pr_info("Device created correctly\n");

	err = create_buffer();
	if (err)
		goto error;

	err = create_procfs();
	if (err)
		goto error;

	return 0;

error:
	pr_err("Failed to load module\n");
	cleanup_procfs();
	cleanup_buffer();
	return err;
}

static void __exit intercom_exit(void)
{
	cleanup_procfs();
	cleanup_buffer();
	device_destroy(intercom_cls, chrdev);
	class_remove_file(intercom_cls, &sysfs_attr);
	class_destroy(intercom_cls);
	unregister_chrdev_region(chrdev, 1);
	pr_info("Exiting intercom\n");
}

module_init(intercom_init);
module_exit(intercom_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Nikita Kvochka <yorkened@gmail.com>");
MODULE_DESCRIPTION("Simple char dev for text messaging between users");
MODULE_VERSION("0.1");
