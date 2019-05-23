#include <linux/module.h>


static int __init thread_module_init(void){
    return 0;
}

static void __exit thread_module_exit(void){
}


MODULE_LICENSE("GPL");
MODULE_AUTHOR("Eduard.Voronkin<eduard.voronkin@nure.ua");
MODULE_DESCRIPTION("Thread module");
MODULE_VERSION("0.1");
module_init(thread_module_init);
module_exit(thread_module_exit);

