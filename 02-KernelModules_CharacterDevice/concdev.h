#ifndef CONCDEV_H
#define CONCDEV_H

#include <linux/fs.h>

int create_cdevice(int major, int minor, int count, size_t size);
void remove_cdevice(void);

int cdev_open(struct inode *, struct file *);
int cdev_release(struct inode *, struct file *);

ssize_t cdev_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos);
ssize_t cdev_write(struct file *filp, const char __user *buf, size_t count, loff_t *f_pos);

#endif
