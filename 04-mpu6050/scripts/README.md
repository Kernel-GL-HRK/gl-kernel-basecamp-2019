# MPU6050 Practice. Hardware diagnostic and user space mode

## Homework

1. Download datasheet and registers map for mpu6050(mpu6000). Use it as reference.
2. Connect gyroscope, accelerometer, thermometer to OrangePI using schematic for Orange PI One 
	and schematic for board GY-521.
3. Check connected hardware using tools for communication from Linux user space i2c-tools package.
	Tools i2detect, i2cdump, i2cset, i2cget
4. Check the value WHO_AM_I register in mpu6050.
5. Run mpu6050 by setting DEVICE_RESET bit of PWR_MGMT_1 to 0.
6. Print value of temperature (TEMP_OUT_H, TEMP_OUT_L) using i2cget.
7. (Optional) Write shell script which prints value of temperature (TEMP_OUT_H, TEMP_OUT_L)
	once per second using i2cget in infinite loop. Value should be converted to F and C.
	Upload the script into repository to folder ‘mpu6050_scripts’.
