/*
 * Шаблон для разработки драйвера символьного устройства
 * driver.c -- very simple chrdev driver
 */

#include <linux/module.h>
#include <linux/moduleparam.h>  /* command-line args */
#include <linux/init.h>
#include <linux/kernel.h>       /* printk() */
#include <linux/slab.h>		    /* kfree(), kmalloc() */
#include <linux/fs.h>           /* file_operations */
#include <linux/types.h>        /* dev_t */
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <asm/uaccess.h>        /* copy_*_user */
#include <linux/proc_fs.h>
#include <linux/kobject.h>
#include <linux/sysfs.h>

#undef VERIFY_OCTAL_PERMISSIONS
#define VERIFY_OCTAL_PERMISSIONS(perms) (0666)

#ifndef MODULE_NAME
#define MODULE_NAME "driver"
#endif

#define CLASS_NAME	"chrdev"
#define DEVICE_NAME	"chrdev"

static struct class *pclass;
static struct device *pdev;

static char *data_buffer;
static int data_size;

static int BUFFER_SIZE = 1024;
module_param(BUFFER_SIZE, int, S_IRUGO);

static int is_open;
static int major;

int error;

static struct kobject *example_kobject;

static struct proc_dir_entry *proc_file;

/*
 * sys interface
 */

static ssize_t sys_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count)
{
	if(data_size > 0){
		printk("will be freed: %d\n", data_size);
		memset(data_buffer, 0, data_size);
		data_size = 0;		
	}
	else {		
		pr_info("buffer is empty \n");
	}
    return count;
}

static struct kobj_attribute sys_attribute =__ATTR(foo, 0666, NULL, sys_store);


/*
 * proc interface
 */

static ssize_t proc_read (struct file *filep, char *buffer, size_t len, loff_t *offset) {	
	printk("chrdev: size of buffer: %d\n", BUFFER_SIZE);
	printk("chrdev: free: %d\n", (BUFFER_SIZE - data_size));
	return 0;
}

static struct file_operations proc_file_fops = {
	.owner = THIS_MODULE,
	.read = proc_read
};


/*
 * dev interface
 */

static int dev_open(struct inode *inodep, struct file *filep)
{
	if (1 == is_open) {
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

	pr_info("chrdev: %ld bytes read\n", len);
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

struct file_operations fops = {
	.owner = THIS_MODULE,
	.open = dev_open,
	.release = dev_release,
	.read = dev_read,
	.write = dev_write
};


static int __init device_init( void ) 
{
    is_open = 0;
	data_size = 0;
	error = 0;	
	
	if(BUFFER_SIZE < 1024)
		BUFFER_SIZE = 1024;
		
	data_buffer = kzalloc(BUFFER_SIZE * sizeof(*data_buffer), GFP_KERNEL);

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

	proc_file = proc_create(DEVICE_NAME, 0666, NULL, &proc_file_fops);
	
	example_kobject = kobject_create_and_add(DEVICE_NAME, kernel_kobj);
	if(!example_kobject)
    	return -ENOMEM;
	
	error = sysfs_create_file(example_kobject, &sys_attribute.attr);
	
    if (error)
    	pr_debug("failed to create the foo file in /sys/kernel/ \n");
    	
	pr_info("chrdev: module loaded\n");
	return 0;
}

static void __exit device_exit( void ) {
	kfree(data_buffer);
	device_destroy(pclass, MKDEV(major, 0));
	class_destroy(pclass);
	unregister_chrdev(major, DEVICE_NAME);	
	
	remove_proc_entry(DEVICE_NAME, NULL);
	kobject_put(example_kobject);
	
	pr_info("chrdev: module exited\n");
}


module_init( device_init );
module_exit( device_exit );

MODULE_LICENSE("GPL");
MODULE_AUTHOR ("Ihor Ryzhov <ihor.ryzhov@nure.ua>");

