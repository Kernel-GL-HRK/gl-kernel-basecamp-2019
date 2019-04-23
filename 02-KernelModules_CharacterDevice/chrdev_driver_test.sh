#!/bin/bash

MODULE=chrdev_driver.ko
DRIVER_NAME=chrdev_driver
MODULE_LOC=$MODULE_TEST_PATH

PROC="/proc/chrdev_info"
DEV="/dev/chrdev_cdev"
SYSFS="/sys/kernel/chrdev_flag/memory_free"

echo "Module:"
echo $MODULE
echo "Module path:"
echo $MODULE_LOC
echo 
echo "Start driver:"
insmod $MODULE_LOC/$MODULE

echo "Memory status (Buffer size, Free memory):"
cat $PROC

echo "Write string 'Hello world!' to device buffer:"
echo "Hello world!" > $DEV 

echo "Checking buffer size:"
cat $PROC

echo "Read from buffer:"
cat $DEV

echo "Cleaning device buffer:"
echo 1 > $SYSFS

echo "Checking buffer size:"
cat $PROC

echo "Free driver:"
rmmod $DRIVER_NAME

echo 
echo "Start driver with chrdev_buffer_size parameter:"
insmod $MODULE_LOC/$MODULE chrdev_buffer_size=128

echo "Checking buffer status:"
cat $PROC

echo "Free driver:"
rmmod $DRIVER_NAME

echo
echo "Start driver:"
insmod $MODULE_LOC/$MODULE

echo "Write string 'Hello world!' to device buffer:"
echo "Hello world!" > $DEV 

echo "Checking buffer size:"
cat $PROC

echo "Read from buffer:"
cat $DEV

echo "Cleaning device buffer with flag 5:"
echo 5 > $SYSFS

echo "Checking buffer size:"
cat $PROC

echo "Read from buffer:"
cat $DEV

echo "Free driver:"
rmmod $DRIVER_NAME

echo
echo "Done"

