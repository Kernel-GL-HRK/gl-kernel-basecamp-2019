#include "sysfs_control.h"
#include <linux/device.h>
#include <linux/kernel.h>

#define FILE_NAME "clear"

static struct device *char_device;
static struct kobject ko;
static struct device_attribute attr_chard;
static void (*clear_function) (void);

static struct class *char_dev_class;
static struct device *char_class_device;
static dev_t devt_buf;

int create_sysfs_control(const char *name, void (*clear) (void), dev_t devt)
{
	int res;
	char_device = root_device_register(name);
	ko = char_device->kobj;

	attr_chard.attr.name = FILE_NAME;
	attr_chard.attr.mode = 0222;
	attr_chard.show = NULL;
	attr_chard.store = sysfs_control_store;
	clear_function = clear;
	devt_buf = devt;

	res = sysfs_create_file(&ko, &attr_chard.attr);

	if (res < 0) {
		goto err;
	}

	printk("MESSAGE (SysFsControl): /sys/devices/%s/%s file was created\n",
		    name, FILE_NAME);

	char_dev_class = class_create(NULL, name);
	printk("MESSAGE (SysFsControl): class was created\n");

	char_class_device = device_create(char_dev_class, NULL, devt_buf, NULL, name);
	printk("Message (SysFsControl): device was created\n");

	return 0;

	err:
		remove_sysfs_control();
		return res;
}

void remove_sysfs_control()
{
	device_destroy(char_dev_class, devt_buf);
	class_destroy(char_dev_class);

	sysfs_remove_file(&ko, &attr_chard.attr);
	root_device_unregister(char_device);

	printk("MESSAME (SysFsControl): sysfs control removed\n");
}

static ssize_t sysfs_control_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
	int buffer;

	sscanf(buf, "%d", &buffer);

	if (buffer) {
		clear_function();
		printk("MESSAGE (SysFsControl): buffer has been cleared\n");
	} else {
		printk("MESSAGE (SysFsControl): buffer cannot be cleared\n");
	}

	return count;
}

