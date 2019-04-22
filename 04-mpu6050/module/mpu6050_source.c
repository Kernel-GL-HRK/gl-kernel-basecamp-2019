#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/err.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>

// Define mpu6050 Register values

#define REG_CONFIG			0x1A
#define REG_GYRO_CONFIG		0x1B
#define REG_ACCEL_CONFIG	0x1C
#define REG_FIFO_EN			0x23
#define REG_INT_PIN_CFG		0x37
#define REG_INT_ENABLE		0x38
#define REG_ACCEL_XOUT_H	0x3B
#define REG_ACCEL_XOUT_L	0x3C
#define REG_ACCEL_YOUT_H	0x3D
#define REG_ACCEL_YOUT_L	0x3E
#define REG_ACCEL_ZOUT_H	0x3F
#define REG_ACCEL_ZOUT_L	0x40
#define REG_TEMP_OUT_H		0x41
#define REG_TEMP_OUT_L		0x42
#define REG_GYRO_XOUT_H		0x43
#define REG_GYRO_XOUT_L		0x44
#define REG_GYRO_YOUT_H		0x45
#define REG_GYRO_YOUT_L		0x46
#define REG_GYRO_ZOUT_H		0x47
#define REG_GYRO_ZOUT_L		0x48
#define REG_USER_CTRL		0x6A
#define REG_PWR_MGMT_1		0x6B
#define REG_PWR_MGMT_2		0x6C
#define REG_WHO_AM_I		0x75

// Probe/remove device

static int mpu6050_probe(struct i2c_client *drv_client, const struct i2c_device_id *id)
{
    printk(KERN_INFO "mpu6050: Driver probe started\n");
    printk(KERN_INFO "mpu6050: i2c address is 0x%X\n", drv_client->addr);
    printk(KERN_INFO "mpu6050: Driver probed\n");
    return 0;

}

static int mpu6050_remove(struct i2c_client *drv_client)
{

    printk(KERN_INFO "mpu6050: i2c driver removed\n");

    return 0;

}

static const struct i2c_device_id mpu6050_idtable [] = {
    
    { "mpu6050", 0 },
    { }

};

MODULE_DEVICE_TABLE(i2c, mpu6050_idtable);

static struct i2c_driver mpu6050_driver = {

    .driver = {
    .name = "mpu6050",
    },
    .probe = mpu6050_probe ,
    .remove = mpu6050_remove,
    .id_table = mpu6050_idtable,
};

// Init/exit functions

static int mpu6050_init(void)
{
    int ret;

    ret = i2c_add_driver(&mpu6050_driver);

    if (ret) {
    printk(KERN_ERR "mpu6050: Failed to add new i2c device: %d\n", ret);
    return ret;
    }

    printk(KERN_INFO "mpu6050: i2c driver created\n");
	printk(KERN_INFO "mpu6050: Module loaded\n");
	return 0;
}

static void mpu6050_exit(void)
{
    i2c_del_driver(&mpu6050_driver);

    printk(KERN_INFO "mpu6050: i2c driver deleted\n");
	printk(KERN_INFO "mpu6050: Module exited\n");
}

module_init(mpu6050_init);
module_exit(mpu6050_exit);

MODULE_AUTHOR("Bogdan.Shchogolev <bogdan993000@gmail.com>");
MODULE_DESCRIPTION("mpu6050 I2C");
MODULE_LICENSE("GPL");
