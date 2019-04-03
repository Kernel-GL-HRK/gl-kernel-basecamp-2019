#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/moduleparam.h>
#include <linux/stat.h>
#include <linux/slab.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Eduard.Voronkin<eduard.voronkin@nure.ua");
MODULE_DESCRIPTION("Character driver");
MODULE_VERSION("0.1");

#define MIN_BUF_SIZE   1024

static uint BUF_SIZE = MIN_BUF_SIZE; //by default
module_param(BUF_SIZE, uint, S_IRUGO); //Parameter may be read only by owner of the file
MODULE_PARM_DESC(BUF_SIZE, "The size of buffer");


static const char *driver_buffer = NULL; //Pointer to driver buffer
static uint read_position = 0;
static size_t data_length = 0; //Length of data in buffer

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
    data_length = read_position = 0;
}


static int __init char_driver_init(void){
    if(BUF_SIZE < MIN_BUF_SIZE){
        printk( KERN_DEBUG "Buffer size must me at least 1024 bytes.\n");
        return -1;
    }
    if(create_buffer() < 0){
        printk( KERN_DEBUG "Failed to allocate memory...\n");
        return -1;
    }
    //Some actions will be performed here
    return 0;
}

static void __exit char_driver_exit(void){
    delete_buffer();
    //Some actions will be performed here
}

module_init(char_driver_init);
module_exit(char_driver_exit);
