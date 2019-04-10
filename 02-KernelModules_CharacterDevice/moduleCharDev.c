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

#define PROC_FILENAME "MyModule"
#define DRIVER_NAME "MyDriver"
#define SIZE_STRING 10

static struct proc_dir_entry *proc_file;
static dev_t first;
static size_t countFile = 1;
static size_t all_memory = 1024;
static char size_memory[SIZE_STRING];

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
        return -ENOMEM;
    }
    return 0;
}

static void __exit dev_exit(void) /* Destructor */
{
    printk(KERN_INFO "End device");
    remove_proc_entry(PROC_FILENAME, NULL);
    unregister_chrdev_region(first, countFile);
}

module_init(dev_init);
module_exit(dev_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Danil Petrov <daaaanil81@gmail.com>");
MODULE_DESCRIPTION("My First Character Driver");
