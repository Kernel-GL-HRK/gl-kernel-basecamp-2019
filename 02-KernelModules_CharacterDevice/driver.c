/*
 * Шаблон для разработки драйвера символьного устройства
 * driver.c -- very simple chrdev driver
 */

#include <linux/module.h>
#include <linux/moduleparam.h>  /* command-line args */
#include <linux/init.h>
#include <linux/kernel.h>       /* printk() */
#include <linux/slab.h>		    /* kfree(), kmalloc() */
#include <linux/fs.h>           /* file_operations */
#include <linux/types.h>        /* dev_t */
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <asm/uaccess.h>        /* copy_*_user */
#include <linux/proc_fs.h>
#include <linux/kobject.h>
#include <linux/sysfs.h>


/*
 * Инициализация и завершение модуля
 */

static int __init device_init( void ) 
{	
	pr_info("chrdev: module loaded\n");
	return 0;
}

static void __exit device_exit( void ) 
{	
	pr_info("chrdev: module exited\n");
}


module_init( device_init );
module_exit( device_exit );

MODULE_LICENSE("GPL");
MODULE_AUTHOR ("Ihor Ryzhov <ihor.ryzhov@nure.ua>");

