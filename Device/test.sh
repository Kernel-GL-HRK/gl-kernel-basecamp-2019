#! /bin/bash

MODULE_NAME=${1:-char_Mydriver}

MODULE_FULL_PATH=${MODULE_PATH}${MODULE_NAME}.ko

DEVICE_NAME=${2:-chdev}

DEV_PATH=/dev/${DEVICE_NAME}
PROC_PATH=/proc/${DEVICE_NAME}_proc_dir/${DEVICE_NAME}_proc_file
SYS_PATH=/sys/${DEVICE_NAME}_sys_dir/${DEVICE_NAME}_sys_file

echo "Loading module"
insmod ${MODULE_FULL_PATH}

echo "Checking if module is loaded:"
lsmod | grep "${MODULE_NAME}"

echo "Reading from /proc file (should be 1024 allocated, 0 used):"
cat ${PROC_PATH}

echo "Writing \" Random text 1 2 3\" to device"
echo "Random text 1 2 3" > ${DEV_PATH}

echo "Reading from /proc file (should be 1024 allocated, 18 used):"
cat ${PROC_PATH}

echo "Reading from device (should be \"Random text 1 2 3\")":
cat ${DEV_PATH}

echo "Writing \"1\" to /sys file to clean the buffer"
echo 1 > ${SYS_PATH}

echo "Reading from /proc file (should be 1024 allocated, 0 used):"
cat ${PROC_PATH}

echo "Reading from device (should be nothing):"
cat ${DEV_PATH}

echo "Unloading module"
rmmod ${MODULE_NAME}

echo "Loading module with increased buffer size (2048):"
insmod ${MODULE_FULL_PATH} alloc_size=2048

echo "Checking if module is loaded:"
lsmod | grep "${MODULE_NAME}"

echo "Reading from /proc file (should be 2048 allocated, 0 used):"
cat ${PROC_PATH}

echo "Unloading module"
rmmod ${MODULE_NAME}

echo "Trying to load module with buffer size < 1024 (should not work):"
insmod ${MODULE_FULL_PATH} alloc_size=512

echo "Checking if module is loaded (should be nothing):"
lsmod | grep "${MODULE_NAME}"

echo "Testing done"
