#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/sched.h>
#include <asm/uaccess.h>
#include <linux/slab.h>
#include <linux/moduleparam.h>
#include <linux/device.h>
#include <linux/module.h>

#ifndef MODULE_NAME
#define MODULE_NAME 	"ChD"
#endif

#define BUFFER_SIZE 		1024 //обьем буфера
#define DEVICE_NAME 	"charDevice"
#define CLASS_NAME		"chDClass"

int buffer_increase = 0;

module_param(buffer_increase, int, 0220);

static int buffer_size = BUFFER_SIZE;
static char *data_buff;

//===Buffer operations===

static int buffer_create(void)
{
    data_buff = kmalloc(buffer_size+buffer_increase, GFP_KERNEL);

    if (NULL == data_buff) 
        return -ENOMEM;

    return 0;
}

static void buffer_clean(void)
{
    if (data_buff) {
        kfree(data_buff);
        data_buff = NULL;
    }
}
