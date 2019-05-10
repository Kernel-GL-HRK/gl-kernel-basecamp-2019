# MPU6050 Practice. Kernel module

## Homework

1. Build Linux kernel from www.kernel.org for Orange PI
	Use build instruction from http://linux-sunxi.org/Mainline_Kernel_Howto
	make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- sunxi_defconfig
	ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- make -j4 zImage
2. Build driver for mpu6050 which provide data to sysfs
3. Measure temperature in F. with precision 0.001 F
4. (Optional) create digital filter for temperature channel.
  E.g. moving averange filter for 1000 points.