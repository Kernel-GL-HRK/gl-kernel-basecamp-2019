#!/bin/bash

if [ "$EUID" -ne 0 ]
	then echo "Please run as root"
	exit
fi

DEVICE=/dev/intercom
PROCFS=/proc/intercom/bufstate
SYSFS=/sys/class/GL/clear_buf

if [[ ! -c $DEVICE || ! -f $PROCFS || ! -f $SYSFS ]]
	then echo "Error! Module not loaded"
	exit -1
fi
