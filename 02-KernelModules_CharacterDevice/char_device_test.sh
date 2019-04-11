#!/bin/bash

Module="character_device.ko"

Module_location="$MODULE_PATH$Module"

Proc="/proc/charDevice/chd"

Dev="/dev/charDevice"

Sys="/sys/kernel/ChD/clear_memory"

echo "Loading module:"
insmod $Module_location

echo "Inital memory status (Buffer size, Free memory):"
cat $Proc

echo "Attempt to write string 'Some text' to device buffer:"
echo "Some text" > $Dev 

echo "Checking memory status after using some buffer space:"
cat $Proc

echo "Reading from buffer:"
cat $Dev

echo "Cleaning device buffer through sysfs:"
echo 1 > $Sys

echo "Checking memory status after cleaning buffer:"
cat $Proc

echo "Unloading module:"
rmmod $Module

echo "Loading module with buffer_increase module parameter:"
insmod $Module_location buffer_increase=512

echo "Checking increased memory status:"
cat $Proc

echo "Unloading module:"
rmmod $Module

echo "Done"

