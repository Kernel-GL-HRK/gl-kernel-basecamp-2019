#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/err.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>

// Define mpu6050 Register values

#define REG_CONFIG		0x1A
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
#define MPU6050_ADDR    	0x68

// Define byteshift parameters

#define SHIFT               	8
#define SHIFT_MASK          	((1 << SHIFT) - 1)

// Define data values

struct mpu6050_i2c_data {
	struct i2c_client *drv_client;
	int accelerometer_values[3];
	int gyroscope_values[3];
	int temperature_values[2];
};

static struct mpu6050_i2c_data mpu6050_data;
static struct class *attribute_class;


static int mpu6050_read_data(void)
{
	int raw_temp_data;

	struct i2c_client *drv_client = mpu6050_data.drv_client;

	if (drv_client == 0)
		return -ENODEV;

	// Accelerometer data reading

	mpu6050_data.accelerometer_values[0] = (s16)((u16)i2c_smbus_read_word_swapped(drv_client, REG_ACCEL_XOUT_H));
	mpu6050_data.accelerometer_values[1] = (s16)((u16)i2c_smbus_read_word_swapped(drv_client, REG_ACCEL_YOUT_H));
	mpu6050_data.accelerometer_values[2] = (s16)((u16)i2c_smbus_read_word_swapped(drv_client, REG_ACCEL_ZOUT_H));

	// Gyroscope data reading

	mpu6050_data.gyroscope_values[0] = (s16)((u16)i2c_smbus_read_word_swapped(drv_client, REG_GYRO_XOUT_H));
	mpu6050_data.gyroscope_values[1] = (s16)((u16)i2c_smbus_read_word_swapped(drv_client, REG_GYRO_YOUT_H));
	mpu6050_data.gyroscope_values[2] = (s16)((u16)i2c_smbus_read_word_swapped(drv_client, REG_GYRO_ZOUT_H));

	// Temperature value reading

	raw_temp_data = (((s16)((u16)i2c_smbus_read_word_swapped(drv_client, REG_TEMP_OUT_H)) << SHIFT) | ((u16)i2c_smbus_read_word_swapped(drv_client, REG_TEMP_OUT_L)));

	// Temperature transform to Fahrenheit scale

	raw_temp_data = (((raw_temp_data/340) + (3653 << SHIFT)/100) * 18)/10 + (32 << SHIFT);

	mpu6050_data.temperature_values[0] = raw_temp_data >> SHIFT;
	mpu6050_data.temperature_values[1] = raw_temp_data & SHIFT_MASK;

	return 0;
}

// Probe/remove device

static int mpu6050_probe(struct i2c_client *drv_client, const struct i2c_device_id *id)
{
	int err;

	printk(KERN_INFO "mpu6050: Driver probe started\n");

	err=i2c_smbus_read_byte_data(drv_client, REG_WHO_AM_I);
	if(err != MPU6050_ADDR){
			printk(KERN_ERR "Wrong i2c device address: expected 0x%X, found 0x%X\n", MPU6050_ADDR, err);
		return -1;
	}
	else
		printk(KERN_INFO "mpu6050: i2c address is 0x%X\n", drv_client->addr);

	i2c_smbus_write_byte_data(drv_client, REG_CONFIG, 0);
	i2c_smbus_write_byte_data(drv_client, REG_GYRO_CONFIG, 0);
	i2c_smbus_write_byte_data(drv_client, REG_ACCEL_CONFIG, 0);
	i2c_smbus_write_byte_data(drv_client, REG_FIFO_EN, 0);
	i2c_smbus_write_byte_data(drv_client, REG_INT_PIN_CFG, 0);
	i2c_smbus_write_byte_data(drv_client, REG_INT_ENABLE, 0);
	i2c_smbus_write_byte_data(drv_client, REG_USER_CTRL, 0);
	i2c_smbus_write_byte_data(drv_client, REG_PWR_MGMT_1, 0);
	i2c_smbus_write_byte_data(drv_client, REG_PWR_MGMT_2, 0);

	mpu6050_data.drv_client = drv_client;

	printk(KERN_INFO "mpu6050: Driver probed\n");
	return 0;

}

static int mpu6050_remove(struct i2c_client *drv_client)
{
	mpu6050_data.drv_client = 0;
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

// Accelerometer show functions

// X axis

static ssize_t accelerometer_x_show(struct class *class, struct class_attribute *attr, char *buf)
{
	mpu6050_read_data();

	sprintf(buf, "%d\n", mpu6050_data.accelerometer_values[0]);
	return strlen(buf);
}

static struct class_attribute class_attr_accelerometer_x = __ATTR(accelerometer_x, 0444, &accelerometer_x_show, NULL);

// Y axis

static ssize_t accelerometer_y_show(struct class *class, struct class_attribute *attr, char *buf)
{
	mpu6050_read_data();

	sprintf(buf, "%d\n", mpu6050_data.accelerometer_values[1]);
	return strlen(buf);
}

static struct class_attribute class_attr_accelerometer_y = __ATTR(accelerometer_y, 0444, &accelerometer_y_show, NULL);

// Z axis

static ssize_t accelerometer_z_show(struct class *class, struct class_attribute *attr, char *buf)
{
	mpu6050_read_data();

	sprintf(buf, "%d\n", mpu6050_data.accelerometer_values[2]);
	return strlen(buf);
}

static struct class_attribute class_attr_accelerometer_z = __ATTR(accelerometer_z, 0444, &accelerometer_z_show, NULL);

// Gyroscope show functions

// X axis

static ssize_t gyroscope_x_show(struct class *class, struct class_attribute *attr, char *buf)
{
	mpu6050_read_data();

	sprintf(buf, "%d\n", mpu6050_data.gyroscope_values[0]);
	return strlen(buf);
}

static struct class_attribute class_attr_gyroscope_x = __ATTR(gyroscope_x, 0444, &gyroscope_x_show, NULL);

// Y axis

static ssize_t gyroscope_y_show(struct class *class, struct class_attribute *attr, char *buf)
{
	mpu6050_read_data();

	sprintf(buf, "%d\n", mpu6050_data.gyroscope_values[1]);
	return strlen(buf);
}

static struct class_attribute class_attr_gyroscope_y = __ATTR(gyroscope_y, 0444, &gyroscope_y_show, NULL);

// Z axis

static ssize_t gyroscope_z_show(struct class *class, struct class_attribute *attr, char *buf)
{
	mpu6050_read_data();

	sprintf(buf, "%d\n", mpu6050_data.gyroscope_values[2]);
	return strlen(buf);
}

static struct class_attribute class_attr_gyroscope_z = __ATTR(gyroscope_z, 0444, &gyroscope_z_show, NULL);

// Temperature show

static ssize_t temperature_show(struct class *class, struct class_attribute *attr, char *buf)
{
	mpu6050_read_data();
	
	sprintf(buf, "%d.%d\n", mpu6050_data.temperature_values[0], mpu6050_data.temperature_values[1]);
	return strlen(buf);
}

static struct class_attribute class_attr_temperature = __ATTR(temperature, 0444, &temperature_show, NULL);

// Init/exit functions

static int mpu6050_init(void)
{
	int ret;

	//Add driver

	ret = i2c_add_driver(&mpu6050_driver);
	if (ret) {
		printk(KERN_ERR "mpu6050: Failed to add new i2c device: %d\n", ret);
		return ret;
	}

	printk(KERN_INFO "mpu6050: i2c driver created\n");

	// Sysfs class creation

	attribute_class = class_create(THIS_MODULE, "mpu6050");
	if (IS_ERR(attribute_class)) {
		pr_err("mpu6050: Failed to create sysfs class\n");
		return -1;
	}

	printk(KERN_INFO "mpu6050: Sysfs class created\n");

	// Accelerometer X interface creation

	ret = class_create_file(attribute_class, &class_attr_accelerometer_x);
	if (ret) {
		printk(KERN_ERR "mpu6050: Failed to create sysfs class attribute accelerometer_x\n");
		return ret;
	}

	// Accelerometer Y interface creation

	ret = class_create_file(attribute_class, &class_attr_accelerometer_y);
	if (ret) {
		printk(KERN_ERR "mpu6050: Failed to create sysfs class attribute accelerometer_y\n");
		return ret;
	}

	// Accelerometer Z interface creation

	ret = class_create_file(attribute_class, &class_attr_accelerometer_z);
	if (ret) {
		printk(KERN_ERR "mpu6050: Failed to create sysfs class attribute accelerometer_z\n");
		return ret;
	}

	// Gyroscope X interface creation

	ret = class_create_file(attribute_class, &class_attr_gyroscope_x);
	if (ret) {
		printk(KERN_ERR "mpu6050: Failed to create sysfs class attribute gyroscope_x\n");
		return ret;
	}

	// Gyroscope Y interface creation

	ret = class_create_file(attribute_class, &class_attr_gyroscope_y);
	if (ret) {
		printk(KERN_ERR "mpu6050: Failed to create sysfs class attribute gyroscope_y\n");
		return ret;
	}

	// Gyroscope Z interface creation

	ret = class_create_file(attribute_class, &class_attr_gyroscope_z);
	if (ret) {
		printk(KERN_ERR "mpu6050: Failed to create sysfs class attribute gyroscope_z\n");
		return ret;
	}

	// Temperature interface creation

	ret = class_create_file(attribute_class, &class_attr_temperature);
	if (ret) {
		printk(KERN_ERR "mpu6050: failed to create sysfs class attribute temperature\n");
		return ret;
	}

	printk(KERN_INFO "mpu6050: Module loaded\n");
	return 0;
}

static void mpu6050_exit(void)
{
   
	class_remove_file(attribute_class, &class_attr_accelerometer_x);
	class_remove_file(attribute_class, &class_attr_accelerometer_y);
	class_remove_file(attribute_class, &class_attr_accelerometer_z);

	class_remove_file(attribute_class, &class_attr_gyroscope_x);
	class_remove_file(attribute_class, &class_attr_gyroscope_y);
	class_remove_file(attribute_class, &class_attr_gyroscope_z);

	class_destroy(attribute_class);
	i2c_del_driver(&mpu6050_driver);

	printk(KERN_INFO "mpu6050: i2c driver deleted\n");
	printk(KERN_INFO "mpu6050: Module exited\n");
}

module_init(mpu6050_init);
module_exit(mpu6050_exit);

MODULE_AUTHOR("Bogdan.Shchogolev <bogdan993000@gmail.com>");
MODULE_DESCRIPTION("mpu6050 I2C");
MODULE_LICENSE("GPL");

