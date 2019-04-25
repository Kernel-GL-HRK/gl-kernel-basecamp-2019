#!/bin/bash

started=`i2cget -y 1 0x68 0x6b`
started="${started: -2}"

if [ $started -ne 0 ]
then 
	i2cset -y 1 0x68 0x6b 0
fi

while [ true ]
do
	tempHexH=`i2cget -y 1 0x68 0x41`
	tempHexL=`i2cget -y 1 0x68 0x42`

	temperature=$(( -(((tempHexH << 8) + tempHexL) ^ ((1 << 0x10) - 1)) ))
	
	fahrenheit=`bc -l <<< "scale=3; ($temperature/340 + 36.53)" * 1.8 + 32"`
    
	echo "Fahrenheit: $fahrenheit"
	
	sleep 0.5
done

