#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/err.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <linux/kobject.h>
#include <linux/sysfs.h>
#include <linux/interrupt.h>
#include <linux/gpio.h>

#include "mpu6050-regs.h"

#define PRECISION           1000
#define MPU_INT_PIN_NUMBER  10

struct mpu6050_data {
	struct i2c_client *drv_client;
	int accel_values[3];
	int gyro_values[3];
	int temperature[2];
};

static struct mpu6050_data g_mpu6050_data;


static int mpu6050_read_temp(void)
{
	int temp_raw;
	struct i2c_client *drv_client = g_mpu6050_data.drv_client;

	if (drv_client == 0)
		return -ENODEV;
	/* Temperature in degrees C =
	 * (TEMP_OUT Register Value  as a signed quantity)/340 + 36.53
	 */
	temp_raw = (s16)((u16)i2c_smbus_read_word_swapped(drv_client, REG_TEMP_OUT_H));
	int temp_full = (temp_raw * PRECISION / 340) + 36530; //36.53 = 36530
	g_mpu6050_data.temperature[0] =  temp_full / PRECISION;
	g_mpu6050_data.temperature[1] =  temp_full % PRECISION;
	int t_faren_full = temp_full * 9 / 5 + 32 * PRECISION; // F = C * 9 / 5 + 32
	dev_info(&drv_client->dev, "TEMP[C,F] = [%d.%03d, %d.%03d]\n",
		g_mpu6050_data.temperature[0], g_mpu6050_data.temperature[1],
		t_faren_full / PRECISION, t_faren_full % PRECISION);
	return 0;
}


static int mpu6050_read_acc_and_gyro(void)
{
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
	
	dev_info(&drv_client->dev, "sensor data read:\n");
	dev_info(&drv_client->dev, "ACCEL[X,Y,Z] = [%d, %d, %d]\n",
		g_mpu6050_data.accel_values[0],
		g_mpu6050_data.accel_values[1],
		g_mpu6050_data.accel_values[2]);
	dev_info(&drv_client->dev, "GYRO[X,Y,Z] = [%d, %d, %d]\n",
		g_mpu6050_data.gyro_values[0],
		g_mpu6050_data.gyro_values[1],
		g_mpu6050_data.gyro_values[2]);
	int int_status = (s8)((u8)i2c_smbus_read_byte_data(drv_client, REG_INT_STATUS));
	dev_info(&drv_client->dev, "INT STATUS %x\n", int_status);
	return 0;
}



static int mpu6050_probe(struct i2c_client *drv_client, const struct i2c_device_id *id)
{
	int ret;
	printk(KERN_DEBUG "PROBE" );
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
	i2c_smbus_write_byte_data(drv_client, REG_PWR_MGMT_1, 0x00);
	i2c_smbus_write_byte_data(drv_client, REG_SIG_PATH_RESET, 0x07);
	i2c_smbus_write_byte_data(drv_client, REG_INT_PIN_CFG, 0x20);
	i2c_smbus_write_byte_data(drv_client, REG_ACCEL_CONFIG, 0x01);
	i2c_smbus_write_byte_data(drv_client, REG_MOT_THR, 20);
	i2c_smbus_write_byte_data(drv_client, REG_MOT_DUR, 40); //40 ms
	i2c_smbus_write_byte_data(drv_client, REG_MOT_DETECT_CTRL, 0x15);
	i2c_smbus_write_byte_data(drv_client, REG_INT_ENABLE, 0x40);
	
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
	sprintf(buf, "%d\n", g_mpu6050_data.accel_values[0]);
	return strlen(buf);
}

static ssize_t accel_y_show(struct class *class,
			    struct class_attribute *attr, char *buf)
{
	sprintf(buf, "%d\n", g_mpu6050_data.accel_values[1]);
	return strlen(buf);
}

static ssize_t accel_z_show(struct class *class,
			    struct class_attribute *attr, char *buf)
{
	sprintf(buf, "%d\n", g_mpu6050_data.accel_values[2]);
	return strlen(buf);
}

static ssize_t gyro_x_show(struct class *class,
			   struct class_attribute *attr, char *buf)
{
	sprintf(buf, "%d\n", g_mpu6050_data.gyro_values[0]);
	return strlen(buf);
}

static ssize_t gyro_y_show(struct class *class,
			   struct class_attribute *attr, char *buf)
{
	sprintf(buf, "%d\n", g_mpu6050_data.gyro_values[1]);
	return strlen(buf);
}

static ssize_t gyro_z_show(struct class *class,
			   struct class_attribute *attr, char *buf){

	sprintf(buf, "%d\n", g_mpu6050_data.gyro_values[2]);
	return strlen(buf);
}

static ssize_t temp_c_show(struct class *class,
			 struct class_attribute *attr, char *buf)
{
	mpu6050_read_temp();
	sprintf(buf, "%d.%03d\n", g_mpu6050_data.temperature[0], g_mpu6050_data.temperature[1]);
	return strlen(buf);
}
static ssize_t temp_f_show(struct class *class,
			 struct class_attribute *attr, char *buf)
{
	mpu6050_read_temp();
	int t_faren_full = g_mpu6050_data.temperature[0] * PRECISION + g_mpu6050_data.temperature[1];
	t_faren_full = t_faren_full * 9 / 5 + 32 * PRECISION;
	sprintf(buf, "%d.%03d\n", t_faren_full / PRECISION, t_faren_full % PRECISION);
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

static irqreturn_t mpu_int_main(int irq, void *dev_id)
{
	disable_irq_nosync(irq);
	printk(KERN_INFO "INTERRUPT ARRIEVED\n");
	return IRQ_WAKE_THREAD;
}
static irqreturn_t mpu_int_thr_fn(int irq, void *dev_id)
{
	mpu6050_read_acc_and_gyro();
	enable_irq(irq);
	printk(KERN_INFO "INTERRUPT HANDELED\n");
	return IRQ_HANDLED;
}

//Pointer to directory in /sys/class
static struct class * class_dir;

static int irqNumber;

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
	ret = gpio_request(MPU_INT_PIN_NUMBER, "MPU INT");
	if(ret){
		printk(KERN_ERR "Cannot request gpio\n");
		return ret;
	}
	ret = gpio_direction_input(MPU_INT_PIN_NUMBER);
	if(ret < 0){
		printk(KERN_ERR "Cannot set input mode on gpio\n");
		return ret;
	}
	
	irqNumber = gpio_to_irq(MPU_INT_PIN_NUMBER);
	ret = request_threaded_irq(irqNumber, mpu_int_main, mpu_int_thr_fn, 
	 IRQF_TRIGGER_RISING, "MPU_INT", NULL);
	if(ret){
		 printk(KERN_ERR "Cannot request interrupt\n");
		 return ret;
	 }

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
	ret = class_create_file(class_dir, &class_attr_temp_c);
	if (ret) {
		pr_err("mpu6050: failed to create sysfs class attribute temp_c: %d\n", ret);
		return ret;
	}
	ret = class_create_file(class_dir, &class_attr_temp_f);
	if (ret) {
		pr_err("mpu6050: failed to create sysfs class attribute temp_f: %d\n", ret);
		return ret;
	}

	pr_info("mpu6050: sysfs class attributes created\n");
	
	pr_info("mpu6050: module loaded\n");
	return 0;
}

static void mpu6050_exit(void)
{
	if (class_dir) {
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
	free_irq(irqNumber, NULL);
	gpio_free(MPU_INT_PIN_NUMBER);
	i2c_del_driver(&mpu6050_i2c_driver);
	pr_info("mpu6050: i2c driver deleted\n");

	pr_info("mpu6050: module exited\n");
}

module_init(mpu6050_init);
module_exit(mpu6050_exit);

MODULE_AUTHOR("Andriy.Khulap <andriy.khulap@globallogic.com>");
MODULE_DESCRIPTION("mpu6050 I2C acc&gyro");
MODULE_LICENSE("GPL");
MODULE_VERSION("0.1");

