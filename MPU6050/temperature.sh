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

i2cset -y 0 $REG_MPU6050 $REG_PWR_MGMT_1 $MASK_DEVICE_RESET

sleep 0.2

for ((;;))
do

TEMP=$(( ($TEMP_H<<8) | $TEMP_L ))

T=$(( ($TEMP & 0x7fff) - ($TEMP & 0x8000) ))

TEMP_CELS=$(echo "scale = 3; $T/340 +36.53" |bc)

printf "TEMP_CELS. = %.3f\n" $TEMP_CELS

FAHR=$(echo "scale = 3; ($TEMP_CELS * 9 / 5) + 32" | bc)

printf "TEMP_FAR. = %.3f\n\n" $FAHR

#printf "\n"

sleep 1

done

fi
