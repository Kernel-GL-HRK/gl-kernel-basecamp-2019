#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/err.h>
#include <linux/uaccess.h>
#include <linux/kdev_t.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/stat.h>
#include <linux/proc_fs.h>
#include <linux/proc_fs.h>
#include <linux/sched.h>
#include <asm/uaccess.h>
#include <linux/slab.h>

/********************************* Global varables and definitions ********************************/

#define CLASS_NAME  "my_chardev_class"
#define DEVICE_NAME "my_chardev"
#define DEFAULT_SIZE_BUFF 1024
#define DEVICE_OPEN 	  1
#define DEVICE_CLOSE	  0


static int is_device_open;
static int major;

static struct class *pclass;
static struct device *pdev;

static int buffer_size;

static struct data_info{
	int   crnt_size_buff;
	int   size_buff;
	char* data_buff;
} dev_data_file;

static struct proc_info{
	int  data_length;
	char data_buff[10];
} proc_data_file;

static struct device_attribute my_dev_attr;

static struct proc_dir_entry *proc_file;

/**************************************************************************************************/




MODULE_LICENSE("GPL");
MODULE_AUTHOR("Daniel.Shami <danismi715@gmail");
MODULE_DESCRIPTION("Character device driver");                                                                                   
MODULE_VERSION("0.1");