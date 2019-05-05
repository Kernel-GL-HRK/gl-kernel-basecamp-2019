#!/bin/bash

while true
do
    scan=$(i2cdetect -y 0 | grep 68)
    if [[ $scan == "" ]]
    then
        echo  "Please connect to  MPU6050"
    else
        echo   "MPU6050 to found"
        break
    fi
sleep 0.3
done

who_am_i=i2cget -y 0 0x68 117
echo "Value of WHO_AM_I register is : $who_am_i"

echo "Turning on MPU6050"
i2cset -y 0 0x68 107 0

