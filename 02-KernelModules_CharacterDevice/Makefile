obj-m += character_driver.o

KERNELDIR := $(BUILD_KERNEL)

.PHONY: all clean
all:
	$(MAKE) -C $(KERNELDIR) M=$$(pwd) modules
clean:
	$(MAKE) -C $(KERNELDIR) M=$$(pwd) clean
