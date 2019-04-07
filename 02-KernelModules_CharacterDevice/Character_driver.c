#include <linux/module.h>
//#include "Create_device.h"
#include "Proc_interface.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Eduard.Voronkin<eduard.voronkin@nure.ua");
MODULE_DESCRIPTION("Character driver");
MODULE_VERSION("0.1");




static int __init char_driver_init(void){
    if(BUF_SIZE < MIN_BUF_SIZE){
        printk( KERN_DEBUG "Buffer size must me at least 1024 bytes.\n");
        return -1;
    }
    if(create_buffer() < 0){
        printk( KERN_DEBUG "Failed to allocate memory...\n");
        return -1;
    }
    init();
    init_proc();
    return 0;
}

static void __exit char_driver_exit(void){
    device_destroy(my_class, dev);
    cdev_del(&my_dev);
    class_destroy(my_class);
    unregister_chrdev_region(dev, 1);
    remove_proc_subtree(DIR_NAME, NULL);
    delete_buffer();
}

module_init(char_driver_init);
module_exit(char_driver_exit);
