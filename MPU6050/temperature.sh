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

who_am_i=(i2cget -y 0 0x68 117)
echo "Value of WHO_AM_I register is : $who_am_i"

echo "Turning on MPU6050"
i2cset -y 0 0x68 107 0

while true
do
TempH=$(i2cget -y 0 0x68 65)
TempL=$(i2cget -y 0 0x68 66)
Temp_abs=$(( -(((TempH << 8)+ TempL) ^ ((1 << 0x10) - 1)) ))
Temp_C=$(echo "scale=3; $Temp_abs / 340 + 36.53" | bc)
Temp_F=$(echo "scale=3; $Temp_C * 9/5 + 32" | bc)
echo "Celcius: $Temp_C"
echo "Fahrenheit: $Temp_F"
sleep 1
