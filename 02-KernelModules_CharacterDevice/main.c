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
