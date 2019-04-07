#include <linux/module.h>
#include <linux/moduleparam.h>  
#include <linux/init.h>
#include <linux/kernel.h>       
#include <linux/slab.h>		
#include <linux/fs.h>           
#include <linux/types.h>        
#include <linux/cdev.h>
#include <asm/uaccess.h> 
#include <linux/device.h>
#include <linux/err.h>

#define CLASS_NAME	"chrdev"
#define DEVICE_NAME	"chrdev_example"


static int BUFFER_SIZE=1024;
static int major = 0; 
static int minor = 0; 
static int count = 1; 
module_param(major, int, S_IRUGO);
module_param(minor, int, S_IRUGO);
module_param(count, int, S_IRUGO);
module_param(BUFFER_SIZE, int, S_IRUGO);
 
static struct class *pclass;
static struct device *pdev;

static int data_size;
static unsigned char *data_buffer;

static int is_open;

static int dev_open(struct inode *inodep, struct file *filep)
{
	if (is_open) {
		pr_err("chrdev: already open\n");
		return -EBUSY;
	}

	is_open = 1;
	pr_info("chrdev: device opened\n");
	return 0;
}

static int dev_release(struct inode *inodep, struct file *filep)
{
	is_open = 0;
	pr_info("chrdev: device closed\n");
	return 0;
}
