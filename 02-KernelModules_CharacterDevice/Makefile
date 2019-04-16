TARGET = char_dev

ifneq ($(KERNELRELEASE),)

obj-m := $(TARGET).o
$(TARGET)-objs := chrdev.o

else

KERNELDIR := $(BUILD_KERNEL)

all:
	$(MAKE) -C $(KERNELDIR) M=$(CURDIR) modules
clean:
	$(MAKE) -C $(KERNELDIR) M=$(CURDIR) clean

endif
