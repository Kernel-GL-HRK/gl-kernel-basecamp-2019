#include <linux/uaccess.h>	//copy_to_user, copy_from_user
#include <linux/slab.h>		//kzalloc, kfree
#include <linux/proc_fs.h>
#include <linux/kobject.h>
#include <linux/sysfs.h>

#define CLASS_NAME 		"chdev_class"
#define DEVICE_NAME	 	"chdev"
#define NUMBER_OF_DEVICES 	1
#define DEFALT_BUFFER_SIZE 	1024

#define PROC_DIR_NAME 		"chdev_proc_dir"
#define PROC_FILE_NAME 		"chdev_proc_file"

#define SYS_DIR_NAME		"chdev_sys_dir"
#define SYS_FILE_NAME		chdev_sys_file


static unsigned char *buffer;

static struct cdev dev_struct;


static struct proc_dir_entry *proc_dir;
static struct proc_dir_entry *proc_file;


ssize_t sys_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buff, size_t count)
{
	if(buff[0] != 0) {
		used_size = 0;
		buffer[0] = '\0';
		printk("Device buffer cleaned!\n");
	}
	return count;
}

static struct kobject *sys_dir;
static struct kobj_attribute sys_file_attr = __ATTR(SYS_FILE_NAME, S_IWUSR, NULL, sys_store);

static int chdev_open(struct inode *inode_p, struct file *file_p)
{
	if(is_open) {
	return -EBUSY;
}


static int chdev_release(struct inode *inode_p, struct file *file_p)
{
	is_open = 0;
	.write = chdev_write
};


static ssize_t proc_read(struct file *file_p, char __user *usr_buff, size_t len, loff_t *offset)
{
	if(*offset != 0)
		goto end_read;

	char res[256];

	int res_len = sprintf(res, "Buffer info:\n\tallocated size - %d bytes\n\tused size - %d bytes\n", alloc_size, used_size);

	*offset += res_len;

	if(copy_to_user(usr_buff, res, res_len) != 0)
		return -EFAULT;

	return res_len;

end_read:
	return 0;
}

static struct file_operations proc_fops = {
	.owner = THIS_MODULE,
	.read = proc_read
};


static int __init init(void)
{
        int res;
	is_open = 0;
	used_size = 0;

	if(alloc_size < DEFALT_BUFFER_SIZE) {
		printk("Buffer size must be at least %d bytes!\n", DEFALT_BUFFER_SIZE);
		goto buffer_too_small;
	}

	cdev_init(&dev_struct, &fops);
	res = cdev_add(&dev_struct, dev_num, NUMBER_OF_DEVICES);
	if(res < 0)
		goto cdev_add_error;

        proc_dir = proc_mkdir(PROC_DIR_NAME, NULL);
	proc_file = proc_create(PROC_FILE_NAME, NULL, proc_dir, &proc_fops);


	sys_dir = kobject_create_and_add(SYS_DIR_NAME, NULL);
	sysfs_create_file(sys_dir, &sys_file_attr.attr);

        buffer_too_small:
	return -1;

        cdev_add_error:
	device_destroy(chdev_class, dev_num);
	class_destroy(chdev_class);

	unregister_chrdev_region(dev_num, NUMBER_OF_DEVICES);

	kfree(buffer);

	return res;

}


static void __exit exit(void)
{
	kfree(buffer);
	class_destroy(chdev_class);
	unregister_chrdev_region(dev_num, NUMBER_OF_DEVICES);

	remove_proc_subtree(PROC_DIR_NAME, NULL);

	sysfs_remove_file(sys_dir, &sys_file_attr.attr);
	kobject_put(sys_dir);

	printk("Driver removed!\n");
}

module_init(init);
module_exit(exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Dmytro Kalynenko  <dmytrokalynenko@gmail.com>");
MODULE_DESCRIPTION("Simple character device driver");

