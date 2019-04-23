KDIR := /lib/modules/$(shell uname -r)/build

PWD := $(shell pwd)

obj-m := xchar.o

all:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean

install:
	sudo insmod xchar.ko

remove:
	sudo rmmod xchar

test:
	echo 1 2 3 4 5 > /dev/xchar0
	cat /dev/xchar0
