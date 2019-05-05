#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/err.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#define N 9
#define N1 5

/* Registed addresses */
#define REG_CONFIG	        0x1A
#define REG_GYRO_CONFIG		0x1B
#define REG_ACCEL_CONFIG	0x1C
#define REG_FIFO_EN		0x23
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

/* Register values */
#define MPU6050_WHO_AM_I	0x68

struct mpu6050_data {
	struct i2c_client *drv_client;
	int accel_values[3];
	int gyro_values[3];
	int temperature_C;
};

static struct mpu6050_data g_mpu6050_data;

static int mpu6050_read_data(void)
{
	int temp_raw;
	struct i2c_client *drv_client = g_mpu6050_data.drv_client;

	if (drv_client == 0)
		return -ENODEV;

	/* accel */
	g_mpu6050_data.accel_values[0] = (s16)((u16)i2c_smbus_read_word_swapped(drv_client, REG_ACCEL_XOUT_H));
	g_mpu6050_data.accel_values[1] = (s16)((u16)i2c_smbus_read_word_swapped(drv_client, REG_ACCEL_YOUT_H));
	g_mpu6050_data.accel_values[2] = (s16)((u16)i2c_smbus_read_word_swapped(drv_client, REG_ACCEL_ZOUT_H));
	/* gyro */
	g_mpu6050_data.gyro_values[0] = (s16)((u16)i2c_smbus_read_word_swapped(drv_client, REG_GYRO_XOUT_H));
	g_mpu6050_data.gyro_values[1] = (s16)((u16)i2c_smbus_read_word_swapped(drv_client, REG_GYRO_YOUT_H));
	g_mpu6050_data.gyro_values[2] = (s16)((u16)i2c_smbus_read_word_swapped(drv_client, REG_GYRO_ZOUT_H));
	/* Temperature in degrees C =
	 * (TEMP_OUT Register Value  as a signed quantity)/340 + 36.53
	 */
	temp_raw = (s16)((u16)i2c_smbus_read_word_swapped(drv_client, REG_TEMP_OUT_H));
	g_mpu6050_data.temperature_C = (int)(temp_raw / 340) + 36;

	dev_info(&drv_client->dev, "sensor data read:\n");
	dev_info(&drv_client->dev, "ACCEL[X,Y,Z] = [%d, %d, %d]\n",
		g_mpu6050_data.accel_values[0],
		g_mpu6050_data.accel_values[1],
		g_mpu6050_data.accel_values[2]);
	dev_info(&drv_client->dev, "GYRO[X,Y,Z] = [%d, %d, %d]\n",
		g_mpu6050_data.gyro_values[0],
		g_mpu6050_data.gyro_values[1],
		g_mpu6050_data.gyro_values[2]);
	dev_info(&drv_client->dev, "TEMP = %d\n",
		g_mpu6050_data.temperature_C);

	return 0;
}

static int mpu6050_probe(struct i2c_client *drv_client,
			 const struct i2c_device_id *id)
{
	int ret;
        printk("PROBE" KERN_DEBUG);

	dev_info(&drv_client->dev,
		"i2c client address is 0x%X\n", drv_client->addr);

	/* Read who_am_i register */
	ret = i2c_smbus_read_byte_data(drv_client, REG_WHO_AM_I);
	if (IS_ERR_VALUE(ret)) {
		dev_err(&drv_client->dev,
			"i2c_smbus_read_byte_data() failed with error: %d\n",
			ret);
		return ret;
	}
	if (ret != MPU6050_WHO_AM_I) {
		dev_err(&drv_client->dev,
			"wrong i2c device found: expected 0x%X, found 0x%X\n",
			MPU6050_WHO_AM_I, ret);
		return -1;
	}
	dev_info(&drv_client->dev,
		"i2c mpu6050 device found, WHO_AM_I register value = 0x%X\n",
		ret);

	/* Setup the device */
	/* No error handling here! */
	i2c_smbus_write_byte_data(drv_client, REG_CONFIG, 0);
	i2c_smbus_write_byte_data(drv_client, REG_GYRO_CONFIG, 0);
	i2c_smbus_write_byte_data(drv_client, REG_ACCEL_CONFIG, 0);
	i2c_smbus_write_byte_data(drv_client, REG_FIFO_EN, 0);
	i2c_smbus_write_byte_data(drv_client, REG_INT_PIN_CFG, 0);
	i2c_smbus_write_byte_data(drv_client, REG_INT_ENABLE, 0);
	i2c_smbus_write_byte_data(drv_client, REG_USER_CTRL, 0);
	i2c_smbus_write_byte_data(drv_client, REG_PWR_MGMT_1, 0);
	i2c_smbus_write_byte_data(drv_client, REG_PWR_MGMT_2, 0);

	g_mpu6050_data.drv_client = drv_client;

	dev_info(&drv_client->dev, "i2c driver probed\n");
	return 0;
}

static int mpu6050_remove(struct i2c_client *drv_client)
{
	g_mpu6050_data.drv_client = 0;

	dev_info(&drv_client->dev, "i2c driver removed\n");
	return 0;
}

static const struct i2c_device_id mpu6050_idtable[] = {
	{ "mpu6050", 0 },
	{ }
};
MODULE_DEVICE_TABLE(i2c, mpu6050_idtable);

static struct i2c_driver mpu6050_i2c_driver = {
	.driver = {
		.name = "gl_mpu6050",
	},

	.probe = mpu6050_probe,
	.remove = mpu6050_remove,
	.id_table = mpu6050_idtable,
};

static ssize_t accel_x_show(struct class *class,
			    struct class_attribute *attr, char *buf)
{
	mpu6050_read_data();

	sprintf(buf, "%d\n", g_mpu6050_data.accel_values[0]);
	return strlen(buf);
}

static ssize_t accel_y_show(struct class *class,
			    struct class_attribute *attr, char *buf)
{
	mpu6050_read_data();

	sprintf(buf, "%d\n", g_mpu6050_data.accel_values[1]);
	return strlen(buf);
}

static ssize_t accel_z_show(struct class *class,
			    struct class_attribute *attr, char *buf)
{
	mpu6050_read_data();

	sprintf(buf, "%d\n", g_mpu6050_data.accel_values[2]);
	return strlen(buf);
}

static ssize_t gyro_x_show(struct class *class,
			   struct class_attribute *attr, char *buf)
{
	mpu6050_read_data();

	sprintf(buf, "%d\n", g_mpu6050_data.gyro_values[0]);
	return strlen(buf);
}

static ssize_t gyro_y_show(struct class *class,
			   struct class_attribute *attr, char *buf)
{
	mpu6050_read_data();

	sprintf(buf, "%d\n", g_mpu6050_data.gyro_values[1]);
	return strlen(buf);
}

static ssize_t gyro_z_show(struct class *class,
			   struct class_attribute *attr, char *buf)
{
	mpu6050_read_data();

	sprintf(buf, "%d\n", g_mpu6050_data.gyro_values[2]);
	return strlen(buf);
}

static ssize_t  temp_c_show(struct class *class,
			 struct class_attribute *attr, char *buf)
{
	mpu6050_read_data();

	sprintf(buf, "%d\n", g_mpu6050_data.temperature_C);
	return strlen(buf);
}
static ssize_t temp_f_show(struct class *class,
			 struct class_attribute *attr, char *buf)
{
	mpu6050_read_data();
        sprintf(buf, "%d\n", g_mpu6050_data.temperature_C * N/N1 + 32);
        return strlen(buf);
}

static struct class_attribute class_attr_accel_x = __ATTR(accel_x, 0444, accel_x_show, NULL);
static struct class_attribute class_attr_accel_y = __ATTR(accel_y, 0444, accel_y_show, NULL);
static struct class_attribute class_attr_accel_z = __ATTR(accel_z, 0444, accel_z_show, NULL);
static struct class_attribute class_attr_gyro_x = __ATTR(gyro_x, 0444, gyro_x_show, NULL);
static struct class_attribute class_attr_gyro_y = __ATTR(gyro_y, 0444, gyro_y_show, NULL);
static struct class_attribute class_attr_gyro_z = __ATTR(gyro_z, 0444, gyro_z_show, NULL);
static struct class_attribute class_attr_temp_c = __ATTR(temp_c, 0444, temp_c_show, NULL);
static struct class_attribute class_attr_temp_f = __ATTR(temp_f, 0444, temp_f_show, NULL);

static int mpu6050_init(void)
{
	int ret;

	/* Create i2c driver */
	ret = i2c_add_driver(&mpu6050_i2c_driver);
	if (ret) {
		pr_err("mpu6050: failed to add new i2c driver: %d\n", ret);
		return ret;
	}
	pr_info("mpu6050: i2c driver created\n");

	/* Create class */
	class_dir = class_create(THIS_MODULE, "mpu6050");
	if (IS_ERR(class_dir)) {
		ret = PTR_ERR(class_dir);
		pr_err("mpu6050: failed to create sysfs class: %d\n", ret);
		return ret;
	}
	pr_info("mpu6050: sysfs class created\n");

	/* Create accel_x */
        ret = class_create_file(class_dir, &class_attr_accel_x);
	if (ret) {
		pr_err("mpu6050: failed to create sysfs class attribute accel_x: %d\n", ret);
		return ret;
	}
	/* Create accel_y */
        ret = class_create_file(class_dir, &class_attr_accel_y);
	if (ret) {
		pr_err("mpu6050: failed to create sysfs class attribute accel_y: %d\n", ret);
		return ret;
	}
	/* Create accel_z */
        ret = class_create_file(class_dir, &class_attr_accel_z);
	if (ret) {
		pr_err("mpu6050: failed to create sysfs class attribute accel_z: %d\n", ret);
		return ret;
	}
	/* Create gyro_x */
        ret = class_create_file(class_dir, &class_attr_gyro_x);
	if (ret) {
		pr_err("mpu6050: failed to create sysfs class attribute gyro_x: %d\n", ret);
		return ret;
	}
	/* Create gyro_y */
        ret = class_create_file(class_dir, &class_attr_gyro_y);
	if (ret) {
		pr_err("mpu6050: failed to create sysfs class attribute gyro_y: %d\n", ret);
		return ret;
	}
	/* Create gyro_z */
        ret = class_create_file(class_dir, &class_attr_gyro_z);  
	if (ret) {
		pr_err("mpu6050: failed to create sysfs class attribute gyro_z: %d\n", ret);
		return ret;
	}
	/* Create temperature */
	ret = class_create_file(class_dir, &class_attr_temp_C);
	if (ret) {
		pr_err("mpu6050: failed to create sysfs class attribute Temp_f: %d\n", ret);;
		return ret;
	}
	ret = class_create_file(class_dir, &class_attr_temp_f);

	pr_info("mpu6050: sysfs class attributes created\n");

	pr_info("mpu6050: module loaded\n");
	return 0;
}

static void mpu6050_exit(void)
{
	if (attr_class) {
		class_remove_file(class_dir, &class_attr_accel_x);
		class_remove_file(class_dir, &class_attr_accel_y);
		class_remove_file(class_dir, &class_attr_accel_z);
		class_remove_file(class_dir, &class_attr_gyro_x);
		class_remove_file(class_dir, &class_attr_gyro_y);
		class_remove_file(class_dir, &class_attr_gyro_z);
		class_remove_file(class_dir, &class_attr_temp_c);
		class_remove_file(class_dir, &class_attr_temp_f);
		pr_info("mpu6050: sysfs class attributes removed\n");

	        class_destroy(class_dir);
		pr_info("mpu6050: sysfs class destroyed\n");
	}

	i2c_del_driver(&mpu6050_i2c_driver);
	pr_info("mpu6050: i2c driver deleted\n");

	pr_info("mpu6050: module exited\n");
}

module_init(mpu6050_init);
module_exit(mpu6050_exit);

MODULE_AUTHOR("KalynenkoDima <dmytrokalynenko@gmail.com>");
MODULE_DESCRIPTION("mpu6050 I2C acc&gyro");
MODULE_LICENSE("GPL");
MODULE_VERSION("0.1");
















