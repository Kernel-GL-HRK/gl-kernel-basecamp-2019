KDIR := /home/eduard/Kernel_i386

obj-m := character_driver.o

.PHONY: all clean

all:
	$(MAKE) -C ${KDIR} M=$$PWD modules

clean:
	$(MAKE) -C ${KDIR} M=$$PWD clean
	
