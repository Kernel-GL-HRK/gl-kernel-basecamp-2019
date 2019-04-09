#!/bin/bash

DEVICE=/dev/intercom
PROCFS=/proc/intercom/bufstate
SYSFS=/sys/class/GL/clear_buf

if [[ ! -c $DEVICE || ! -f $PROCFS || ! -f $SYSFS ]]
	then echo "Error! Module not loaded"
	exit -1
fi
