#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <linux/gpio.h>
#include <linux/interrupt.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Maxim Primerov <primerovmax@gmail.com>");
MODULE_DESCRIPTION("Driver for temperature sensor on the mpu6050 device");
MODULE_VERSION("0.1");

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
#define MOT_THR                 0x1F  
#define MOT_DUR                 0x20  
#define MOT_DETECT_CTRL         0x69

#define ONE_TEMP_POINT          1000

#define MPU6050_WHO_AM_I        0x68
#define MPU6050_DEVICE_RESET    0x00

#define GPIO_INT 6

static unsigned int irq_number;

struct mpu6050_data {
	struct i2c_client *client;
	int temperature;
        int accel[3];
        int gyro[3];
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

        i2c_smbus_write_byte_data(client, REG_CONFIG, 0);
	i2c_smbus_write_byte_data(client, REG_GYRO_CONFIG, 0);
	i2c_smbus_write_byte_data(client, REG_INT_PIN_CFG, 0x20);
	i2c_smbus_write_byte_data(client, REG_ACCEL_CONFIG, 0x01);
	i2c_smbus_write_byte_data(client, MOT_THR, 0x14);
	i2c_smbus_write_byte_data(client, MOT_DUR, 0x01);
	i2c_smbus_write_byte_data(client, MOT_DETECT_CTRL, 0x15);
	i2c_smbus_write_byte_data(client, REG_INT_ENABLE, 0x40);
	i2c_smbus_write_byte_data(client, REG_FIFO_EN, 0);
	i2c_smbus_write_byte_data(client, REG_USER_CTRL, 0);
	i2c_smbus_write_byte_data(client, REG_PWR_MGMT_1, 0);
	i2c_smbus_write_byte_data(client, REG_PWR_MGMT_2, 0);

	device_data.client = client; 

	pr_info("mpu6050: i2c driver probed\n");

	return 0;

	err:
		return ret;
}

static int mpu6050_remove(struct i2c_client *client)
{
        i2c_smbus_write_byte_data(client, REG_INT_PIN_CFG, 0);
	i2c_smbus_write_byte_data(client, REG_ACCEL_CONFIG, 0);
	i2c_smbus_write_byte_data(client, MOT_THR, 0);
	i2c_smbus_write_byte_data(client, MOT_DUR, 0);
	i2c_smbus_write_byte_data(client, MOT_DETECT_CTRL, 0);
	i2c_smbus_write_byte_data(client, REG_INT_ENABLE, 0);

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

        device_data.accel[0] = (s16)((u16)i2c_smbus_read_word_swapped(device_data.client, REG_ACCEL_XOUT_H));
        device_data.accel[1] = (s16)((u16)i2c_smbus_read_word_swapped(device_data.client, REG_ACCEL_YOUT_H));
	device_data.accel[2] = (s16)((u16)i2c_smbus_read_word_swapped(device_data.client, REG_ACCEL_ZOUT_H));

        device_data.gyro[0] = (s16)((u16)i2c_smbus_read_word_swapped(device_data.client, REG_GYRO_XOUT_H));
	device_data.gyro[1] = (s16)((u16)i2c_smbus_read_word_swapped(device_data.client, REG_GYRO_YOUT_H));
	device_data.gyro[2] = (s16)((u16)i2c_smbus_read_word_swapped(device_data.client, REG_GYRO_ZOUT_H));

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

static ssize_t accel_x_show(struct class *class, struct class_attribute *attr, char *buf)
{
	sprintf(buf, "%d\n", device_data.accel[0]);
	return strlen(buf);
}

static ssize_t accel_y_show(struct class *class, struct class_attribute *attr, char *buf)
{
	sprintf(buf, "%d\n", device_data.accel[1]);
	return strlen(buf);
}

static ssize_t accel_z_show(struct class *class, struct class_attribute *attr, char *buf)
{
	sprintf(buf, "%d\n", device_data.accel[2]);
	return strlen(buf);
}

static ssize_t gyro_x_show(struct class *class, struct class_attribute *attr, char *buf)
{
	sprintf(buf, "%d\n", device_data.gyro[0]);
	return strlen(buf);
}

static ssize_t gyro_y_show(struct class *class, struct class_attribute *attr, char *buf)
{
	sprintf(buf, "%d\n", device_data.gyro[1]);
	return strlen(buf);
}

static ssize_t gyro_z_show(struct class *class, struct class_attribute *attr, char *buf)
{
	sprintf(buf, "%d\n", device_data.gyro[2]);
	return strlen(buf);
}

CLASS_ATTR_RO(temperature);
CLASS_ATTR_RO(accel_x);
CLASS_ATTR_RO(accel_y);
CLASS_ATTR_RO(accel_z);
CLASS_ATTR_RO(gyro_x);
CLASS_ATTR_RO(gyro_y);
CLASS_ATTR_RO(gyro_z);

static struct class *attr_class;

static irq_handler_t move_int(unsigned int irq, void *dev_id, struct pt_regs *regs)
{      
        mpu6050_read_data();
        return (irq_handler_t)IRQ_HANDLED;
}

static void mpu6050_exit(void)
{
	if (attr_class) {
		class_remove_file(attr_class, &class_attr_temperature);
		class_remove_file(attr_class, &class_attr_accel_x);
		class_remove_file(attr_class, &class_attr_accel_y);
		class_remove_file(attr_class, &class_attr_accel_z);
		class_remove_file(attr_class, &class_attr_gyro_x);
		class_remove_file(attr_class, &class_attr_gyro_y);
		class_remove_file(attr_class, &class_attr_gyro_z);
		pr_info("mpu6050: sysfs class attributes removed\n");

		class_destroy(attr_class);
		pr_info("mpu6050: sysfs class destroyed\n");
	}

        free_irq(irq_number, NULL);
        gpio_free(GPIO_INT);

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

        gpio_request(GPIO_INT, "GPIO_INT");
        gpio_direction_input(GPIO_INT);
        gpio_set_debounce(GPIO_INT, 50);
        gpio_export(GPIO_INT, false);

        irq_number = gpio_to_irq(GPIO_INT);

        ret = request_irq(irq_number, (irq_handler_t)move_int, 
                          IRQF_TRIGGER_RISING, "mpu6050", NULL);

        if (ret) {
                pr_err("mpu6050: failed to request irq\n");
                goto err;
        }

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

	ret = class_create_file(attr_class, &class_attr_accel_x);
	if (ret) {
		pr_err("mpu6050: failed to created sysfs class attribute accel_x: %d\n", ret);
		goto err;
	}

	ret = class_create_file(attr_class, &class_attr_accel_y);
	if (ret) {
		pr_err("mpu6050: failed to created sysfs class attribute accel_y: %d\n", ret);
		goto err;
	}

	ret = class_create_file(attr_class, &class_attr_accel_z);
	if (ret) {
		pr_err("mpu6050: failed to created sysfs class attribute accel_z: %d\n", ret);
		goto err;
	}

	ret = class_create_file(attr_class, &class_attr_gyro_x);
	if (ret) {
		pr_err("mpu6050: failed to created sysfs class attribute gyro_x: %d\n", ret);
		goto err;
	}

	ret = class_create_file(attr_class, &class_attr_gyro_y);
	if (ret) {
		pr_err("mpu6050: failed to created sysfs class attribute gyro_y: %d\n", ret);
		goto err;
	}

	ret = class_create_file(attr_class, &class_attr_gyro_z);
	if (ret) {
		pr_err("mpu6050: failed to created sysfs class attribute gyro_z: %d\n", ret);
		goto err;
	}

	pr_info("mpu6050: sysfs class attributes created\n");

	pr_info("mpu6050: module loaded\n");
	return 0;

	err:
		mpu6050_exit();
		return ret;
}

module_init(mpu6050_init);
module_exit(mpu6050_exit);

