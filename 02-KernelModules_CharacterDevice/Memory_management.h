#include <linux/moduleparam.h>
#include <linux/kernel.h>
#include <linux/stat.h>
#include <linux/slab.h>
#include <linux/printk.h>
#define MIN_BUF_SIZE   1024

static uint BUF_SIZE = MIN_BUF_SIZE; //by default
module_param(BUF_SIZE, uint, S_IRUGO); //Parameter may be read only by owner of the file
MODULE_PARM_DESC(BUF_SIZE, "The size of buffer");


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