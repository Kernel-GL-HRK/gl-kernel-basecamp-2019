#include <linux/moduleparam.h>		//library
#include <linux/init.h>           // Macros used to mark up functions e.g. __init __exit
#include <linux/module.h>         // Core header for loading LKMs into the kernel
#include <linux/device.h>         // Header to support the kernel Driver Model
#include <linux/kernel.h>         // Contains types, macros, functions for the kernel
#include <linux/fs.h>             // Header for the Linux file system support
#include <linux/uaccess.h>          // Required for the copy to user function
#define  DEVICE_NAME "chtest"    ///< The device will appear at /dev/chtest using this value
#define  CLASS_NAME  "Test_Class"        ///< The device class -- this is a character device driver
#define BSIZE  1024

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Vitaliy Izhyk <vizhyk1@gmail.com");
MODULE_DESCRIPTION("Char_Driver");
MODULE_VERSION("0.1");

static int B_size = 0;			///<BUFFER from parameter
module_param(B_size, int, S_IRUGO);
char *new_buff;				
static int    majorNumber;                  ///< Stores the device number -- determined automatically
static char   message[BSIZE] = {0};           ///< Memory for the string that is passed from param
static short  size_of_message;              ///< Used to remember the size of the string stored
static int    numberOpens = 0;              ///< Counts the number of times the device is opened
static struct class*  Driver_Class  = NULL; ///< The device-driver class struct pointer
static struct device* Driver_Device = NULL; ///< The device-driver device struct pointer

					//Prototypes
static int     dev_open(struct inode *, struct file *);
static int     dev_release(struct inode *, struct file *);
static ssize_t dev_read(struct file *, char *, size_t, loff_t *);
static ssize_t dev_write(struct file *, const char *, size_t, loff_t *);


static struct file_operations fops =
{
	.open = dev_open,
	.read = dev_read,
	.write = dev_write,
	.release = dev_release,
	.owner = THIS_MODULE,
};





static int __init chtest_init(void){
	printk(KERN_INFO "chtest: Initializing the chtest LKM\n");
    if (B_size > 0) {
		new_buff = kzalloc(B_size*sizeof(*new_buff),GFP_KERNEL);

    }
     
   // Try to dynamically allocate a major number for the device -- more difficult but worth it

		majorNumber = register_chrdev(0, DEVICE_NAME, &fops);
    if (majorNumber<0){
		printk(KERN_ALERT "chtest failed to register a major number\n");
		return majorNumber;
    }
		printk(KERN_INFO "chtest: registered correctly with major number %d\n", majorNumber);
 
   // Register the device class
		Driver_Class = class_create(THIS_MODULE, CLASS_NAME);
    if (IS_ERR(Driver_Class)){                // Check for error and clean up if there is
		unregister_chrdev(majorNumber, DEVICE_NAME);
		printk(KERN_ALERT "Failed to register device class\n");
		return PTR_ERR(Driver_Class);          // Correct way to return an error on a pointer
   }
		printk(KERN_INFO "chtest: device class registered correctly\n");
 
   // Register the device driver
		Driver_Device = device_create(Driver_Class, NULL, MKDEV(majorNumber, 0), NULL, DEVICE_NAME);
    if (IS_ERR(Driver_Device)){               // Clean up if there is an error
		class_destroy(Driver_Class);           // Repeated code but the alternative is goto statements
		unregister_chrdev(majorNumber, DEVICE_NAME);
		printk(KERN_ALERT "chtest Failed to create the device\n");
		return PTR_ERR(Driver_Device);
    }
		printk(KERN_INFO "chtest: device class created correctly\n"); // Made it! device was initialized

		return 0;

}

static void __exit chtest_exit(void)
{
    if(B_size == 0){
    kfree(new_buff);
    }
		proc_cleanup();				    //proc_cleanup
		sys_cleanup();       	 //kobject_put(chtest_kobject);			    //sys_cleanup
		device_destroy(Driver_Class, MKDEV(majorNumber, 0));     // remove the device
		class_unregister(Driver_Class);                          // unregister the device class
		class_destroy(Driver_Class);                             // remove the device class
		unregister_chrdev(majorNumber, DEVICE_NAME);             // unregister the major number

		printk(KERN_INFO "chtest: Goodbye from the LKM!\n");
}

static int dev_open(struct inode *inodep, struct file *filep)
{
		numberOpens++;
		printk(KERN_INFO "chtest: Device has been opened %d time(s)\n", numberOpens);
		return 0;
}
 


static ssize_t dev_read(struct file *filep, char *buffer, size_t len, loff_t *offset)
{
		int error_count = 0;
   // copy_to_user has the format ( * to, *from, size) and returns 0 on success
    if (B_size == 0){
error_count = copy_to_user(buffer, message, size_of_message);
    }
    else 
	error_count = copy_to_user(buffer, new_buff, size_of_message);
 
    if (error_count==0){            // if true then have success
		printk(KERN_INFO "chtest: Sent %d characters to the user\n", size_of_message);
		return (size_of_message=0);  // clear the position to the start and return 0
    }
    else {
		printk(KERN_INFO "chtest: Failed to send %d characters to the user\n", error_count);
		return -EFAULT;              // Failed -- return a bad address message (i.e. -14)
    }
}
 

static ssize_t dev_write(struct file *filep, const char *buffer, size_t len, loff_t *offset)
{
    if(B_size == 0) {
		copy_from_user(message, buffer, len);   // appending received string with its length
		size_of_message = strlen(message); 
    }                // store the length of the stored message
    else {
		copy_from_user(new_buff, buffer, len);
		size_of_message = strlen(new_buff); 
    } 


		printk(KERN_INFO "chtest: Received %zu characters from the user\n", len);
		return len;
}






static int dev_release(struct inode *inodep, struct file *filep)
{
		printk(KERN_INFO "chtest: Device successfully closed\n");
		return 0;
}

module_init(chtest_init);
module_exit(chtest_exit);


