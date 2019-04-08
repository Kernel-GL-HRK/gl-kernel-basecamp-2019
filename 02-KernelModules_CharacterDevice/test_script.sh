#!/bin/bash

PATH_TO_DRIVER="/home/user/char_driver/Character_driver.ko"
PATH_TO_PROC="/proc/My_proc_stats/Info"
PATH_TO_SYS="/sys/My_sys_operations/Clear"
PATH_TO_DEV="/dev/My_character_device"

insmod $PATH_TO_DRIVER BUF_SIZE=1111

printf "Checking buffer size...\n"
cat $PATH_TO_PROC

printf "\nWriting some data to device...\n"

echo "Test data" > $PATH_TO_DEV
echo "47 47 47" > $PATH_TO_DEV

printf "Checking stats after writing data...\n"

cat $PATH_TO_PROC

printf "\nReading data form device...\n"

cat $PATH_TO_DEV

printf "\nCheking stats after readiing data...\n"

cat $PATH_TO_PROC

printf "\nWriting another data to device for checking /sys interface...\n"

echo "Another test string" > $PATH_TO_DEV
echo "Very important" > $PATH_TO_DEV

printf "Stats:\n"
cat $PATH_TO_PROC

printf "\nClearing buffer by means of /sys/My_sys_operations/Clear\n"
echo 1 > $PATH_TO_SYS

printf "Stats after buffer clearing:\n"

cat $PATH_TO_PROC

printf "\nTrying to read data from device after clearing (should display nothing)...\n"

cat $PATH_TO_PROC

rmmod Character_driver

