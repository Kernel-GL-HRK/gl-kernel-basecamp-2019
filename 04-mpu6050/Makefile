#
# mpu6050 accelerometer & gyroscope
#

export CROSS_COMPILE=/usr/bin/arm-linux-gnueabihf-
export KERNELDIR=/home/alex/build/cache/sources/linux-mainline/orange-pi-4.19
PWD := $(shell pwd)

ifneq ($(KERNELRELEASE),)

obj-m := mpu6050.o

else


ifeq ($(KERNELDIR),)
ifeq ($(BBB_KERNEL),)
	$(error Path to kernel tree - KERNELDIR or BBB_KERNEL variable is not defined!)
endif
endif

#KERNELDIR ?= $(BBB_KERNEL)


all:
	$(MAKE) ARCH=arm CROSS_COMPILE=$(CROSS_COMPILE) -C $(KERNELDIR) M=$(PWD) modules

clean:
	$(MAKE) ARCH=arm CROSS_COMPILE=$(CROSS_COMPILE) -C $(KERNELDIR) M=$(PWD) clean


endif
