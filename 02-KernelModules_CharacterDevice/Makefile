#
# HelloWorld Linux module
#$(BUILD_KERNEL)
BUILD_KERNEL=/home/daniel/Workspace/GL_Linux_Kernel/tmp/kernel-build

obj-m := my_chardev.o

all:
	$(MAKE) -C $(BUILD_KERNEL) M=$(CURDIR) modules

clean:
	$(MAKE) -C  $(BUILD_KERNEL) M=$(CURDIR) clean
