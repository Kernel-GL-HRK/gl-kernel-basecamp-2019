#!/bin/bash

set -x #enable debug mode

MPU6050_ADDR="0x68"
WHO_AM_I_REG="0x75"
WAKE_UP_REG="0x6b"
TEMP_H="0x41"
TEMP_L="0x42"

if i2cdetect -y 0 | grep 68
    then
        echo "MPU6050 connected success"
    else
        echo  "MPU6050 not found"
        exit 1
fi

who_am_i=$(i2cget -y 0 $MPU6050_ADDR $WHO_AM_I_REG)
echo "Value WHO_AM_I registr in mpu6050 : $who_am_i"

echo "Wake up MPU6050"
$(i2cset -y 0 $MPU6050_ADDR $WAKE_UP_REG 0)

while [ true ]
do
    temp_h=$(i2cget -y 0 $MPU6050_ADDR $TEMP_H)
    temp_l=$(i2cget -y 0 $MPU6050_ADDR $TEMP_L)
    
    temp_adc_unsig=$((($temp_h << 8)+ $temp_l))
    temp_sig=$(((temp_adc_unsig & 0x7fff)- (temp_adc_unsig & 0x8000)))
    echo "Value ADC: $temp_sig"

    temp_c=$(echo " scale = 2; $temp_sig / 340 + 36.53" | bc)
    echo "Value temperature in Celsius: $temp_c"

    sleep 1
done

