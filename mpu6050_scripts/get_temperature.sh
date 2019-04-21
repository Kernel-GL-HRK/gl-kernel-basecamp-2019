#!/bin/bash

if i2cset -y 0 0x68 0x6b 0 ; then
	sleep 0.2
else
	echo "Error occurred!"
	exit
fi

while :
do
	HIGH=$(i2cget -y 0 0x68 0x41)
	LOW=$(i2cget -y 0 0x68 0x42)
	sleep 1
done
