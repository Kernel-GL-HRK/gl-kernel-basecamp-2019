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

