#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/err.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>

static int mpu6050_init(void)
{
	printk(KERN_INFO "mpu6050: Module loaded\n");
	return 0;
}

static void mpu6050_exit(void)
{
	printk(KERN_INFO "mpu6050: Module exited\n");
}

module_init(mpu6050_init);
module_exit(mpu6050_exit);

MODULE_AUTHOR("Bogdan.Shchogolev <bogdan993000@gmail.com>");
MODULE_DESCRIPTION("mpu6050 I2C");
MODULE_LICENSE("GPL");
