#ifndef CHRDEV_DRIVER_H
#define CHRDEV_DRIVER_H
#include <linux/init.h>

int chrdev_init(void);

void chrdev_exit(void);

ssize_t chrdev_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos);

ssize_t chrdev_write(struct file *filp, const char __user *buf, size_t count, loff_t *f_pos);

int chrdev_open(struct inode *inode, struct file *filp);

int chrdev_release(struct inode *inode, struct file *filp);

#endif

