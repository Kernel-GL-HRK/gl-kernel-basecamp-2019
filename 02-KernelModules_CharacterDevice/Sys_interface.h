#pragma once
#include <linux/kobject.h> // for kobj and kobj_create
#include <linux/sysfs.h> // for sysfs_create_file
#include "Create_device.h"

#define SYS_DIR_NAME    "My_sys_operations"
#define SYS_FILE_NAME   Clear


ssize_t store_clear(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count){
    if(buf[0] != '0'){
        memset(driver_buffer, 0, data_size);
        data_size = 0;
        printk(KERN_DEBUG "Successfully cleared buffer\n");
    }
    return count;
}
static struct  kobject *kobj_dir;
static struct kobj_attribute file_attr = __ATTR(SYS_FILE_NAME, S_IWUSR, NULL, store_clear);

void init_sys(void){
    kobj_dir = kobject_create_and_add(SYS_DIR_NAME, NULL);
    sysfs_create_file(kobj_dir, &file_attr.attr);
}
