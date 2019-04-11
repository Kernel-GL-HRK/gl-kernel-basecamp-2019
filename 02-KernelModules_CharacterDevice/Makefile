modulefile=moduleCharDev

KDIR=$(BUILD_KERNEL)

obj-m := $(modulefile).o

all:
	$(MAKE) -C $(KDIR) M=$(PWD) modules
clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean

