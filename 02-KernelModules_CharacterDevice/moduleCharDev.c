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
#include <linux/sysfs.h>

#define PROC_FILENAME "MyCharDev"
#define NAME_SYSCLASS "chardev"
#define SIZE_STRING 20
#define BUFFER_SIZE 1024
#define SYSFS_NAME Cleanup

static struct proc_dir_entry *proc_file;
static dev_t first;
static size_t countFile = 1;
static size_t all_memory = 0;
static char size_memory[SIZE_STRING];
static char* dev_buffer;
static struct class *class_device;
struct device *dev_ret;
static struct cdev c_dev;
static size_t full_memory = 0;
static ssize_t sysfs_show(struct class*, struct class_attribute*, char*);
static ssize_t sysfs_store(struct class*, struct class_attribute*, const char*, size_t );
static ssize_t proc_read(struct file*, char __user*, size_t len, loff_t*);
struct class_attribute sysfs = __ATTR(SYSFS_NAME, 0664, &sysfs_show, &sysfs_store);

static int create_buffer(void)
{
    all_memory += BUFFER_SIZE;
    dev_buffer = kmalloc(all_memory, GFP_KERNEL);
    if (NULL == dev_buffer)
        return -ENOMEM;
    memset(dev_buffer, 0, all_memory);
    return 0;
}


static void cleanup_buffer(void)
{
    if (dev_buffer) {
        memset(dev_buffer, 0, all_memory);
        kfree(dev_buffer);
        dev_buffer = NULL;
    }
}
static int check_ascii(const char*buf)
{
    int len = strlen(buf) - 1;
    unsigned int time;
    int i;
    printk(KERN_INFO "check_ascii: %s and size: %d\n", buf, len);
    for (i = 0; i < len; i++)
    {
        time = buf[i];
        printk(KERN_INFO "%d", time);
        if (time < 33 || time > 126) {
            return -EFAULT;
        }
    }
    return 0;
}
static ssize_t sysfs_show(struct class *class, struct class_attribute *attr, char *buf)
{
    printk(KERN_INFO "Sysfs_show\n");
    memset(dev_buffer, 0, all_memory);
    full_memory = 0;
    return 0;
}
static ssize_t sysfs_store(struct class *class, struct class_attribute *attr, const char *buf, size_t count)
{
    printk(KERN_INFO "Sysfs_store\n");
    memset(dev_buffer, 0, all_memory);
    full_memory = 0;
    return count;
}
static ssize_t proc_read(struct file *f, char __user *buffer, size_t len, loff_t *off)
{
    sprintf(size_memory, "%u %u\n", all_memory, all_memory - full_memory);
    len = SIZE_STRING;
    return simple_read_from_buffer(buffer, len, off, size_memory, len);
}

static int dev_open(struct inode *i, struct file *f)
{
    printk(KERN_INFO "Driver: open()\n");
    return 0;
}
static int dev_close(struct inode *i, struct file *f)
{
    printk(KERN_INFO "Driver: close()\n");
    return 0;
}
static ssize_t dev_read(struct file *f, char __user *buf, size_t len, loff_t *off)
{
    int ret;
    printk(KERN_INFO "chrdev: read from file %s\n", f->f_path.dentry->d_iname);
    if (len > full_memory) len = full_memory;
    ret = copy_to_user(buf, dev_buffer, len);
    if (ret) {
        printk(KERN_ERR "chrdev: copy_to_user failed: %d\n", ret);
        return -EFAULT;
    }
    full_memory = 0;
    printk(KERN_INFO "chrdev: %d bytes read\n", len);
    return len;
}
static ssize_t dev_write(
                         struct file *f, const char __user *buf, size_t len, loff_t *off)
{
    int ret;
    printk(KERN_INFO "chrdev: write to file %s\n", f->f_path.dentry->d_iname);
    full_memory = len;
    if (full_memory > all_memory) full_memory = all_memory;
    if (check_ascii(buf) < 0) {
        printk(KERN_ERR "chrdev: used not allowed characters\n");
        return -EFAULT;
    }
    ret = copy_from_user(dev_buffer, buf, full_memory);
    if (ret) {
        printk(KERN_ERR "chrdev: copy_from_user failed: %d\n", ret);
        return -EFAULT;
    }
    printk(KERN_INFO "chrdev: %d bytes written\n", full_memory);
    return len;
}
static struct file_operations proc_fops =
{
    .read = proc_read,
};

static struct file_operations dev_fops =
{
    .owner = THIS_MODULE,
    .open = dev_open,
    .release = dev_close,
    .read = dev_read,
    .write = dev_write
};

static int __init dev_init(void) /* Constructor */
{
    int res;
    printk(KERN_INFO "Start device");
    if ((alloc_chrdev_region(&first, 0, countFile, PROC_FILENAME)) < 0) {
        goto fail;
    }
    printk(KERN_INFO "Registered: <%d %d>\n", MAJOR(first),MINOR(first));
    proc_file = proc_create(PROC_FILENAME, S_IFREG | S_IRUGO | S_IWUGO, NULL, &proc_fops);
    if(proc_file == NULL) {
        goto fail;
    }
    printk(KERN_INFO "File /proc/MyCharDev was created\n");
    
    if (IS_ERR(class_device = class_create(THIS_MODULE, NAME_SYSCLASS))) {
        goto fail;
    }
    printk(KERN_INFO "Class /sys/class/chardev was created\n");
    res = class_create_file(class_device, &sysfs);
    printk(KERN_INFO "File /sys/class/cleanup was created\n");
    if (res < 0) {
        goto fail;
    }
    if (IS_ERR(dev_ret = device_create(class_device, NULL, first, NULL, NAME_SYSCLASS))) {
        goto fail;
    }
    printk(KERN_INFO "File /dev/chardev was created\n");
    cdev_init(&c_dev, &dev_fops);
    if (cdev_add(&c_dev, first, countFile) < 0){
        goto fail;
    }
    if (create_buffer() < 0) {
        goto fail;
    }
    return 0;
fail:
    printk(KERN_ERR "Error");
    cleanup_buffer();
    cdev_del(&c_dev);
    device_destroy(class_device, first);
    class_remove_file(class_device, &sysfs);
    class_destroy(class_device);
    remove_proc_entry(PROC_FILENAME, NULL);
    unregister_chrdev_region(first, countFile);
    return -ENOMEM;
}

static void __exit dev_exit(void) /* Destructor */
{
    
    printk(KERN_INFO "End device");
    cleanup_buffer();
    cdev_del(&c_dev);
    device_destroy(class_device, first);
    class_remove_file(class_device, &sysfs);
    class_destroy(class_device);
    remove_proc_entry(PROC_FILENAME, NULL);
    unregister_chrdev_region(first, countFile);
}

module_init(dev_init);
module_exit(dev_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Danil Petrov <daaaanil81@gmail.com>");
MODULE_DESCRIPTION("My First Character Driver");
