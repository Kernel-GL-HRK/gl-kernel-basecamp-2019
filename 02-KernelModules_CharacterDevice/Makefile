obj-m += chrdev.o

PATHTOMOD=/home/alex/Downloads/linux-5.0.7

all:
	make -C ${PATHTOMOD} M=$(PWD) modules

clean:
	make -C ${PATHTOMOD}/build M=$(PWD) clean

