obj-m := threads.o

KERNELDIR ?= ~/linux-4.19.36

export ARCH = arm

export CROSS_COMPILE ?= arm-linux-gnueabihf-

.PHONY: all clean

all:

	$(MAKE) -C $(KERNELDIR) M=$(CURDIR) modules

clean:

	$(MAKE) -C $(KERNELDIR) M=$(CURDIR) clean
