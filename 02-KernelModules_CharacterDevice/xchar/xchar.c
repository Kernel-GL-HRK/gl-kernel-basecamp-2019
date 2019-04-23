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

#define CLASS_NAME "xchar"
#define DEVICE_NAME "xchar_device"

#define PROC_DIRECTORY CLASS_NAME
#define PROC_BUFFER_SIZE_FILENAME "buffer_size"
#define PROC_USED_BUFFER_VOLUME_FILENAME "used_buffer_volume"
#define PROC_BUFFER_SIZE 10
#define PROC_PARAM_NUM 2
#define PROC_BUFFER_SIZE_ID 0
#define PROC_USED_BUFFER_VOLUME_ID 1
#define SYS_BUFFER_SIZE 10

/**
 *  Module parameter, eg: insmod ./xchar.ko buffer_size=1024
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
 * sysfs resources
 */
static char sys_buffer[SYS_BUFFER_SIZE];

/*
 * procfs resources
 */
static char proc_buffer[PROC_PARAM_NUM][PROC_BUFFER_SIZE];
static size_t proc_msg_length[PROC_PARAM_NUM];
static size_t proc_msg_read_pos[PROC_PARAM_NUM];

static struct proc_dir_entry *proc_dir;
static struct proc_dir_entry *proc_file[PROC_PARAM_NUM];

static ssize_t proc_buffer_size_read(struct file *file_p, char __user *buffer,
				     size_t length, loff_t *offset);

static ssize_t proc_buffer_size_write(struct file *file_p,
				      const char __user *buffer, size_t length,
				      loff_t *offset);

static ssize_t proc_used_buffer_volume_read(struct file *file_p,
					    char __user *buffer, size_t length,
					    loff_t *offset);

static ssize_t proc_used_buffer_volume_write(struct file *file_p,
					     const char __user *buffer,
					     size_t length, loff_t *offset);

static struct file_operations proc_buffer_size_fops = {
	.read = proc_buffer_size_read,
	.write = proc_buffer_size_write,
};

static struct file_operations proc_used_buffer_volume_fops = {
	.read = proc_used_buffer_volume_read,
	.write = proc_used_buffer_volume_write,
};

/*
 * sysfs operations
 */
static ssize_t clean_up_buffer_show(struct class *class,
				    struct class_attribute *attr, char *buf)
{
	sprintf(sys_buffer, "%zu", data_size);
	strcpy(buf, sys_buffer);
	pr_info(CLASS_NAME ": (sysfs) read %zu chars\n", strlen(buf));
	return strlen(buf);
}

static ssize_t clean_up_buffer_store(struct class *class,
				     struct class_attribute *attr,
				     const char *buf, size_t count)
{
	int res;
	if (SYS_BUFFER_SIZE <= count)
		count = SYS_BUFFER_SIZE - 1;
	strncpy(sys_buffer, buf, count);
	sys_buffer[count] = '\0';
	pr_info(CLASS_NAME ": (sysfs) write %zu chars\n", count);
	if (kstrtoint(sys_buffer, 10, &res) == 0) {
		pr_info(CLASS_NAME
			": (sysfs) write a nonzero integer, let's clear the buffer\n");
		data_size = 0;
		sprintf(proc_buffer[PROC_USED_BUFFER_VOLUME_ID], "%zu",
			data_size);
		proc_msg_length[PROC_USED_BUFFER_VOLUME_ID] =
			strlen(proc_buffer[PROC_USED_BUFFER_VOLUME_ID]);
	} else {
		pr_info(CLASS_NAME ": (sysfs) zero value is ignored\n");
	}
	return count;
}

CLASS_ATTR_RW(clean_up_buffer);

/*
 * procfs operations
 */
static int init_proc(void)
{
	proc_dir = proc_mkdir(PROC_DIRECTORY, NULL);
	if (NULL == proc_dir)
		return -EFAULT;

	proc_file[PROC_BUFFER_SIZE_ID] =
		proc_create(PROC_BUFFER_SIZE_FILENAME,
			    S_IFREG | S_IRUGO /* | S_IWUGO */, proc_dir,
			    &proc_buffer_size_fops);
	if (NULL == proc_file[PROC_BUFFER_SIZE_ID])
		return -EFAULT;

	proc_file[PROC_USED_BUFFER_VOLUME_ID] =
		proc_create(PROC_USED_BUFFER_VOLUME_FILENAME,
			    S_IFREG | S_IRUGO /* | S_IWUGO */, proc_dir,
			    &proc_used_buffer_volume_fops);
	if (NULL == proc_file[PROC_USED_BUFFER_VOLUME_ID])
		return -EFAULT;

	return 0;
}

static void cleanup_proc(void)
{
	if (proc_file[PROC_BUFFER_SIZE_ID]) {
		remove_proc_entry(PROC_BUFFER_SIZE_FILENAME, proc_dir);
		proc_file[PROC_BUFFER_SIZE_ID] = NULL;
	}
	if (proc_file[PROC_USED_BUFFER_VOLUME_ID]) {
		remove_proc_entry(PROC_USED_BUFFER_VOLUME_FILENAME, proc_dir);
		proc_file[PROC_USED_BUFFER_VOLUME_ID] = NULL;
	}
	if (proc_dir) {
		remove_proc_entry(PROC_DIRECTORY, NULL);
		proc_dir = NULL;
	}
}

static ssize_t proc_buffer_size_read(struct file *file_p, char __user *buffer,
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
		pr_err(CLASS_NAME
		       ": (procfs) failed to read %zu from %zu chars\n",
		       left, length);
	else
		pr_info(CLASS_NAME ": (procfs) read %zu chars\n", length);

	return length - left;
}

static ssize_t proc_used_buffer_volume_read(struct file *file_p,
					    char __user *buffer, size_t length,
					    loff_t *offset)
{
	static int complete = 1;
	size_t left;

	if (complete) {
		complete = 0;
		proc_msg_read_pos[PROC_USED_BUFFER_VOLUME_ID] = 0;
	}

	if (length > (proc_msg_length[PROC_USED_BUFFER_VOLUME_ID] -
		      proc_msg_read_pos[PROC_USED_BUFFER_VOLUME_ID]))
		length = (proc_msg_length[PROC_USED_BUFFER_VOLUME_ID] -
			  proc_msg_read_pos[PROC_USED_BUFFER_VOLUME_ID]);

	left = copy_to_user(
		buffer,
		&proc_buffer[PROC_USED_BUFFER_VOLUME_ID]
			    [proc_msg_read_pos[PROC_USED_BUFFER_VOLUME_ID]],
		length);

	proc_msg_read_pos[PROC_USED_BUFFER_VOLUME_ID] += length - left;

	if ((length - left) == 0)
		complete = 1;

	if (left)
		pr_err(CLASS_NAME
		       ": (procfs) failed to read %zu from %zu chars\n",
		       left, length);
	else
		pr_info(CLASS_NAME ": (procfs) read %zu chars\n", length);

	return length - left;
}

static ssize_t proc_buffer_size_write(struct file *file_p,
				      const char __user *buffer, size_t length,
				      loff_t *offset)
{
	pr_info(CLASS_NAME ": write to /procf/buffer_size is forbidden.\n");
	return length;
}

static ssize_t proc_used_buffer_volume_write(struct file *file_p,
					     const char __user *buffer,
					     size_t length, loff_t *offset)
{
	pr_info(CLASS_NAME
		": write to /proc/used_buffer_volume is forbidden.\n");
	return length;
}

/*
 * devfs operations
 */
static int dev_open(struct inode *inodep, struct file *filep)
{
	if (is_open) {
		pr_err(CLASS_NAME ": already open\n");
		return -EBUSY;
	}

	is_open = 1;
	pr_info(CLASS_NAME ": device opened\n");
	return 0;
}

static int dev_release(struct inode *inodep, struct file *filep)
{
	is_open = 0;
	pr_info(CLASS_NAME ": device closed\n");
	return 0;
}

static ssize_t dev_read(struct file *filep, char *buffer, size_t len,
			loff_t *offset)
{
	int ret;

	pr_info(CLASS_NAME ": read from file %s\n",
		filep->f_path.dentry->d_iname);

	if (len > data_size)
		len = data_size;

	ret = copy_to_user(buffer, data_buffer, len);
	if (ret) {
		pr_err(CLASS_NAME ": copy_to_user failed: %d\n", ret);
		return -EFAULT;
	}
	data_size = 0; /* eof for cat */

	sprintf(proc_buffer[PROC_USED_BUFFER_VOLUME_ID], "%zu", data_size);
	proc_msg_length[PROC_USED_BUFFER_VOLUME_ID] =
		strlen(proc_buffer[PROC_USED_BUFFER_VOLUME_ID]);

	pr_info(CLASS_NAME ": %zd bytes read\n", len);
	return len;
}

static ssize_t dev_write(struct file *filep, const char *buffer, size_t len,
			 loff_t *offset)
{
	int ret;

	pr_info(CLASS_NAME ": write to file %s\n",
		filep->f_path.dentry->d_iname);

	data_size = len;
	if (data_size > buffer_size)
		data_size = buffer_size;

	sprintf(proc_buffer[PROC_USED_BUFFER_VOLUME_ID], "%zu", data_size);
	proc_msg_length[PROC_USED_BUFFER_VOLUME_ID] =
		strlen(proc_buffer[PROC_USED_BUFFER_VOLUME_ID]);

	ret = copy_from_user(data_buffer, buffer, data_size);
	if (ret) {
		pr_err(CLASS_NAME ": copy_from_user failed: %d\n", ret);
		return -EFAULT;
	}

	pr_info(CLASS_NAME ": %zu bytes written\n", data_size);
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
		pr_err(CLASS_NAME ": register_chrdev failed: %d\n", major);
		return major;
	}
	pr_info(CLASS_NAME ": register_chrdev ok, major = %d\n", major);

	pclass = class_create(THIS_MODULE, CLASS_NAME);
	if (IS_ERR(pclass)) {
		unregister_chrdev(major, DEVICE_NAME);
		pr_err(CLASS_NAME ": class_create failed\n");
		return PTR_ERR(pclass);
	}
	pclass->devnode = xchar_devnode;
	pr_info(CLASS_NAME ": device class created successfully\n");

	data_buffer = kzalloc(buffer_size * sizeof(*data_buffer), GFP_KERNEL);
	if (!data_buffer) {
		class_destroy(pclass);
		unregister_chrdev(major, DEVICE_NAME);
		pr_info(CLASS_NAME ": buffer allocation failed\n");
		return -ENOMEM;
	}
	pr_info(CLASS_NAME ": buffer allocated successfully with size = %u\n",
		buffer_size);

	sprintf(proc_buffer[PROC_BUFFER_SIZE_ID], "%u", buffer_size);
	proc_msg_length[PROC_BUFFER_SIZE_ID] =
		strlen(proc_buffer[PROC_BUFFER_SIZE_ID]);

	sprintf(proc_buffer[PROC_USED_BUFFER_VOLUME_ID], "%zu", data_size);
	proc_msg_length[PROC_USED_BUFFER_VOLUME_ID] =
		strlen(proc_buffer[PROC_USED_BUFFER_VOLUME_ID]);

	pdev = device_create(pclass, NULL, MKDEV(major, 0), NULL,
			     CLASS_NAME "0");
	if (IS_ERR(pdev)) {
		class_destroy(pclass);
		unregister_chrdev(major, DEVICE_NAME);
		pr_err(CLASS_NAME ": device_create failed\n");
		return PTR_ERR(pdev);
	}
	pr_info(CLASS_NAME ": device node created successfully\n");

	err = init_proc();
	if (err) {
		cleanup_proc();
		class_destroy(pclass);
		unregister_chrdev(major, DEVICE_NAME);
		pr_err(CLASS_NAME ": can't create procfs file or directory\n");
		return err;
	}
	pr_info(CLASS_NAME ": all procfs resources created successfully\n");

	err = class_create_file(pclass, &class_attr_clean_up_buffer);
	if (err) {
		cleanup_proc();
		class_destroy(pclass);
		unregister_chrdev(major, DEVICE_NAME);
		pr_err(CLASS_NAME ": can't create sysfs file\n");
		return err;
	}
	pr_info(CLASS_NAME ": all sysfs resources created successfully\n");

	pr_info(CLASS_NAME ": module loaded\n");
	return 0;
}

static void xchar_exit(void)
{
	class_remove_file(pclass, &class_attr_clean_up_buffer);
	cleanup_proc();
	device_destroy(pclass, MKDEV(major, 0));
	class_destroy(pclass);
	unregister_chrdev(major, DEVICE_NAME);
	if (data_buffer) {
		kfree(data_buffer);
		data_buffer = NULL;
	}
	pr_info(CLASS_NAME ": module exited\n");
}

module_init(xchar_init);
module_exit(xchar_exit);

MODULE_AUTHOR("xone <xone@ukr.net>");
MODULE_DESCRIPTION("Simple character device driver");
MODULE_LICENSE("GPL");
MODULE_VERSION("0.1");
