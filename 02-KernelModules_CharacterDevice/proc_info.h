#ifndef PROC_INFO_H
#define PROC_INFO_H

#include <linux/types.h>
#include <linux/proc_fs.h>

struct proc_info {
	size_t *capacity;
	size_t *buffer_size;
	const char *name;
};

int create_proc_info(const char *name, size_t *buffer_size, size_t *capacity);
void remove_proc_info(void);
static ssize_t proc_info_read(struct file *filp, char *buf, size_t count, loff_t *pos);

#endif