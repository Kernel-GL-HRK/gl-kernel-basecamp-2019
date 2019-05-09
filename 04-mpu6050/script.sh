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

i2cset -y 0 $REG_MPU6050 $REG_PWR_MGMT_1 $MASK_DEVICE_RESET

sleep 0.2

for ((;;))
do

TEMP_H=$(i2cget -y 0 $REG_MPU6050 $REG_TEMP_OUT_H)

printf "TEMP_OUT_H = %#x\n" $TEMP_H

TEMP_L=$(i2cget -y 0 $REG_MPU6050 $REG_TEMP_OUT_L)

printf "TEMP_OUT_L = %#x\n" $TEMP_L

TEMP=$(( ($TEMP_H<<8) | $TEMP_L ))

T=$(( ($TEMP & 0x7fff) - ($TEMP & 0x8000) ))

TEMP=$(( $T/340 ))
TEMP=$(( $TEMP+36 ))

printf "TEMP_CELS. = %d\n" $TEMP

FAR=$(( $(($TEMP * 9 / 5)) + 32))

printf "TEMP_FAR. = %d\n\n" $FAR

#printf "\n"

sleep 1

done

fi
