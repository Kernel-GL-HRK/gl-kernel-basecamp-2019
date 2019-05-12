obj-m := mpu6050.o

KERNELDIR ?=

export ARCH = arm
export CROSS_COMPILE ?= arm-linux-gnueabihf-

.PHONY: all clean

all:
	$(MAKE) -C $(KERNELDIR) M=$(CURDIR) modules
clean:
	$(MAKE) -C $(KERNELDIR) M=$(CURDIR) clean