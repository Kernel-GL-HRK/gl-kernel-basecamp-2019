#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/sched.h>
#include <asm/uaccess.h>
#include <linux/slab.h>
#include <linux/moduleparam.h>
#include <linux/device.h>
#include <linux/module.h>
#include <linux/sysfs.h>

#ifndef MODULE_NAME
#define MODULE_NAME 	"chrdev"
#endif

#define BUFFER_SIZE	    1024 
#define DEVICE_NAME 	"char_device"
#define CLASS_NAME	    "char_class"
#define PROC_DIRECTORY 	"char_device"
#define PROC_FILENAME 	"chrdev"

int buffer_increase = 0;

module_param(buffer_increase, int, 0660);

static char *data_buff;
static int buffer_size = BUFFER_SIZE;
static int used_memory = 0;
static int major;

static int clear_memory = false;
static bool busy  = false;

static struct class *pclass = NULL;
static struct device *pdev = NULL;
static struct proc_dir_entry *proc_dir;
static struct proc_dir_entry *proc_file;
static struct kobject *chd_kobject;

static ssize_t proc_read(struct file *file_p, char __user *buffer, size_t length, loff_t *offset);

static struct file_operations proc_fops = {
	.read  = proc_read,
};

static int buffer_create(void){
	int res = 0;
 
    data_buff = kzalloc(buffer_size+buffer_increase, GFP_KERNEL);
    if (NULL == data_buff) res = -ENOMEM;
    return res;
}

static void buffer_clean(void){
    if (data_buff) {
        kfree(data_buff);
        data_buff = NULL;
    }
}

static ssize_t sysfs_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buffer, size_t len){
	sscanf(buffer, "%d\n", &clear_memory);
	printk(KERN_INFO "chrdev: clear memory = %d\n", clear_memory);
	
	if(clear_memory){
		kfree(data_buff);
		data_buff = kzalloc(buffer_size+buffer_increase, GFP_KERNEL);
		used_memory=0;
		printk(KERN_INFO "chrdev: memory cleared\n");
	}
	return len;
}

static struct kobj_attribute sysfs_attribute =__ATTR(clear_memory, 0220, NULL, sysfs_store);

static int create_proc(void){
	int res = 0;
    proc_dir = proc_mkdir(PROC_DIRECTORY, NULL);
    if (NULL == proc_dir) res = -EFAULT;

    proc_file = proc_create(PROC_FILENAME, S_IFREG | S_IRUGO | S_IWUGO, proc_dir, &proc_fops);
    if (NULL == proc_file) res = -EFAULT;
    return res;
}


static void cleanup_proc(void){
    if (proc_file) {
        remove_proc_entry(PROC_FILENAME, proc_dir);
        proc_file = NULL;
    }
    if (proc_dir)     {
        remove_proc_entry(PROC_DIRECTORY, NULL);
        proc_dir = NULL;
    }
}

static ssize_t proc_read(struct file *filp, char *buffer, size_t len, loff_t *offset ){
	ssize_t res;
	char msg[124];
	
	sprintf(msg, "%d %d\n", buffer_size+buffer_increase, buffer_size+buffer_increase-used_memory);

	if(*offset >= strlen(msg)) {
		*offset=0;
		return 0;
	}
	
	if(len > strlen(msg) - *offset) len = strlen(msg) - *offset;
	res = raw_copy_to_user((void*)buffer, msg - *offset, len);
	*offset += len;
	return len;
}

int dev_open(struct inode *inode, struct file *filp){	
	if(busy){
		printk(KERN_ERR "chrdev: failed to open");
		return -EBUSY;
	}
	busy = true;
	return 0;
}

int dev_release(struct inode *inode, struct file *filp){
	busy = false;
	return 0;
}

ssize_t dev_read(struct file *filep, char *buffer, size_t len, loff_t *offset){
	ssize_t retval = 0;

	int remain = buffer_size - (int) (*offset);
	if (remain == 0) {
		return retval;
	}

	if (len > remain) {
		len = remain;
	}

	if (raw_copy_to_user(buffer, data_buff + *offset, len)) {
		retval = -EFAULT;
		return retval;
	}
	*offset += len;
	retval = len;

	return retval;
}

ssize_t dev_write(struct file *filep, const char *buffer, size_t len, loff_t *offset){
	int error = 0;
	int remain = buffer_size - (int) (*offset);
	
	used_memory += len;
	
	if(0 == remain) return 0;
	if(used_memory > remain) return -EIO;
	
	error = raw_copy_from_user(data_buff + *offset, buffer, len);
	if(error){
		printk(KERN_INFO "chrdev: failed to receive %zu characters from the user\n", len);
		return -EFAULT;
	}
	else{
		printk(KERN_INFO "chrdev: received %zu characters from the user\n", len);
		offset += len;
		return len;
	}
}

static struct file_operations fops = {
	.open = dev_open,
	.read = dev_read,
	.write = dev_write,
	.release = dev_release,
};

static int __init chrdev_init(void){
	int err;
	
	printk(KERN_INFO "chrdev loading");
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

    pdev = device_create(pclass, NULL, MKDEV(major, 0), NULL, DEVICE_NAME);
	if (IS_ERR(pdev)) {
		class_destroy(pclass);
		unregister_chrdev(major, DEVICE_NAME);
		pr_err("chrdev: device_create failed\n");
		return PTR_ERR(pdev);
	}
	printk(KERN_INFO "chrdev: Device initialized succesfully\n");

	err = buffer_create();
	if (IS_ERR(err)) {
		class_destroy(pclass);
		unregister_chrdev(major, DEVICE_NAME);
		pr_err("chrdev: buffer_create failed\n");
		return err;
	}

	err = create_proc();
	if (IS_ERR(err)) {
		class_destroy(pclass);
		unregister_chrdev(major, DEVICE_NAME);
		pr_err("chrdev: proc_create failed\n");
		return err;
	}
	
	chd_kobject = kobject_create_and_add(MODULE_NAME, kernel_kobj);
	err = sysfs_create_file(chd_kobject, &sysfs_attribute.attr);
	return err;
}

static void __exit chrdev_exit(void){
	kobject_put(chd_kobject);
	
	cleanup_proc();
	buffer_clean();
	device_destroy(pclass, MKDEV(major,0));
	class_destroy(pclass);
	class_unregister(pclass);
	
	unregister_chrdev(major, DEVICE_NAME);
	printk(KERN_INFO "chrdev: Module unloaded\n");
}

module_init(chrdev_init);
module_exit(chrdev_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Oleksandr Shmatko");
MODULE_DESCRIPTION("Characters buffer driver module");
MODULE_VERSION("0.1");
