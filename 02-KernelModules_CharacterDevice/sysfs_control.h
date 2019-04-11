#ifndef SYSFS_CONTROL_H
#define SYSFS_CONTROL_H

int create_sysfs_control(const char *name, void (*clear) (void), dev_t devt);
void remove_sysfs_control(void);
static ssize_t sysfs_control_store(struct device *dev,
								   struct device_attribute *attr, 
								   const char *buf, size_t count);

#endif

