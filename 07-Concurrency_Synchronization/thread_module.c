#include <linux/module.h>
#include <linux/kobject.h>
#include <linux/sysfs.h>
#include <linux/kernel.h>
#include <linux/slab.h>

#define SYS_DIR_NAME "thread_module"
#define SYS_FILE_NAME command

struct task_struct{
    char symbol;
    long block_size;
    long blocks_amount;
} task;


ssize_t store_task(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count){
    if(count != 8){
        printk(KERN_ERR "ERROR::Wrong command size string\n"
        "Command should look like: SYMBOL NUMBER NUMBER, where number is two symbols\n");
        return -1;
    }
    task.symbol = buf[0];
    char *temp = kmalloc(8, GFP_KERNEL);
    memcpy(temp, buf, 8);
    temp[4] = temp[7] = '\0';
    kstrtol((temp + 2), 10, &(task.block_size));
    kstrtol((temp + 5), 10, &(task.blocks_amount));
    kfree(temp);
    return count;
}
ssize_t show_task(struct kobject *kobj, struct kobj_attribute *attr, char *buf){
    return sprintf(buf, "SYMBOL: %c, BLOCK SIZE: %li, BLOCKS AMOUNT: %li\n",
     task.symbol, task.block_size, task.blocks_amount);
}



static struct kobject *kobj_dir;
static struct kobj_attribute file_attr = __ATTR(SYS_FILE_NAME, (S_IWUSR | S_IRUGO), show_task, store_task);


int init_sys(void){
    kobj_dir = kobject_create_and_add(SYS_DIR_NAME, NULL);
    if(IS_ERR(kobj_dir))
        return -1;
    sysfs_create_file(kobj_dir, &file_attr.attr);
    return 0;
}

static int __init thread_module_init(void){
    if(-1 == init_sys()){
    printk(KERN_ERR "ERROR::Cannot init sys entry\n");
    return -1;
    }
    return 0;
}

static void __exit thread_module_exit(void){
    kobject_put(kobj_dir);
}


MODULE_LICENSE("GPL");
MODULE_AUTHOR("Eduard.Voronkin<eduard.voronkin@nure.ua");
MODULE_DESCRIPTION("Thread module");
MODULE_VERSION("0.1");
module_init(thread_module_init);
module_exit(thread_module_exit);

