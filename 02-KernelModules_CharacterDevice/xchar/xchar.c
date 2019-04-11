#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/err.h>
#include <linux/uaccess.h>
#include <linux/proc_fs.h>
/* kfree(), kzalloc() */
#include <linux/slab.h>
/* command-line args */
#include <linux/moduleparam.h>

#define CLASS_NAME "xchar_class"
#define DEVICE_NAME "xchar"

#define PROC_DIRECTORY DEVICE_NAME
#define PROC_FILENAME "buffer_size"
#define PROC_BUFFER_SIZE 10

#define PROC_PARAM_NUM 2
#define PROC_BUFFER_SIZE_ID 0
#define PROC_USED_BUFFER_VOLUME_ID 1
/**
 *  Module parameter, eg: insmod ./xchar.ko buffer=1024
 */
static unsigned int buffer_size = 1024;
module_param(buffer_size, uint, S_IRUGO);

static struct class *pclass;
static struct device *pdev;

static int major;
static int is_open;

static size_t data_size;
static unsigned char *data_buffer = NULL;

/*
 * procfs resources
 */
static char proc_buffer[PROC_PARAM_NUM][PROC_BUFFER_SIZE];
static size_t proc_msg_length[PROC_PARAM_NUM];
static size_t proc_msg_read_pos[PROC_PARAM_NUM];

static struct proc_dir_entry *proc_dir;
static struct proc_dir_entry *proc_file;

static ssize_t example_read(struct file *file_p, char __user *buffer,
			    size_t length, loff_t *offset);
static ssize_t example_write(struct file *file_p, const char __user *buffer,
			     size_t length, loff_t *offset);

static struct file_operations proc_fops = {
	.read = example_read,
	.write = example_write,
};

/*
 * procfs operations
 */
static int create_proc_example(void)
{
	proc_dir = proc_mkdir(PROC_DIRECTORY, NULL);
	if (NULL == proc_dir)
		return -EFAULT;

	proc_file =
		proc_create(PROC_FILENAME, S_IFREG | S_IRUGO /* | S_IWUGO */,
			    proc_dir, &proc_fops);
	if (NULL == proc_file)
		return -EFAULT;

	return 0;
}

static void cleanup_proc_example(void)
{
	if (proc_file) {
		remove_proc_entry(PROC_FILENAME, proc_dir);
		proc_file = NULL;
	}
	if (proc_dir) {
		remove_proc_entry(PROC_DIRECTORY, NULL);
		proc_dir = NULL;
	}
}

static ssize_t example_read(struct file *file_p, char __user *buffer,
			    size_t length, loff_t *offset)
{
	static int complete = 1;
	size_t left;

	if (complete) {
		complete = 0;
		proc_msg_read_pos[PROC_BUFFER_SIZE_ID] = 0;
	}

	if (length > (proc_msg_length[PROC_BUFFER_SIZE_ID] -
		      proc_msg_read_pos[PROC_BUFFER_SIZE_ID]))
		length = (proc_msg_length[PROC_BUFFER_SIZE_ID] -
			  proc_msg_read_pos[PROC_BUFFER_SIZE_ID]);

	left = copy_to_user(
		buffer,
		&proc_buffer[PROC_BUFFER_SIZE_ID]
			    [proc_msg_read_pos[PROC_BUFFER_SIZE_ID]],
		length);

	proc_msg_read_pos[PROC_BUFFER_SIZE_ID] += length - left;

	if ((length - left) == 0)
		complete = 1;

	if (left)
		pr_err(DEVICE_NAME
		       ": (procfs) failed to read %zu from %zu chars\n",
		       left, length);
	else
		pr_info(DEVICE_NAME ": (procfs) read %zu chars\n", length);

	return length - left;
}

static ssize_t example_write(struct file *file_p, const char __user *buffer,
			     size_t length, loff_t *offset)
{
	pr_info(DEVICE_NAME ": write to procfs is forbidden.\n");
	return length;
}

/*
 * devfs operations
 */
static int dev_open(struct inode *inodep, struct file *filep)
{
	if (is_open) {
		pr_err(DEVICE_NAME ": already open\n");
		return -EBUSY;
	}

	is_open = 1;
	pr_info(DEVICE_NAME ": device opened\n");
	return 0;
}

static int dev_release(struct inode *inodep, struct file *filep)
{
	is_open = 0;
	pr_info(DEVICE_NAME ": device closed\n");
	return 0;
}

static ssize_t dev_read(struct file *filep, char *buffer, size_t len,
			loff_t *offset)
{
	int ret;

	pr_info(DEVICE_NAME ": read from file %s\n",
		filep->f_path.dentry->d_iname);

	if (len > data_size)
		len = data_size;

	ret = copy_to_user(buffer, data_buffer, len);
	if (ret) {
		pr_err(DEVICE_NAME ": copy_to_user failed: %d\n", ret);
		return -EFAULT;
	}
	data_size = 0; /* eof for cat */

	pr_info(DEVICE_NAME ": %zd bytes read\n", len);
	return len;
}

static ssize_t dev_write(struct file *filep, const char *buffer, size_t len,
			 loff_t *offset)
{
	int ret;

	pr_info(DEVICE_NAME ": write to file %s\n",
		filep->f_path.dentry->d_iname);

	data_size = len;
	if (data_size > buffer_size)
		data_size = buffer_size;

	ret = copy_from_user(data_buffer, buffer, data_size);
	if (ret) {
		pr_err(DEVICE_NAME ": copy_from_user failed: %d\n", ret);
		return -EFAULT;
	}

	pr_info(DEVICE_NAME ": %zd bytes written\n", data_size);
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
	int err;
	is_open = 0;
	data_size = 0;

	major = register_chrdev(0, DEVICE_NAME, &fops);
	if (major < 0) {
		pr_err(DEVICE_NAME ": register_xchar failed: %d\n", major);
		return major;
	}
	pr_info(DEVICE_NAME ": register_xchar ok, major = %d\n", major);

	pclass = class_create(THIS_MODULE, CLASS_NAME);
	if (IS_ERR(pclass)) {
		unregister_chrdev(major, DEVICE_NAME);
		pr_err(DEVICE_NAME ": class_create failed\n");
		return PTR_ERR(pclass);
	}
	pclass->devnode = xchar_devnode;
	pr_info(DEVICE_NAME ": device class created successfully\n");

	data_buffer = kzalloc(buffer_size * sizeof(*data_buffer), GFP_KERNEL);
	if (!data_buffer) {
		class_destroy(pclass);
		unregister_chrdev(major, DEVICE_NAME);
		pr_info(DEVICE_NAME ": buffer allocation failed\n");
		return -ENOMEM;
	}
	pr_info(DEVICE_NAME ": buffer allocated successfully with size = %u\n",
		buffer_size);

	sprintf(proc_buffer[PROC_BUFFER_SIZE_ID], "%u", buffer_size);
	proc_msg_length[PROC_BUFFER_SIZE_ID] =
		strlen(proc_buffer[PROC_BUFFER_SIZE_ID]);

	pdev = device_create(pclass, NULL, MKDEV(major, 0), NULL,
			     DEVICE_NAME "0");
	if (IS_ERR(pdev)) {
		class_destroy(pclass);
		unregister_chrdev(major, DEVICE_NAME);
		pr_err(DEVICE_NAME ": device_create failed\n");
		return PTR_ERR(pdev);
	}
	pr_info(DEVICE_NAME ": device node created successfully\n");

	err = create_proc_example();
	if (err) {
		cleanup_proc_example();
		pr_err(DEVICE_NAME
		       ": (procfs) can't create file or directory\n");
		return err;
	}
	pr_info(DEVICE_NAME ": (procfs) all resources created successfully\n");

	pr_info(DEVICE_NAME ": module loaded\n");
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
	cleanup_proc_example();
	pr_info(DEVICE_NAME ": module exited\n");
}

module_init(xchar_init);
module_exit(xchar_exit);

MODULE_AUTHOR("xone <xone@ukr.net>");
MODULE_DESCRIPTION("Simple character device driver");
MODULE_LICENSE("GPL");
MODULE_VERSION("0.1");
