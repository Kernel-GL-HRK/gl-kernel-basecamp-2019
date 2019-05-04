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
#include <linux/err.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>

static int mpu6050_probe(struct i2c_client *drv_client, const struct i2c_device_id *id)
{
    dev_info(&drv_client->dev, "i2c driver probed\n");
    //printk(KERN_INFO "mpu6050: mpu6050_probe");
    return 0;
}

static int mpu6050_remove(struct i2c_client *drv_client)
{
    dev_info(&drv_client->dev, "i2c driver remove\n");
    return 0;
}

static const struct i2c_device_id mpu6050_id[] = {
    { "mpu6050", 0 },
    { }
};
MODULE_DEVICE_TABLE(i2c, mpu6050_id);

static struct i2c_driver mpu6050_i2c_driver = {
    .driver = {
        .name = "gl_mpu6050",
    },
    .probe = mpu6050_probe,
    .remove = mpu6050_remove,
    .id_table = mpu6050_id,
};
static int mpu6050_init(void)
{
    int ret;
    ret = i2c_add_driver(&mpu6050_i2c_driver);
    printk(KERN_INFO "mpu6050: module loaded\n");
    printk(KERN_INFO "mpu6050: module add\n");
    return 0;
}

static void mpu6050_exit(void)
{
    
    i2c_del_driver(&mpu6050_i2c_driver);
    printk(KERN_INFO "mpu6050: module del\n");
    printk(KERN_INFO "mpu6050: module exited\n");
}

module_init(mpu6050_init);
module_exit(mpu6050_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Danil Petrov <daaaanil81@gmail.com>");
MODULE_DESCRIPTION("My module for mpu6050 on GY-521");
MODULE_VERSION("0.1");


