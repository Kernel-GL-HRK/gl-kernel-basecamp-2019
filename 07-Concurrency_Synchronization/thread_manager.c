#include <linux/module.h>
#include <linux/kernel.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Primerov Maxim <primerovmax@gmail.com>");
MODULE_VERSION("0.1");

void __exit thread_manager_exit(void)
{
        pr_info("thread manager: module exited");
}

int __init thread_manager_init(void)
{
        pr_info("thread manager: module loaded");

        return 0;
}

module_init(thread_manager_init);
module_exit(thread_manager_exit);
