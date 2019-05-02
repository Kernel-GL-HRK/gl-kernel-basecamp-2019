#!/bin/bash


i2cset -y 1 0x68 0x6b 0

while :
do

	MAJOR=$(i2cget -y 0 0x68 0x41)
	MINOR=$(i2cget -y 0 0x68 0x42)
	
	SIGNED=$(( (MAJOR & 0x80) >> 7))
	Temperature=$(( (MAJOR << 8) + MINOR))
	
	if [ $SIGNED -eq 1 ] ; then
		Temperature=$(( -(Temperature ^ 0xFFFF) ))
	fi

	Fahrenheit=`bc -l <<< "scale=3; ($Temperature/340 + 36.53) * 1.8 + 32"`	
	
	echo "Temperature = $Fahrenheit"
	sleep 2
	
done


