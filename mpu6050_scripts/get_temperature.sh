#!/bin/bash

if i2cset -y 0 0x68 0x6b 0 ; then
	sleep 0.2
else
	echo "Error occurred!"
	exit
fi
