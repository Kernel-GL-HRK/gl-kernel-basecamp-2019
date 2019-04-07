#include "Create_device.h"
#include <linux/proc_fs.h>

#define DIR_NAME    "My_character_dirver"
#define FILE_NAME   "Info"


ssize_t proc_read_info (struct file *file_p, char __user *user_buf, size_t len, loff_t *offset){
    char answer[100];
    if(*offset != 0)
            return 0;

    int answ_length = sprintf(answer, "Size of written data: %d\n"
                        "Size of buffer: %d\n", data_size, BUF_SIZE );
    *offset += answ_length;
    if(copy_to_user(user_buf, answer, answ_length)){
        return -EFAULT;
    
    }
    else
        return answ_length;
}


static struct proc_dir_entry *proc_dir; //direcotry
static struct proc_dir_entry *proc_info_file; //file with info

static struct file_operations proc_ops = {
    .owner = THIS_MODULE,
    .read = proc_read_info
};

static void init_proc(void)
{
    proc_dir = proc_mkdir(DIR_NAME, NULL);
    proc_info_file = proc_create(FILE_NAME, S_IRUGO, proc_dir, &proc_ops);
}

