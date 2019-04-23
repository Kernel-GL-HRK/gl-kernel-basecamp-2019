!#/bin/bash

MPATH="/home/user/$MODNAME.ko"
DEV="/dev/chrdev0"
PROCFS="/proc/chrdev"
SYSFS="/sys/class/chrdev"
MODNAME=chrdev

insmod $MPATH BUFFER_SIZE=512

echo "Status"
cat $PROCFS

echo "writting "something" to dev"
echo "something"> $DEV

echo "reading data"
cat $DEV

rmmod $MODNAME


