#
# character device
#

ifneq ($(KERNELRELEASE),)

obj-m += concmod.o

else

.PHONY: all clean
all:
	${MAKE} -C ${BUILD_KERNEL} M=${CURDIR} modules

clean:
	${MAKE} -C ${BUILD_KERNEL} M=${CURDIR} clean

endif
