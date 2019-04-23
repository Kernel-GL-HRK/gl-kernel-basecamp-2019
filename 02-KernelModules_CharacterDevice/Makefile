TARGET = chrdev

ifneq ($(KERNELRELEASE),)

obj-m := $(TARGET).o
$(TARGET).objs := chrdev.o

else

KERNELDIR := $(BUILD_KERNEL)

.PHONY: all clean
all:
	$(MAKE) -C $(KERNELDIR) M=$(CURDIR) modules
clean:
	$(MAKE) -C $(KERNELDIR) M=$(CURDIR) clean

endif

