#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Maxim Primerov <primerovmax@gmail.com>");
MODULE_DESCRIPTION("Driver for temperature sensor on the mpu6050"
	               "device");
MODULE_VERSION("0.1");

#define REG_WHO_AM_I 0x75
#define REG_PWR_MGMT_1 0x6B

#define MPU6050_WHO_AM_I 0x68
#define MPU6050_DEVICE_RESET 0x00

struct mpu6050_data {
	struct i2c_client *client;
	int temperature;
};

static struct mpu6050_data device_data;

static int mpu6050_probe(struct i2c_client *client,
					     const struct i2c_device_id *id)
{
	int ret;

	dev_info(&client->dev, 
		     "mpu6050: i2c client address is 0x%X\n", client->addr);

	ret = i2c_smbus_read_byte_data(client, REG_WHO_AM_I);
	if (IS_ERR_VALUE(ret)) {
		dev_err(&client->dev,
			    "mpu6050: i2c_smbus_read_byte_data()"
			    "failed with error: %d\n", ret);
		goto err;
	}

	if (ret != MPU6050_WHO_AM_I) {
		dev_err(&client->dev,
			    "mpu6050: wrong i2c device found: expected 0x%X, "
			    "found 0x%X\n", MPU6050_WHO_AM_I, ret);
		ret = -1;
		goto err;
	}

	dev_info(&client->dev, "mpu6050: i2c mpu6050 device found, "
		     "WHO_AM_I register value = 0x%X\n", ret);

	i2c_smbus_write_byte_data(client, REG_PWR_MGMT_1,
	                          MPU6050_DEVICE_RESET);

	device_data.client = client; 

	dev_info(&client->dev, "mpu6050: i2c driver probed\n");

	return 0;

	err:
		return ret;
}

static int mpu6050_remove(struct i2c_client *client)
{
	device_data.client = 0;

	dev_info(&client->dev, "mpu6050: i2c driver removed\n");

	return 0;
}

static const struct i2c_device_id mpu6050_idtable[] = {
	{ "mpu6050", 0 },
	{ }
};

static struct i2c_driver mpu6050_i2c_driver = {
	.driver = {
		.name = "con_mpu6050",
	},
	.probe = mpu6050_probe,
	.remove = mpu6050_remove,
	.id_table = mpu6050_idtable,
};

static void mpu6050_exit(void)
{
	i2c_del_driver(&mpu6050_i2c_driver);
	pr_info("mpu6050: i2c driver deleted");

	pr_info("mpu6050: module exited\n");
}

static int mpu6050_init(void)
{
	int ret;

	ret = i2c_add_driver(&mpu6050_i2c_driver);
	if (ret) {
		pr_err("mpu6050: failed to add new i2c driver: %d\n", ret);
		goto err;
	}
	pr_info("mpu6050: i2c driver created\n");

	pr_info("mpu6050: module loaded\n");
	return 0;

	err:
		mpu6050_exit();
		return ret;
}

module_init(mpu6050_init);
module_exit(mpu6050_exit);
