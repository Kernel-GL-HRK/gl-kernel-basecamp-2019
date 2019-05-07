#!/bin/bash

set -x #enable debug mode

MPU6050_ADDR="0x68"
WHO_AM_I_REG="0x75"

if i2cdetect -y 0 | grep 68
    then
        echo "MPU6050 connected success"
    else
        echo  "MPU6050 not found"
        exit 1
fi

who_am_i=$(i2cget -y 0 $MPU6050_ADDR $WHO_AM_I_REG)
echo "Value WHO_AM_I registr in mpu6050 : $who_am_i"

