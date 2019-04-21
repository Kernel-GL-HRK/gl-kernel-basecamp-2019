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

	SIGN=$(( (HIGH & 0x80) >> 7 ))
	TEMP=$(( (HIGH << 8) + LOW ))

	if [ $SIGN -eq 1 ] ; then
		TEMP=$(( -(0x10000 - TEMP) ))
	fi

	sleep 1
done
