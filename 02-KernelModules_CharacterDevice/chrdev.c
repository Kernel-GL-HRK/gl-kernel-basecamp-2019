#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/err.h>
#include <linux/uaccess.h>
#include <linux/fs.h>

#define CLASS_NAME	"chrdev"
#define DEVICE_NAME	"chrdev_example"
#define BUFFER_SIZE	1024

static struct class *pclass;
static struct device *pdev;

static int major;
static int is_open;

static int data_size;
static unsigned char data_buffer[BUFFER_SIZE];

static int dev_open(struct inode *inodep, struct file *filep)
{
	if (is_open) {
		pr_err("chrdev: already open\n");
		return -EBUSY;
	}

	is_open = 1;
	pr_info("chrdev: device opened\n");
	return 0;
}

static int dev_release(struct inode *inodep, struct file *filep)
{
	is_open = 0;
	pr_info("chrdev: device closed\n");
	return 0;
}


static ssize_t dev_read(struct file *filep, char *buffer, size_t len, loff_t *offset)
{
	int ret;

	pr_info("chrdev: read from file %s\n", filep->f_path.dentry->d_iname);

	if (len > data_size) len = data_size;

	ret = copy_to_user(buffer, data_buffer, len);
	if (ret) {
		pr_err("chrdev: copy_to_user failed: %d\n", ret);
		return -EFAULT;
	}
	data_size = 0; /* eof for cat */

	pr_info("chrdev: %ld bytes read\n", len);
	return len;
}

static ssize_t dev_write(struct file *filep, const char *buffer, size_t len, loff_t *offset)
{
	int ret;

	pr_info("chrdev: write to file %s\n", filep->f_path.dentry->d_iname);

	data_size = len;
	if (data_size > BUFFER_SIZE) data_size = BUFFER_SIZE;

	ret = copy_from_user(data_buffer, buffer, data_size);
	if (ret) {
		pr_err("chrdev: copy_from_user failed: %d\n", ret);
		return -EFAULT;
	}

	pr_info("chrdev: %d bytes written\n", data_size);
	return data_size;
}



static struct file_operations fops =
{
	.open = dev_open,
	.release = dev_release,
	.read = dev_read,
	.write = dev_write,
};

static int chrdev_init(void)
{
	is_open = 0;
	data_size = 0;

	major = register_chrdev(0, DEVICE_NAME, &fops);
	if (major < 0) {
		pr_err("register_chrdev failed: %d\n", major);
		return major;
	}
	pr_info("chrdev: register_chrdev ok, major = %d\n", major);

	pclass = class_create(THIS_MODULE, CLASS_NAME);
	if (IS_ERR(pclass)) {
		unregister_chrdev(major, DEVICE_NAME);
		pr_err("chrdev: class_create failed\n");
		return PTR_ERR(pclass);
	}
	pr_info("chrdev: device class created successfully\n");

	pdev = device_create(pclass, NULL, MKDEV(major, 0), NULL, CLASS_NAME"0");
	if (IS_ERR(pdev)) {
		class_destroy(pclass);
		unregister_chrdev(major, DEVICE_NAME);
		pr_err("chrdev: device_create failed\n");
		return PTR_ERR(pdev);
	}
	pr_info("chrdev: device node created successfully\n");

	pr_info("chrdev: module loaded\n");
	return 0;
}

static void chrdev_exit(void)
{
	device_destroy(pclass, MKDEV(major, 0));
	class_destroy(pclass);
	unregister_chrdev(major, DEVICE_NAME);

	pr_info("chrdev: module exited\n");
}

module_init(chrdev_init);
module_exit(chrdev_exit);

MODULE_AUTHOR("Andriy.Khulap <andriy.khulap@globallogic.com>");
MODULE_DESCRIPTION("Character device example driver");
MODULE_LICENSE("GPL");
MODULE_VERSION("0.1");

