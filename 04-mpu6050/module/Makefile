ifneq ($(KERNELRELEASE),)

obj-m := mpu6050.o

else

ifeq ($(KERNELDIR),)
ifeq ($(BBB_KERNEL),)

$(error Path to kernel tree - KERNELDIR or BBB_KERNEL variable is not defined!)

endif
endif

KERNELDIR ?= $(BBB_KERNEL)

export ARCH = arm
export CROSS_COMPILE ?= arm-linux-gnueabihf

.PHONY: all clean

all:
	$(MAKE) -C $(KERNELDIR) M=$(CURDIR) modules

clean:
	$(MAKE) -C $(KERNELDIR) M=$(CURDIR) clean

endif
