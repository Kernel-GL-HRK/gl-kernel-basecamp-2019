#!/bin/bash
if [ "$EUID" -ne 0 ]
then 
	echo "Run as root!"
	exit
fi
