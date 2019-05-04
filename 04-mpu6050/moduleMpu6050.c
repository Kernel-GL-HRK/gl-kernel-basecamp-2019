#include <linux/module.h>
#include "mpu6050_reg.h"
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

static int mpu6050_init(void)
{
    printk(KERN_INFO "mpu6050: module loaded\n");
    return 0;
}

static void mpu6050_exit(void)
{
    printk(KERN_INFO "mpu6050: module exited\n");
}

module_init(mpu6050_init);
module_exit(mpu6050_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Danil Petrov <daaaanil81@gmail.com>");
MODULE_DESCRIPTION("My module for mpu6050 on GY-521");
MODULE_VERSION("0.1");


