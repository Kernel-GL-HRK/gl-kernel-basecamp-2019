#!/bin/bash

i2cset -y 1 0x68 0x6b 0

while true
do
	tempHexH=`i2cget -y 1 0x68 0x41`
	tempHexL=`i2cget -y 1 0x68 0x42`
	
	sign=$(( (tempHexH & 0x80) >> 7 ))
	temperature=$(( (tempHexH << 8) + tempHexL ))
	
	if [ $sign = 1 ]
	then
		temperature=$(( $temperature - 0x10000 ))
	fi
	
	fahrenheit=`bc -l <<< "scale=3; ($temperature/340 + 36.53) * 1.8 + 32"`
    
	echo -n "Fahrenheit: $fahrenheit"
	echo -en "\r"
	sleep 0.5
done

