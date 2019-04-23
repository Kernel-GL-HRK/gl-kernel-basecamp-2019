#!/bin/bash

if [ "$EUID" -ne 0 ]
then 
	echo "This script requires root access"
	exit
fi
