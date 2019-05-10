#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Maxim Primerov <primerovmax@gmail.com>");
MODULE_DESCRIPTION("Driver for temperature sensor on the mpu6050 device");
MODULE_VERSION("0.1");

#define REG_WHO_AM_I 117
#define REG_PWR_MGMT_1 107
#define REG_TEMP_OUT_H 0x41
#define ONE_TEMP_POINT 1000

#define MPU6050_WHO_AM_I 0x68
#define MPU6050_DEVICE_RESET 0x00

struct mpu6050_data {
	struct i2c_client *client;
	int temperature;
};

static struct mpu6050_data device_data;

static int mpu6050_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
	int ret;

	pr_info("mpu6050: i2c client address is 0x%X\n", client->addr);

	ret = i2c_smbus_read_byte_data(client, REG_WHO_AM_I);
	if (IS_ERR_VALUE(ret)) {
		pr_err("mpu6050: i2c_smbus_read_byte_data() failed with error: %d\n", ret);
		goto err;
	}

	if (ret != MPU6050_WHO_AM_I) {
		pr_err("mpu6050: wrong i2c device found: expected 0x%X, found 0x%X\n", MPU6050_WHO_AM_I, ret);
		ret = -1;
		goto err;
	}
	pr_info("mpu6050: i2c mpu6050 device found, WHO_AM_I register value = 0x%X\n", ret);

	i2c_smbus_write_byte_data(client, REG_PWR_MGMT_1, MPU6050_DEVICE_RESET);

	device_data.client = client; 

	pr_info("mpu6050: i2c driver probed\n");

	return 0;

	err:
		return ret;
}

static int mpu6050_remove(struct i2c_client *client)
{
	device_data.client = 0;

	pr_info("mpu6050: i2c driver removed\n");

	return 0;
}

static const struct i2c_device_id mpu6050_idtable[] = {
	{ "mpu6050", 0 },
	{ }
};
MODULE_DEVICE_TABLE(i2c, mpu6050_idtable);

static struct i2c_driver mpu6050_i2c_driver = {
	.driver = {
		.name = "con_mpu6050",
	},
	
	.probe = mpu6050_probe,
	.remove = mpu6050_remove,
	.id_table = mpu6050_idtable,
};

static int mpu6050_read_data(void)
{
	int ret;
	int temp;

	if (device_data.client == 0) {
		ret = -ENODEV;
		goto err;
	}

	temp = (s16)((u16)i2c_smbus_read_word_swapped(device_data.client, REG_TEMP_OUT_H));
	device_data.temperature = ((((temp + 12420 + 170) * ONE_TEMP_POINT) / 340) * 9) / 5 + 32000;

	pr_info("mpu6050: temperature = %d.%03d\n",
                device_data.temperature / ONE_TEMP_POINT,
                device_data.temperature % ONE_TEMP_POINT);

	return 0;

	err:
		return ret;
}

static ssize_t temperature_show(struct class *class, struct class_attribute *attr, char *buf)
{	                     
	mpu6050_read_data();

	sprintf(buf, "%d.%03d\n", device_data.temperature / ONE_TEMP_POINT, device_data.temperature % ONE_TEMP_POINT);

	return strlen(buf);
}	                     

CLASS_ATTR_RO(temperature);

static struct class *attr_class;

static void mpu6050_exit(void)
{
	if (attr_class) {
		class_remove_file(attr_class, &class_attr_temperature);
		pr_info("mpu6050: sysfs class attributes removed\n");

		class_destroy(attr_class);
		pr_info("mpu6050: sysfs class destroyed\n");
	}

	i2c_del_driver(&mpu6050_i2c_driver);
	pr_info("mpu6050: i2c driver deleted\n");

	pr_info("mpu6050: module exited\n");
}

static int mpu6050_init(void)
{
	int ret;

	ret = i2c_register_driver(THIS_MODULE, &mpu6050_i2c_driver);
	if (ret) {
		pr_err("mpu6050: failed to add new i2c driver: %d\n", ret);
		goto err;
	}
	pr_info("mpu6050: i2c driver created %d\n", ret);

	attr_class = class_create(THIS_MODULE, "mpu6050");
	if (IS_ERR(attr_class)) {
		ret = PTR_ERR(attr_class);
		pr_err("mpu6050: failed to create sysfs class: %d\n", ret);
		goto err;
	}
	pr_info("mpu6050: sysfs class created\n");

	ret = class_create_file(attr_class, &class_attr_temperature);

	if (ret) {
		pr_err("mpu6050: failed to created sysfs class attribute temperature: %d\n", ret);
		goto err;
	}

	pr_info("mpu6050: sysfs class attributes created\n");

	pr_info("mpu6050: module loaded\n");
	return 0;

	ret = mpu6050_read_data();
	if (!ret) {
		goto err;
	}

	err:
		mpu6050_exit();
		return ret;
}

module_init(mpu6050_init);
module_exit(mpu6050_exit);

