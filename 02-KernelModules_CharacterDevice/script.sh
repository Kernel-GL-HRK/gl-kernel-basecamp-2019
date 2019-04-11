#!/bin/bash

DRIVER_NAME="chrdev.ko"
DRIVER_PATH="$MODULE_OBJECT$DRIVER_NAME"
PROC_PATH="/proc/chrdev-info"
SYS_PATH="/sys/kernel/chrdev-flag/clear_memory"
DEV_PATH="/dev/chrdev_cdev0"

insmod $DRIVER_PATH BUFFER_SIZE =1500

printf "Checking device \n"
cat $PROC_PATH

printf "Writing to device (HELLO TEST)\n"
echo "HELLO TEST" > $DEV_PATH

printf "Checking device \n"
cat $PROC_PATH

printf "Reading form device \n"
cat $PATH_TO_DEV

printf "Cheking after reading \n"
cat $PATH_TO_PROC

printf "Clearing buffer \n"
echo 1 > $SYS_PATH

printf "Reading form device ()\n"
cat $PATH_TO_DEV

printf "Checking device \n"
cat $PROC_PATH

rmmod $DRIVER_PATH

