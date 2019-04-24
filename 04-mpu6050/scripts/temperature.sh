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

who_am_i=$(i2cget -y 0 0x68 117)
echo "Value of WHO_AM_I register is : $who_am_i"

echo "Turning on MPU6050"
$(i2cset -y 0 0x68 107 0)

while [ true ]
do
        tempH=$(i2cget -y 0 0x68 65)
        tempL=$(i2cget -y 0 0x68 66)
        temp_abs=$(( -(((tempH << 8)+ tempL) ^ ((1 << 0x10) - 1)) ))
        temp_C=$(echo "scale=3; $temp_abs / 340 + 36.53" | bc)
        temp_F=$(echo "scale=3; $temp_C * 9/5 + 32" | bc)
        echo "Celcius: $temp_C"
        echo "Fahrenheit: $temp_F"
        sleep 1
done

