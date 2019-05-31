KDIR ?= /home/eduard/Kernel_ARM

obj-m := thread_module.o

.PHONY: all clean

all:
	$(MAKE) -C ${KDIR} M=$$PWD modules

clean:
	$(MAKE) -C ${KDIR} M=$$PWD clean
