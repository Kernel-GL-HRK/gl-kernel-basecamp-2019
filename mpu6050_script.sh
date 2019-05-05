#!/bin/bash

Power="0x6B"
TEMP_OUT_H="0x41"
TEMP_OUT_L="0x42"
MPU6050="0x68"
WHO_AM_I="0x75"

echo "Probing for mpu6050..."
tmp=$(i2cget -y 1 $MPU6050 $WHO_AM_I)
sleep 0.2


#echo "mpu" $MPU6050
if [ "$tmp" != "$MPU6050" ]
	then echo "MPU6050 not found"
else  printf 'Device answer is OK and = %#x\n' $tmp 
fi

sleep 0.2

#Set device online
i2cset -y 1 $MPU6050 $Power 0

sleep 0.2
