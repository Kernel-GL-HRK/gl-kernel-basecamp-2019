# Kernel lib
KDIR := /lib/modules/$(shell uname -r)/build

# Workspaces
PWD := $(shell pwd)

#  List mod for build
obj-m := chrdev_driver.o

all:
	$(MAKE) -C $(KDIR) M=$(PWD) modules
clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean

