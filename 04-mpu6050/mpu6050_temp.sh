#!/bin/bash

PWR_MGMT_1="0x6b"
TEMP_OUT_H="0x41"
TEMP_OUT_L="0x42"

if i2cset -y 1 0x68 $PWR_MGMT_1 0x00
then
    echo "Device wake up\n"
    sleep 2
else
    echo "Error\n"
    exit
fi 

while :
do
    RAW_H=$(i2cget -y 1 0x68 $TEMP_OUT_H)
    RAW_L=$(i2cget -y 1 0x68 $TEMP_OUT_L)
done
