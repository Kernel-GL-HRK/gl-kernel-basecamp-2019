#!/bin/bash

set -x #enable debug mode

if i2cdetect -y 0 | grep 68
    then
        echo "MPU6050 connected success"
    else
        echo  "MPU6050 not found"
        exit 1
fi
