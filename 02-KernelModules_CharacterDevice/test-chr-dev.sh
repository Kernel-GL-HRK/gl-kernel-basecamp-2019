#!/bin/bash

MOD=chrdev

MODULE_PATH="/home/user/$MOD.ko"

insmod $MODULE_PATH buffer_size=128

echo "Device status..."
cat /proc/vdk-chrdev-example

echo "Writting hello to device..."
echo "Hello" > /dev/vdk-chrdev0

echo "Device status..."
cat /proc/vdk-chrdev-example

echo "Reading data..."
cat /dev/vdk-chrdev0

echo "Cleaning up data..."
echo 1 > /sys/kernel/vdk-chrdev-example/cleanup

echo "Reading data..."
cat /dev/vdk-chrdev0

echo "Device status..."
cat /proc/vdk-chrdev-example

rmmod $MOD

insmod $MODULE_PATH buffer_size=128
echo "Device status..."
cat /proc/vdk-chrdev-example
rmmod $MOD
