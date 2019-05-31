obj-m := mpu6050.o
KERNELDIR=/home/danilpetrov/Kernel
export ARCH = arm
export CROSS_COMPILE ?= arm-linux-gnueabihf-


.PHONY: all clean

all:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules

clean:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) clean

