#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/err.h>
#include <linux/uaccess.h>

/* kfree(), kzalloc() */
#include <linux/slab.h>

#define CLASS_NAME "xchar"
#define DEVICE_NAME "xchar_device"
#define BUFFER_SIZE 1024

static struct class *pclass;
static struct device *pdev;

static int major;
static int is_open;

static size_t data_size;
static unsigned char *data_buffer = NULL;

static int dev_open(struct inode *inodep, struct file *filep)
{
	if (is_open) {
		pr_err("xchar: already open\n");
		return -EBUSY;
	}

	is_open = 1;
	pr_info("xchar: device opened\n");
	return 0;
}

static int dev_release(struct inode *inodep, struct file *filep)
{
	is_open = 0;
	pr_info("xchar: device closed\n");
	return 0;
}

static ssize_t dev_read(struct file *filep, char *buffer, size_t len,
			loff_t *offset)
{
	unsigned long ret;

	pr_info("xchar: read from file %s\n", filep->f_path.dentry->d_iname);

	if (len > data_size)
		len = data_size;

	ret = copy_to_user(buffer, data_buffer, len);
	if (ret) {
		pr_err("xchar: copy_to_user failed: %ld\n", ret);
		return -EFAULT;
	}
	data_size = 0; /* eof for cat */

	pr_info("xchar: %zd bytes read\n", len);
	return len;
}

static ssize_t dev_write(struct file *filep, const char *buffer, size_t len,
			 loff_t *offset)
{
	unsigned long ret;

	pr_info("xchar: write to file %s\n", filep->f_path.dentry->d_iname);

	data_size = len;
	if (data_size > BUFFER_SIZE)
		data_size = BUFFER_SIZE;

	ret = copy_from_user(data_buffer, buffer, data_size);
	if (ret) {
		pr_err("xchar: copy_from_user failed: %ld\n", ret);
		return -EFAULT;
	}

	pr_info("xchar: %zd bytes written\n", data_size);
	return data_size;
}

static struct file_operations fops = {
	.open = dev_open,
	.release = dev_release,
	.read = dev_read,
	.write = dev_write,
};

static char *xchar_devnode(struct device *dev, umode_t *mode)
{
	if (mode)
		*mode = 0666;
	return kasprintf(GFP_KERNEL, "%s", dev_name(dev));
}

static int xchar_init(void)
{
	is_open = 0;
	data_size = 0;

	major = register_chrdev(0, DEVICE_NAME, &fops);
	if (major < 0) {
		pr_err("register_xchar failed: %d\n", major);
		return major;
	}
	pr_info("xchar: register_xchar ok, major = %d\n", major);

	pclass = class_create(THIS_MODULE, CLASS_NAME);
	if (IS_ERR(pclass)) {
		unregister_chrdev(major, DEVICE_NAME);
		pr_err("xchar: class_create failed\n");
		return PTR_ERR(pclass);
	}
	pclass->devnode = xchar_devnode;
	pr_info("xchar: device class created successfully\n");

	data_buffer = kzalloc(BUFFER_SIZE * sizeof(*data_buffer), GFP_KERNEL);
	if (!data_buffer) {
		class_destroy(pclass);
		unregister_chrdev(major, DEVICE_NAME);
		pr_info("xchar: buffer allocation failed\n");
		return -ENOMEM;
	}
	pr_info("xchar: buffer allocated successfully\n");

	pdev = device_create(pclass, NULL, MKDEV(major, 0), NULL,
			     CLASS_NAME "0");
	if (IS_ERR(pdev)) {
		class_destroy(pclass);
		unregister_chrdev(major, DEVICE_NAME);
		pr_err("xchar: device_create failed\n");
		return PTR_ERR(pdev);
	}
	pr_info("xchar: device node created successfully\n");

	pr_info("xchar: module loaded\n");
	return 0;
}

static void xchar_exit(void)
{
	device_destroy(pclass, MKDEV(major, 0));
	class_destroy(pclass);
	unregister_chrdev(major, DEVICE_NAME);
	if (data_buffer) {
		kfree(data_buffer);
		data_buffer = NULL;
	}
	pr_info("xchar: module exited\n");
}

module_init(xchar_init);
module_exit(xchar_exit);

MODULE_AUTHOR("xone <xone@ukr.net>");
MODULE_DESCRIPTION("Character device simple driver");
MODULE_LICENSE("GPL");
MODULE_VERSION("0.1");
