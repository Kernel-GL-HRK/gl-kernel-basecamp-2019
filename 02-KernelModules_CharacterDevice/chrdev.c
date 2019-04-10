#include <linux/module.h>
#include <linux/moduleparam.h>  
#include <linux/init.h>
#include <linux/kernel.h>       
#include <linux/slab.h>		
#include <linux/fs.h>           
#include <linux/types.h>        
#include <linux/cdev.h>
#include <asm/uaccess.h> 
#include <linux/device.h>
#include <linux/err.h>

#include <linux/kobject.h>
#include <linux/sysfs.h>

#include <linux/proc_fs.h>
#include <linux/seq_file.h>

#define CLASS_NAME	"chrdev"
#define DEVICE_NAME	"chrdev_example"

static int error;
static int BUFFER_SIZE=1024;
static int major = 0; 
static int minor = 0; 
 
module_param(major, int, S_IRUGO);
module_param(minor, int, S_IRUGO);
module_param(BUFFER_SIZE, int, S_IRUGO);

 
static struct class *pclass;
static struct device *pdev;

static int data_size;
static unsigned char *data_buffer=NULL;

static int is_open;

//sysfs
static int cleanup=0;
static struct kobject *example_kobject;


static ssize_t sysfs_store1(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count)
{
        sscanf(buf, "%du", &cleanup);
        pr_info("sysfs cleanup value %d\n", cleanup);
        if (cleanup){
            memset(data_buffer, BUFFER_SIZE, sizeof(*data_buffer));
        }
        return count;
}

static struct kobj_attribute sysfs_attribute =__ATTR(cleanup, 0220, NULL, sysfs_store1);

//sysfs

//procfs

static int dev_proc_show(struct seq_file *m, void *v) {
  seq_printf(m, "Buffer size = %d\nData length = %d\n", BUFFER_SIZE, data_size);
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

//procfs

static int dev_open(struct inode *inodep, struct file *filep)
{
	if (is_open) {
		pr_err("chrdev: already open\n");
		return -EBUSY;
	}

	is_open = 1;

	proc_create(DEVICE_NAME, 0, NULL, &dev_proc_fops);

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
	ret =raw_copy_to_user(buffer, data_buffer, len);
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
	if (data_size > BUFFER_SIZE) data_size = BUFFER_SIZE;
	ret =raw_copy_from_user(data_buffer, buffer, data_size);
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
	data_buffer = kzalloc(BUFFER_SIZE * sizeof(*data_buffer), GFP_KERNEL);
	if (!data_buffer) {
		pr_info("chrdev: register error\n");
        return -ENOMEM;

    }

    pr_info("chrdev: buffer_size is %d\n", BUFFER_SIZE);

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
    	if(!example_kobject) {
        pr_debug("failed to create kobject \n");
        return -ENOMEM;
    	}
    	error = sysfs_create_file(example_kobject, &sysfs_attribute.attr);
    	if (error) {
        pr_debug("failed to create  file in /sys/kernel/%s \n", DEVICE_NAME);
	return -ENOMEM;
    	}
    	pr_info("create the file in /sys/kernel/%s \n", DEVICE_NAME);

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


MODULE_AUTHOR("Mark Rubinshteyn mark.rubinshtein@meta.ua");
MODULE_DESCRIPTION("Character device driver");
MODULE_LICENSE("GPL");

