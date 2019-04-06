#ifndef DEV_H
#define DEV_H

#include <linux/fs.h>

int create_cdevice(int major, int minor, int count);
void remove_cdevice(void);

int cdev_open(struct inode *, struct file *);
int cdev_release(struct inode *, struct file *);

#endif
