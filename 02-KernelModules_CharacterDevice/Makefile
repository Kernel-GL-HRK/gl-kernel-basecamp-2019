CURRENT = $(shell uname -r)
KDIR := /lib/modules/$(CURRENT)/build
PWD := $(shell pwd)

# Список модулей для сборки
obj-m := driver.o

all:
	$(MAKE) -C $(KDIR) M=$(PWD) modules
            
clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean
