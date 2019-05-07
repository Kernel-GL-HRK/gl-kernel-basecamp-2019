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
#define DISCHARGE 1000
#define SIXTEEN 65536


struct mpu6050_data {
    struct i2c_client *drv_client;
    int accel_values[3];
    int gyro_values[3];
    int temperatureC[2];
    int temperatureF[2];
};

static struct mpu6050_data gl_mpu6050_data;
static struct class *class_device;

static ssize_t accel_x_show(struct class *class, struct class_attribute *attr, char *buf);
static ssize_t accel_y_show(struct class *class, struct class_attribute *attr, char *buf);
static ssize_t accel_z_show(struct class *class, struct class_attribute *attr, char *buf);
static ssize_t gyro_x_show(struct class *class, struct class_attribute *attr, char *buf);
static ssize_t gyro_y_show(struct class *class, struct class_attribute *attr, char *buf);
static ssize_t gyro_z_show(struct class *class, struct class_attribute *attr, char *buf);
static ssize_t temperature_show(struct class *class, struct class_attribute *attr, char *buf);

struct class_attribute accel_x_attr = __ATTR(ACCEL_X, 0444, &accel_x_show, NULL);
struct class_attribute accel_y_attr = __ATTR(ACCEL_Y, 0444, &accel_y_show, NULL);
struct class_attribute accel_z_attr = __ATTR(ACCEL_Z, 0444, &accel_z_show, NULL);

struct class_attribute gyro_x_attr = __ATTR(GYRO_X, 0444, &gyro_x_show, NULL);
struct class_attribute gyro_y_attr = __ATTR(GYRO_Y, 0444, &gyro_y_show, NULL);
struct class_attribute gyro_z_attr = __ATTR(GYRO_Z, 0444, &gyro_z_show, NULL);

struct class_attribute temperature_attr = __ATTR(TEMP, 0444, &temperature_show, NULL);

static ssize_t accel_x_show(struct class *class, struct class_attribute *attr, char *buf)
{
    return 0;
}
static ssize_t accel_y_show(struct class *class, struct class_attribute *attr, char *buf)
{
    return 0;
}
static ssize_t accel_z_show(struct class *class, struct class_attribute *attr, char *buf)
{
    return 0;
}
static ssize_t gyro_x_show(struct class *class, struct class_attribute *attr, char *buf)
{
    return 0;
}
static ssize_t gyro_y_show(struct class *class, struct class_attribute *attr, char *buf)
{
    return 0;
}
static ssize_t gyro_z_show(struct class *class, struct class_attribute *attr, char *buf)
{
    return 0;
}
static ssize_t temperature_show(struct class *class, struct class_attribute *attr, char *buf)
{
    uint8_t h;
    uint8_t l;
    int result;
    printk(KERN_INFO "Show_Temperature\n");
    h = i2c_smbus_read_byte_data(gl_mpu6050_data.drv_client, REG_TEMP_OUT_H);
    l = i2c_smbus_read_byte_data(gl_mpu6050_data.drv_client, REG_TEMP_OUT_L);
    result = h << 8 | l;
    result = result - SIXTEEN;
    result = result*DISCHARGE/340 + 12420*DISCHARGE/340;
    gl_mpu6050_data.temperatureC[0] = result / DISCHARGE;
    gl_mpu6050_data.temperatureC[1] = result % DISCHARGE;
    gl_mpu6050_data.temperatureF[0] = ((result*9/5)+32*DISCHARGE)/DISCHARGE;
    gl_mpu6050_data.temperatureF[1] = ((result*9/5)+32*DISCHARGE) % DISCHARGE;
    printk(KERN_INFO  "Temperature: %d.%03d C\n", gl_mpu6050_data.temperatureC[0], gl_mpu6050_data.temperatureC[1]);
    printk(KERN_INFO  "Temperature: %d.%03d F\n", gl_mpu6050_data.temperatureF[0], gl_mpu6050_data.temperatureF[1]);
    sprintf(buf,"Temperature: %d.%03d C\nTemperature: %d.%03d F\n", gl_mpu6050_data.temperatureC[0], gl_mpu6050_data.temperatureC[1], gl_mpu6050_data.temperatureF[0], gl_mpu6050_data.temperatureF[1]);
    return strlen(buf);
}




static int mpu6050_probe(struct i2c_client *drv_client, const struct i2c_device_id *id)
{
    int ret;
    dev_info(&drv_client->dev, "i2c driver probed\n");
    dev_info(&drv_client->dev, "i2c client address is 0x%X\n", drv_client->addr);
    
    ret = i2c_smbus_read_byte_data(drv_client, REG_WHO_AM_I);
    if (IS_ERR_VALUE(ret)) {
        dev_err(&drv_client->dev, "i2c_smbus_read_byte_data() failed with error: %d\n", ret);
        return ret;
    }
    if (ret != MPU6050_WHO_AM_I) {
        dev_err(&drv_client->dev, "wrong i2c device found: expected 0x%X, found 0x%X\n", MPU6050_WHO_AM_I, ret);
        return -1;
    }
    dev_info(&drv_client->dev, "i2c mpu6050 device found, WHO_AM_I register value = 0x%X\n", ret);
    
    i2c_smbus_write_byte_data(drv_client, REG_CONFIG, 0);
    i2c_smbus_write_byte_data(drv_client, REG_GYRO_CONFIG, 0);
    i2c_smbus_write_byte_data(drv_client, REG_ACCEL_CONFIG, 0);
    i2c_smbus_write_byte_data(drv_client, REG_FIFO_EN, 0);
    i2c_smbus_write_byte_data(drv_client, REG_INT_PIN_CFG, 0);
    i2c_smbus_write_byte_data(drv_client, REG_INT_ENABLE, 0);
    i2c_smbus_write_byte_data(drv_client, REG_USER_CTRL, 0);
    i2c_smbus_write_byte_data(drv_client, REG_PWR_MGMT_1, 0);
    i2c_smbus_write_byte_data(drv_client, REG_PWR_MGMT_2, 0);
    
    gl_mpu6050_data.drv_client = drv_client;
    return 0;
}

static int mpu6050_remove(struct i2c_client *drv_client)
{
    dev_info(&drv_client->dev, "i2c driver remove\n");
    gl_mpu6050_data.drv_client = 0;
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
    printk(KERN_INFO "mpu6050: module add\n");
    
    if ((class_device = class_create(THIS_MODULE, NAME_SYSCLASS)) < 0) {
        printk(KERN_ERR "Mpu6050: Error with create class_create.\n");
        return -ENOMEM;
    }
    ret = class_create_file(class_device, &accel_x_attr);
    if (ret < 0) {
        printk(KERN_ERR "Mpu6050: Error wih create file accel_x_attr.\n");
        return -ENOMEM;
    }
    ret = class_create_file(class_device, &accel_y_attr);
    if (ret < 0) {
        printk(KERN_ERR "Mpu6050: Error wih create file accel_y_attr.\n");
        return -ENOMEM;
    }
    ret = class_create_file(class_device, &accel_z_attr);
    if (ret < 0) {
        printk(KERN_ERR "Mpu6050: Error wih create file accel_z_attr.\n");
        return -ENOMEM;
    }
    ret = class_create_file(class_device, &gyro_x_attr);
    if (ret < 0) {
        printk(KERN_ERR "Mpu6050: Error wih create file gyro_x_attr.\n");
        return -ENOMEM;
    }
    ret = class_create_file(class_device, &gyro_y_attr);
    if (ret < 0) {
        printk(KERN_ERR "Mpu6050: Error wih create file gyro_y_attr.\n");
        return -ENOMEM;
    }
    ret = class_create_file(class_device, &gyro_z_attr);
    if (ret < 0) {
        printk(KERN_ERR "Mpu6050: Error wih create file gyro_z_attr.\n");
        return -ENOMEM;
    }
    ret = class_create_file(class_device, &temperature_attr);
    if (ret < 0) {
        printk(KERN_ERR "Mpu6050: Error wih create file temperature_attrs.\n");
        return -ENOMEM;
    }
    return 0;
}

static void mpu6050_exit(void)
{
    
    i2c_del_driver(&mpu6050_i2c_driver);
    printk(KERN_INFO "mpu6050: module del\n");
    class_remove_file(class_device, &accel_x_attr);
    class_remove_file(class_device, &accel_y_attr);
    class_remove_file(class_device, &accel_z_attr);
    class_remove_file(class_device, &gyro_x_attr);
    class_remove_file(class_device, &gyro_y_attr);
    class_remove_file(class_device, &gyro_z_attr);
    class_remove_file(class_device, &temperature_attr);
    class_destroy(class_device);
}

module_init(mpu6050_init);
module_exit(mpu6050_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Danil Petrov <daaaanil81@gmail.com>");
MODULE_DESCRIPTION("My module for mpu6050 on GY-521");
MODULE_VERSION("0.1");


