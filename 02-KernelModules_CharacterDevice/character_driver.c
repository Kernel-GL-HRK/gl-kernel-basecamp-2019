#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/kernel.h>
#include <linux/stat.h>
#include <linux/slab.h>
#include <linux/printk.h>
#include <linux/fs.h> //for getting free MAJOR and MINOR
#include <linux/cdev.h> //char_dev creation
#include <linux/types.h> //for dev_t
#include <linux/uaccess.h> //copy_to/from_user
#include <linux/device.h> //for device_creaete
#include <linux/proc_fs.h>
#include <linux/kobject.h> // for kobj and kobj_create
#include <linux/sysfs.h> // for sysfs_create_file
#include <linux/err.h> // for IS_ERR macro


MODULE_LICENSE("GPL");
MODULE_AUTHOR("Eduard.Voronkin<eduard.voronkin@nure.ua");
MODULE_DESCRIPTION("Character driver");
MODULE_VERSION("0.1");


#define MIN_BUF_SIZE   1024

static uint BUF_SIZE = MIN_BUF_SIZE; //by default
module_param(BUF_SIZE, uint, S_IRUGO); //Parameter may be read only by owner of the file
MODULE_PARM_DESC(BUF_SIZE, "The size of buffer");

//MEMORY MANAGMENT
static const char *driver_buffer = NULL; //Pointer to driver buffer
static size_t data_size = 0; //Size of data in buffer

static int create_buffer(void){
    driver_buffer = kmalloc(BUF_SIZE, GFP_KERNEL);
    if(driver_buffer == NULL)
        return -ENOMEM;
    return 0;
}


static void delete_buffer(void){
    if(driver_buffer != NULL){
        kfree(driver_buffer);
        driver_buffer = NULL;
    }
    data_size = 0;
}

//DEVICE CREATION
#define NUMBER_OF_DEVICES   1
#define DEVICE_NAME "My_character_device"

ssize_t dev_read (struct file *file_p, char __user *usr_buffer, size_t len, loff_t *offset){
   printk(KERN_DEBUG "My buff size is: %d, required length of read: %d, offset is: %lld\n", data_size, len, *offset);

    ssize_t ret;
    if((*offset >=  data_size) || (*offset > len)){
        ret = 0;
        goto out;
    }

    else if(len > data_size){
        if(copy_to_user(usr_buffer + *offset, driver_buffer, data_size)){
            ret = -EFAULT;
            goto out;
        }
        *offset += data_size;
        data_size = 0;
        ret = *offset;
        goto out;
    }
    else{
        if(copy_to_user(usr_buffer + *offset, driver_buffer, len)){
            ret = -EFAULT;
            goto out;
        }
        *offset += len;
        data_size -= len;
        ret =  len;
        goto out;
        
    }
    out:
        return ret;
}

ssize_t dev_write (struct file *file_p, const char __user *usr_buffer, size_t len, loff_t *offset){
    ssize_t ret;
    if(len + data_size > BUF_SIZE){
        printk( KERN_ERR "Not enough memory\n");
        ret = -ENOMEM;
        goto out;
    }
    else{
        if(copy_from_user(driver_buffer + data_size, usr_buffer, len)){
            ret = -EFAULT;
            goto out;
        }
    data_size += len;
    ret = len;
    goto out;
    }
    
    

    out:
        printk(KERN_DEBUG "Wrote %d bytes, my buffer size now is: %d\n", len, data_size);
        return ret;
}

static struct file_operations my_fops = {
    .owner = THIS_MODULE,
    .read = dev_read,
    .write = dev_write
};

static dev_t dev; // major and minor number

static struct cdev my_dev;
static struct class *my_class;
static int init(void){
    if(0 >= alloc_chrdev_region(&dev, 0/*statring from minor number 0*/, NUMBER_OF_DEVICES, DEVICE_NAME)) //ask kernel to allocate free major number
        return -1;
    cdev_init(&my_dev, &my_fops);
    if(0 > cdev_add(&my_dev, dev, 1))
        return -1;
    my_class = class_create(THIS_MODULE, "Char_driver");
    if(IS_ERR(my_class))
        return -1;
    device_create(my_class, NULL, dev, NULL, DEVICE_NAME );
    return 0;
    printk(KERN_DEBUG "Add dev %d -- %d\n", MAJOR(dev), MINOR(dev));
}


// /proc INTERFACE
#define PROC_DIR_NAME    "My_proc_stats"
#define PROC_FILE_NAME   "Info"

ssize_t proc_read_info (struct file *file_p, char __user *user_buf, size_t len, loff_t *offset){
    char answer[100];
    if(*offset != 0)
        return 0;
    int answ_length = sprintf(answer, "Size of written data: %d\n"
                    "Size of buffer: %d\n", data_size, BUF_SIZE );
    *offset += answ_length;
    if(copy_to_user(user_buf, answer, answ_length)){
        return -EFAULT;
    }
    else
        return answ_length;
}


static struct proc_dir_entry *proc_dir; //direcotry
static struct proc_dir_entry *proc_info_file; //file with info

static struct file_operations proc_ops = {
    .owner = THIS_MODULE,
    .read = proc_read_info
};

static void init_proc(void)
{
    proc_dir = proc_mkdir(PROC_DIR_NAME, NULL);
    proc_info_file = proc_create(PROC_FILE_NAME, S_IRUGO, proc_dir, &proc_ops);
}


// /sys INTERFACE
#define SYS_DIR_NAME    "My_sys_operations"
#define SYS_FILE_NAME   Clear


ssize_t store_clear(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count){
    if(buf[0] != '0'){
        memset(driver_buffer, 0, data_size);
        data_size = 0;
        printk(KERN_DEBUG "Successfully cleared buffer\n");
    }
    return count;
}
static struct  kobject *kobj_dir;
static struct kobj_attribute file_attr = __ATTR(SYS_FILE_NAME, S_IWUSR, NULL, store_clear);

int init_sys(void){
    kobj_dir = kobject_create_and_add(SYS_DIR_NAME, NULL);
    if(IS_ERR(kobj_dir))
        return -1;
    sysfs_create_file(kobj_dir, &file_attr.attr);
}




static int __init char_driver_init(void){
    int retval = 0;
    if(BUF_SIZE < MIN_BUF_SIZE){
        printk( KERN_DEBUG "Buffer size must me at least 1024 bytes.\n");
        retval =  -1;
        goto out;
    }
    if(0 > create_buffer()){
        printk( KERN_DEBUG "Failed to allocate memory...\n");
        retval =  -1;
        goto out;
    }
    if(-1 == init()){
        retval =  -1;
        goto out;
    }
    init_proc();
    if(-1 == init_sys()){
        printk( KERN_DEBUG "Failed to init sys...\n");
        retval =  -1;
        goto out;
    }
    out:
        return retval;
}

static void __exit char_driver_exit(void){
    // Uninitializing /sys/My_sys_operations interface
    kobject_put(kobj_dir);
    sysfs_remove_file(kobj_dir, &file_attr.attr);

    // Uninitializing /proc/My_proc_stats interface
    remove_proc_subtree(PROC_DIR_NAME, NULL);

    // Deleting device from system and free MAJOR and MINOR numbers
    device_destroy(my_class, dev);
    cdev_del(&my_dev);
    class_destroy(my_class);
    unregister_chrdev_region(dev, 1);

    // Free allocated for driver buffer memory
    delete_buffer();
}

module_init(char_driver_init);
module_exit(char_driver_exit);
