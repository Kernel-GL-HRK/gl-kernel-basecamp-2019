#pragma once
#include <linux/fs.h> //for getting free MAJOR and MINOR
#include <linux/cdev.h> //char_dev creation
#include <linux/types.h> //for dev_t
#include <linux/uaccess.h> //copy_to/from_user
#include <linux/device.h> //for device_creaete
#include "Memory_management.h" //driver_buffer, read_position, data_length
#define NUMBER_OF_DEVICES   1
#define DEVICE_NAME "My_character_device"

ssize_t dev_read (struct file *file_p, char __user *usr_buffer, size_t len, loff_t *offset);
ssize_t dev_write (struct file *file_p, const char __user *usr_buffer, size_t len, loff_t *offset);


static struct file_operations my_fops = {
    .owner = THIS_MODULE,
    .read = dev_read,
    .write = dev_write
};

static dev_t dev; // major and minor number

static struct cdev my_dev;
static struct class *my_class;
void init(void){
    alloc_chrdev_region(&dev, 0/*statring from minor number 0*/, NUMBER_OF_DEVICES, DEVICE_NAME); //ask kernel to allocate free major number
    cdev_init(&my_dev, &my_fops);
    cdev_add(&my_dev, dev, 1);
    my_class = class_create(THIS_MODULE, "Char_driver");
    device_create(my_class, NULL, dev, NULL, DEVICE_NAME );
    
    printk(KERN_DEBUG "Add dev %d -- %d\n", MAJOR(dev), MINOR(dev));
}



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
