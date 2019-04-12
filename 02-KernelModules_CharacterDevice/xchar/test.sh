#!/bin/bash

DRIVER_NAME=xchar
PROC_USED_BUFFER_VOLUME=used_buffer_volume
PROC_BUFFER_SIZE=buffer_size
SYS_CLEAN_UP_BUFFER=clean_up_buffer

COUNTER=0

insmod ./${DRIVER_NAME}.ko

COUNTER=$[$COUNTER+1]
echo "$COUNTER. Write \"12345\" to \"/dev/${DRIVER_NAME}0\""
echo 12345 > /dev/${DRIVER_NAME}0

echo "Checking \"/proc/${DRIVER_NAME}/${PROC_USED_BUFFER_VOLUME}\"..."
expected=6
actual=$(cat /proc/${DRIVER_NAME}/${PROC_USED_BUFFER_VOLUME})
if [[ $? != 0 ]]; then
    echo "ERROR. Command failed."
elif [ "$expected" == "$actual" ]; then
    echo "\"$expected\" - Ok."
else
    echo "ERROR. Actual is \"$actual\" but \"$expected\" expected."
fi

echo "Checking \"/proc/${DRIVER_NAME}/${PROC_BUFFER_SIZE}\"..."
expected=1024
actual=$(cat /proc/${DRIVER_NAME}/${PROC_BUFFER_SIZE})
if [[ $? != 0 ]]; then
    echo "ERROR. Command failed."
elif [ "$expected" == "$actual" ]; then
    echo "\"$expected\" - Ok."
else
    echo "ERROR. Actual is \"$actual\" but \"$expected\" expected."
fi

echo "Checking \"/dev/${DRIVER_NAME}0\"..."
expected=12345
actual=$(cat /dev/${DRIVER_NAME}0)
if [[ $? != 0 ]]; then
    echo "ERROR. Command failed."
elif [ "$expected" == "$actual" ]; then
    echo "\"$expected\" - Ok."
else
    echo "ERROR. Actual is \"$actual\" but \"$expected\" expected."
fi

COUNTER=$[$COUNTER+1]
echo "$COUNTER. Write \"abc\" to \"/dev/${DRIVER_NAME}0\""
echo abc > /dev/${DRIVER_NAME}0

echo "Checking \"/proc/${DRIVER_NAME}/${PROC_USED_BUFFER_VOLUME}\"..."
expected=4
actual=$(cat /proc/${DRIVER_NAME}/${PROC_USED_BUFFER_VOLUME})
if [[ $? != 0 ]]; then
    echo "ERROR. Command failed."
elif [ "$expected" == "$actual" ]; then
    echo "\"$expected\" - Ok."
else
    echo "ERROR. Actual is \"$actual\" but \"$expected\" expected."
fi

COUNTER=$[$COUNTER+1]
echo "$COUNTER. Write \"1\" to \"/sys/class/${DRIVER_NAME}/${SYS_CLEAN_UP_BUFFER}\""
echo 1 > /sys/class/${DRIVER_NAME}/${SYS_CLEAN_UP_BUFFER}

echo "Checking \"/proc/${DRIVER_NAME}/${PROC_USED_BUFFER_VOLUME}\"..."
expected=0
actual=$(cat /proc/${DRIVER_NAME}/${PROC_USED_BUFFER_VOLUME})
if [[ $? != 0 ]]; then
    echo "ERROR. Command failed."
elif [ "$expected" == "$actual" ]; then
    echo "\"$expected\" - Ok."
else
    echo "ERROR. Actual is \"$actual\" but \"$expected\" expected."
fi

echo "Checking \"/dev/${DRIVER_NAME}0\"..."
expected=""
actual=$(cat /dev/${DRIVER_NAME}0)
if [[ $? != 0 ]]; then
    echo "ERROR. Command failed."
elif [ "$expected" == "$actual" ]; then
    echo "\"$expected\" - Ok."
else
    echo "ERROR. Actual is \"$actual\" but \"$expected\" expected."
fi

rmmod ${DRIVER_NAME}
