#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/err.h>

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
MODULE_AUTHOR("Alexandr.Datsenko <datsenkoalexander@gmail.com>");
MODULE_DESCRIPTION("mpu6050 I2C acc&gyro");
MODULE_LICENSE("GPL");

