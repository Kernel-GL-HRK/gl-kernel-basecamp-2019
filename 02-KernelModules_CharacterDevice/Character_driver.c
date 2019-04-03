#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/moduleparam.h>
#include <linux/stat.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Eduard.Voronkin<eduard.voronkin@nure.ua");
MODULE_DESCRIPTION("Character driver");
MODULE_VERSION("0.1");

#define MIN_BUF_SIZE   1024

static uint BUF_SIZE = MIN_BUF_SIZE; //by default
module_param(BUF_SIZE, uint, S_IRUGO); //Parameter may be read only by owner of the file
MODULE_PARM_DESC(BUF_SIZE, "The size of buffer");

static int __init char_driver_init(void){
    if(BUF_SIZE < MIN_BUF_SIZE){
        printk("Buffer size must me at least 1024 bytes.\n");
        return -1;
    }
    return 0;
        //Some actions will be performed here
}

static void __exit char_driver_exit(void){
    //Some actions will be performed here
}

module_init(char_driver_init);
module_exit(char_driver_exit);
