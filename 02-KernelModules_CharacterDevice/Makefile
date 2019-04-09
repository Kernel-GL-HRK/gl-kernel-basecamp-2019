TARGET = character_device


ifneq ($(KERNELRELEASE),)

obj-m := $(TARGET).o
$(TARGET)-objs := main.o

else

KERNELDIR := $(BUILD_KERNEL)

all:
	$(MAKE) -C $(KERNELDIR) M=$(CURDIR) modules
clean:
	$(MAKE) -C $(KERNELDIR) M=$(CURDIR) clean

endif

