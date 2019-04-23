#!/bin/bash

while [ true ]
do
    scan=$(i2cdetect -y 0 | grep 68)
    if [[ $scan == "" ]]
    then
        echo  "Please connect MPU6050"
    else
        echo "MPU6050 found"
        break
    fi
sleep 0.3
done

