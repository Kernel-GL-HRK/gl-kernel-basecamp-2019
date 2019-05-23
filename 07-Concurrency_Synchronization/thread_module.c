#include <linux/module.h>
#include <linux/kobject.h>
#include <linux/sysfs.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/kthread.h>
#include <linux/mutex.h>

#define SYS_DIR_NAME "thread_module"
#define SYS_FILE_NAME command

#define NUMBER_OF_THREADS 5

struct task_for_master{
    struct task_struct *mst_thread;
    char symbol;
    long block_size;
    long blocks_amount;
} last_task;
///////////////////////////
struct task_for_slave{
    struct task_struct *master_thread;
    char *block_p;
    int data_size;
    int blocks_count;
    struct mutex main_mutex;
    int thr_id;
};
/////THREAD POOL
static struct completion finished[NUMBER_OF_THREADS];

static int slave_fun(void *data){
    mutex_lock_interruptible(&((struct task_for_slave*)data)->main_mutex);
    int thr_id = ((struct task_for_slave*)data)->thr_id;
    ((struct task_for_slave*)data)->thr_id -= 1;
    mutex_unlock(&((struct task_for_slave*)data)->main_mutex);
    printk(KERN_INFO "Slave number %d started\n", thr_id);
    int blc_count = ((struct task_for_slave*)data)->blocks_count;
    while(blc_count > 0){
        printk(KERN_INFO, ((struct task_for_slave*)data)->block_p);
        blc_count -= 1;
    }
    mutex_lock_interruptible(&((struct task_for_slave*)data)->main_mutex);
    complete(finished + thr_id);
    mutex_unlock(&((struct task_for_slave*)data)->main_mutex);
    printk(KERN_INFO, "Slave %d finished\n", ((struct task_for_slave*)data)->thr_id);
    return 0;
}

static int master_fun(void *data){
    printk(KERN_INFO "Master is running\n");
    int i;
    struct task_struct *slave_thread_pool[NUMBER_OF_THREADS];

    struct task_for_slave task;
    task.data_size = last_task.block_size;
    task.block_p = kmalloc(task.data_size + 1, GFP_KERNEL);
    memset(task.block_p, last_task.symbol, task.data_size);
    task.block_p[task.data_size - 1] = '\n';
    task.blocks_count = last_task.blocks_amount;
    task.master_thread = last_task.mst_thread;
    task.thr_id = NUMBER_OF_THREADS - 1;

    mutex_init(&(task.main_mutex));

    for(i = 0; i < NUMBER_OF_THREADS; ++i)
        init_completion(finished + i);

    for(i = 0; i < NUMBER_OF_THREADS; ++i)
        slave_thread_pool[i] = kthread_run(slave_fun, &task, "salve_thr#%d", i + 1);

    printk(KERN_INFO "STARTED WAITING\n");
    for(i = 0; i < NUMBER_OF_THREADS; ++i)
        wait_for_completion(finished + i);
    fkree(task.block_p);
    printk(KERN_ERR, '\n');
    printk(KERN_INFO "Job done\n");
    return 0;
}

ssize_t store_task(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count){
    if(count != 8){
        printk(KERN_ERR "ERROR::Wrong command size string\n"
        "Command should look like: SYMBOL NUMBER NUMBER, where number is two symbols\n");
        return -1;
    }
    last_task.symbol = buf[0];
    char *temp = kmalloc(8, GFP_KERNEL);
    memcpy(temp, buf, 8);
    temp[4] = temp[7] = '\0';
    kstrtol((temp + 2), 10, &(last_task.block_size));
    kstrtol((temp + 5), 10, &(last_task.blocks_amount));
    kfree(temp);
    if((last_task.mst_thread = kthread_run(master_fun, NULL, "Master thread")))
        printk(KERN_INFO "Master thread created succesfully\n");
    else
        printk(KERN_ERR "Thread creation failed\n");
    return count;
}
ssize_t show_task(struct kobject *kobj, struct kobj_attribute *attr, char *buf){
    return sprintf(buf, "SYMBOL: %c, BLOCK SIZE: %li, BLOCKS AMOUNT: %li\n",
     last_task.symbol, last_task.block_size, last_task.blocks_amount);
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

