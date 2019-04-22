#!/bin/bash
if [ "$EUID" -ne 0 ]
then 
	echo "Run as root!"
	exit
fi

i2cset -y 0 0x68 0x6b 0
