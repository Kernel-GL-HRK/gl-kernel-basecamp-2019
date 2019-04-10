#include <linux/module.h>
#include <linux/version.h>
#include <linux/proc_fs.h>
#include <linux/sched.h>
#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/slab.h>

#define PROC_FILENAME "MyCharDev"
#define NAME_SYSCLASS "chardev"
#define SIZE_STRING 10

static struct proc_dir_entry *proc_file;
static dev_t first;
static size_t countFile = 1;
static size_t all_memory = 1024;
static char size_memory[SIZE_STRING];
static struct class *class_device;
struct device *dev_ret;


static ssize_t proc_read(struct file *f, char __user *buffer, size_t len, loff_t *off)
{
    sprintf(size_memory, "%u\n", all_memory);
    len = SIZE_STRING;
    return simple_read_from_buffer(buffer, len, off, size_memory, len);
}

static struct file_operations fops =
{
    .read = proc_read,
};

static int __init dev_init(void) /* Constructor */
{
    printk(KERN_INFO "Start device");
    if ((alloc_chrdev_region(&first, 0, countFile, PROC_FILENAME)) < 0) {
        return -ENOMEM;
    }
    printk(KERN_INFO "Registered: <%d %d>\n", MAJOR(first),MINOR(first));
    proc_file = proc_create(PROC_FILENAME, S_IFREG | S_IRUGO | S_IWUGO, NULL, &fops);
    if(proc_file == NULL) {
        unregister_chrdev_region(first, countFile);
        return -ENOMEM;
    }
    if (IS_ERR(class_device = class_create(THIS_MODULE, NAME_SYSCLASS)))
    {
        remove_proc_entry(PROC_FILENAME, NULL);
        unregister_chrdev_region(first, countFile);
        return PTR_ERR(class_device);
    }
    if (IS_ERR(dev_ret = device_create(class_device, NULL, first, NULL, NAME_SYSCLASS)))
    {
        class_destroy(class_device);
        remove_proc_entry(PROC_FILENAME, NULL);
        unregister_chrdev_region(first, countFile);
        return PTR_ERR(dev_ret);
    }
    return 0;
}

static void __exit dev_exit(void) /* Destructor */
{
    printk(KERN_INFO "End device");
    
    device_destroy(class_device, first);
    class_destroy(class_device);
    remove_proc_entry(PROC_FILENAME, NULL);
    unregister_chrdev_region(first, countFile);
    
}

module_init(dev_init);
module_exit(dev_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Danil Petrov <daaaanil81@gmail.com>");
MODULE_DESCRIPTION("My First Character Driver");
