#
# Char dev Linux kernel module
#

obj-m := intercom.o

all:
	$(MAKE) -C $(BUILD_KERNEL) M=$(CURDIR) modules

clean:
	$(MAKE) -C $(BUILD_KERNEL) M=$(CURDIR) clean
