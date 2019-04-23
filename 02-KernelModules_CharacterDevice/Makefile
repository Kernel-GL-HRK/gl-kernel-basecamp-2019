obj-m += chrdev.o

BUILD_KERNEL=/lib/modules/$(shell uname -r)/build

all:
	make -C $(BUILD_KERNEL) M=$(PWD) modules

clean:
	make -C $(BUILD_KERNEL) M=$(PWD) clean


