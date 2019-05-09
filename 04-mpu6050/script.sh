#!/bin/bash

REG_MPU6050="0x68"
REG_WHO_AM_I="0x75"
REG_PWR_MGMT_1="0x6b"
REG_TEMP_OUT_H="0x41"
REG_TEMP_OUT_L="0x42"
MASK_DEVICE_RESET="0x00"

TEMP=$(i2cget -y 0 $REG_MPU6050 $REG_WHO_AM_I)
if [ "$TEMP" != "$REG_MPU6050" ]
	then echo "failed to connect to mpu6050"
else printf "Who_am_I = %#x\n\n" $TEMP
fi
