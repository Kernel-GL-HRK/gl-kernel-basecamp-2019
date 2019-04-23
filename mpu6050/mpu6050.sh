#!/bin/bash

if [ "$EUID" -ne 0 ]
then 
	echo "This script requires root access"
	exit
fi

i2cset -y 0 0x68 0x6b 0

while :
do

	MAJOR=$(i2cget -y 0 0x68 0x41)
	MINOR=$(i2cget -y 0 0x68 0x42)
	
	Temperature=$(( -(((MAJOR << 8)+ MINOR) ^ ((1 << 0x10) - 1)) ))

	Fahrenheit=`bc -l <<< "scale=3; ($Temperature/340 + 36.53) * 1.8 + 32"`	
	
	echo "Temperature = $Fahrenheit"
	sleep 2
	
done
