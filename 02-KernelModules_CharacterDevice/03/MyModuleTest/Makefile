
TARGET = char_driver

obj-m := $(TARGET).o

KERNELDIR := $(BUILD_KERNEL)

.PHONY: all clean

all:
	$(MAKE) -C $(KERNELDIR) M=$(CURDIR) modules

clean:
	$(MAKE) -C $(KERNELDIR) M=$(CURDIR) clean
