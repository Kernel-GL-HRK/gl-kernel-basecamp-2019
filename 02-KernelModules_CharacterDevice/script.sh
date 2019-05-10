#!/bin/bash

MODULE=driver.ko
PROC="/proc/chrdev"
DEV="/dev/chrdev0"
SYS="/sys/kernel/chrdev/foo"

echo "TEST : module loading without parameters"
sudo insmod $MODULE
dmesg | tail -n4
sudo chmod -R 0666 $DEV

echo "TEST : buffer size (proc)"
cat $PROC
dmesg | tail -n2

echo "TEST : writing to file (dev)"
echo "Hello, dev" > $DEV

echo "TEST : buffer size (proc)"
cat $PROC
dmesg | tail -n2

echo "TEST : reading from file (dev)"
cat $DEV
dmesg | tail -n1

echo "TEST : buffer size (proc)"
cat $PROC
dmesg | tail -n2

echo "TEST : module unloading"
sudo rmmod driver
dmesg | tail -n1

echo "TEST : module loading with BUFFER_SIZE=1500"
sudo insmod $MODULE BUFFER_SIZE=1500
dmesg | tail -n4

echo "TEST : buffer size (proc)"
cat $PROC
dmesg | tail -n2

echo "TEST : module unloading"
sudo rmmod driver
dmesg | tail -n1


echo "TEST : buffer cleaning"
sudo insmod $MODULE
dmesg | tail -n4
sudo chmod -R 0666 $DEV

echo "TEST : buffer size (proc)"
cat $PROC
dmesg | tail -n2

echo "TEST : writing to file (dev)"
echo "Hello, dev" > $DEV

echo "TEST : buffer size (proc)"
cat $PROC
dmesg | tail -n2

echo "TEST : buffer cleaning (sys)"
echo "1" > $SYS

echo "TEST : buffer size (proc)"
cat $PROC
dmesg | tail -n2

echo "TEST : module unloading"
sudo rmmod driver
dmesg | tail -n1


