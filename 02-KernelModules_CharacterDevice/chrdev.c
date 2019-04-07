#include <linux/init.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/device.h>
#include <linux/err.h>
#include <linux/uaccess.h>
#include <linux/fs.h>
#include <linux/kernel.h>
#include <linux/slab.h>	/* kfree(), kmalloc() */

#include <linux/kobject.h>
#include <linux/sysfs.h>

#include <linux/proc_fs.h>
#include <linux/seq_file.h>

#define CLASS_NAME	"vdk-chrdev"
#define DEVICE_NAME	"vdk-chrdev-example"
#define BUFFER_SIZE	1024

static struct class *pclass;
static struct device *pdev;

static int major;
static int is_open;
static int error;
static int data_size;
static int buffer_size = BUFFER_SIZE;

module_param(buffer_size, int, 0220);


static unsigned char *data_buffer;

//-----sysfs-------
static int cleanup;
static struct kobject *example_kobject;


static ssize_t sysfs_store1(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count)
{
	sscanf(buf, "%du", &cleanup);
	pr_info("chrdev: sysfs cleanup value %d\n", cleanup);
	if (cleanup)
		memset(data_buffer, buffer_size, sizeof(*data_buffer));

	return count;
}

static struct kobj_attribute sysfs_attribute = __ATTR(cleanup, 0220, NULL, sysfs_store1);

//-----sysfs-------

//-----procfs-------


static int dev_proc_show(struct seq_file *m, void *v)
{
	seq_printf(m, "Buffer size = %d\nData length = %d\n", buffer_size, data_size);
	return 0;
}

static int dev_proc_open(struct inode *inode, struct  file *file)
{
	return single_open(file, dev_proc_show, NULL);
}

static const struct file_operations dev_proc_fops = {
	.owner = THIS_MODULE,
	.open = dev_proc_open,
	.read = seq_read,
	.llseek = seq_lseek,
	.release = single_release,

static int dev_proc_show(struct seq_file *m, void *v) {
  seq_printf(m, "Buffer size = %d\nData length = %d\n", buffer_size, data_size);
  return 0;
}

static int dev_proc_open(struct inode *inode, struct  file *file) {
  return single_open(file, dev_proc_show, NULL);
}

static const struct file_operations dev_proc_fops = {
  .owner = THIS_MODULE,
  .open = dev_proc_open,
  .read = seq_read,
  .llseek = seq_lseek,
  .release = single_release,
};

//-----procfs-------

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

	if (len > data_size)
		len = data_size;

	ret = copy_to_user(buffer, data_buffer, len);
	if (ret) {
		pr_err("chrdev: copy_to_user failed: %d\n", ret);
		return -EFAULT;
	}
	data_size = 0; /* eof for cat */

	pr_info("chrdev: %d bytes read\n", len);
	return len;
}

static ssize_t dev_write(struct file *filep, const char *buffer, size_t len, loff_t *offset)
{
	int ret;

	pr_info("chrdev: write to file %s\n", filep->f_path.dentry->d_iname);

	data_size = len;
	if (data_size > BUFFER_SIZE)
		data_size = BUFFER_SIZE;

	ret = copy_from_user(data_buffer, buffer, data_size);
	if (ret) {
		pr_err("chrdev: copy_from_user failed: %d\n", ret);
		return -EFAULT;
	}

	pr_info("chrdev: %d bytes written\n", data_size);
	return data_size;
}



static struct file_operations fops = {
	.open = dev_open,
	.release = dev_release,
	.read = dev_read,
	.write = dev_write,
};

static int chrdev_init(void)
{

	is_open = 0;
	data_size = 0;

	data_buffer = kzalloc(buffer_size * sizeof(*data_buffer), GFP_KERNEL);
	if (!data_buffer) {
		pr_info("chrdev: register error\n");
		return -ENOMEM;
	}

	pr_info("chrdev: buffer_size is %d\n", buffer_size);

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

	example_kobject = kobject_create_and_add(DEVICE_NAME, kernel_kobj);

	if (!example_kobject) {
		pr_debug("failed to create kobject \n");
		return -ENOMEM;
	}
	error = sysfs_create_file(example_kobject, &sysfs_attribute.attr);
	if (error) {
		pr_debug("failed to create the foo file in /sys/kernel/%s\n", DEVICE_NAME);
		return -ENOMEM;
	}
	pr_info("create the file in /sys/kernel/%s\n", DEVICE_NAME);

	proc_create(DEVICE_NAME, 0, NULL, &dev_proc_fops);

	pr_info("chrdev: module loaded\n");

    if(!example_kobject) {
        pr_debug("failed to create kobject \n");
        return -ENOMEM;
    }
    error = sysfs_create_file(example_kobject, &sysfs_attribute.attr);
    if (error) {
        pr_debug("failed to create the foo file in /sys/kernel/%s\n", DEVICE_NAME);
        return -ENOMEM;
    }
    pr_info("create the file in /sys/kernel/%s \n", DEVICE_NAME);


	proc_create(DEVICE_NAME, 0, NULL, &dev_proc_fops);


	pr_info("chrdev: module loaded\n");

	return 0;
}

static void chrdev_exit(void)
{
	kobject_put(example_kobject);
	remove_proc_entry(DEVICE_NAME, NULL);

	device_destroy(pclass, MKDEV(major, 0));
	class_destroy(pclass);
	unregister_chrdev(major, DEVICE_NAME);

	kfree(data_buffer);


	pr_info("chrdev: module exited\n");
}

module_init(chrdev_init);
module_exit(chrdev_exit);


MODULE_AUTHOR("Oleksii.Vodka <oleksii.vodka@gmail.com>");
MODULE_DESCRIPTION("Character device driver");
MODULE_LICENSE("GPL");
MODULE_VERSION("0.1");

