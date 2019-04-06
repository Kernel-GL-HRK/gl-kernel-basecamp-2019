#include "proc_info.h"
#include <linux/proc_fs.h>
#include <asm/uaccess.h>
#include <linux/types.h>
#include <linux/kernel.h>

static struct file_operations fops_proc_info = {
	.owner = THIS_MODULE,
	.read = proc_info_read,
};

static struct proc_info proc_info;

int create_proc_info(const char *name, size_t *buffer_size, size_t *capacity)
{
	struct proc_dir_entry *info_proc_file;

	proc_info.buffer_size = buffer_size;
	proc_info.capacity = capacity;
	proc_info.name = name;

	info_proc_file = proc_create(proc_info.name,
	                             S_IFREG | S_IRUGO | S_IWUGO,
	                             NULL, &fops_proc_info);

	if (info_proc_file == NULL) {
		printk("ERROR (ProcInfo): /proc/%s file was not created\n",
		        proc_info.name);
		goto err;
	}
	printk("MESSAGE (ProcInfo): /proc/%s file was created\n",
	        proc_info.name);

	return 0;

	err:
		return -ENOENT;
}

void remove_proc_info(void)
{
	remove_proc_entry(proc_info.name, NULL);
	printk("MESSAGE (ProcInfo): /proc/%s file was removed\n",
	        proc_info.name);
}

static ssize_t proc_info_read(struct file *filp, char *buf, size_t count, loff_t *pos)
{
	ssize_t retval;
	char buf_msg[124];
	sprintf(buf_msg, "%d %d\n", *proc_info.buffer_size, *proc_info.capacity);

	if (*pos >= strlen(buf_msg)) {
		*pos = 0;
		return 0;
	}

	if (count > strlen(buf_msg) - *pos) {
		count = strlen(buf_msg) - *pos;
	}

	if (raw_copy_to_user((void*)buf, buf_msg + *pos, count)) {
		retval = -EFAULT;
		goto err;
	}
	*pos += count;

	return count;

	err:
		return retval;
}