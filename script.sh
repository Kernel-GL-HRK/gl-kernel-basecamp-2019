!#/bin/bash

echo "write path to module directory as path/to/module_directory"
read home_path
echo "write path to proc file as path/to/proc/file"
read proc_file_path
echo "write path to dev file as path/to/dev/file"
read dev_file_path
echo "write path to sys file as path/to/sys/file"
read sys_file_path
read test_file_path
echo "write path to test file as path/to/test/file"
echo "write module name as module_name.ko"
read mod_name


echo "Loading modul"
insmod $home_path$mod_name 
echo "checking module status"
cat $proc_file_path
echo "writing to device"
echo  "0123HELLO456789" > $dev_file_path
echo "checking module status"
cat $proc_file_path
echo "clearing a buffer"
echo 1 >$sys_file_path
echo "checking module status"
cat $proc_file_path
echo "writing to device"
echo  "0123HELLO456789" > $dev_file_path
echo "checking module status"
cat $proc_file_path
echo "reading data from device"
cat /dev/chtest
echo "unloading module"
rmmod $mod_name

echo "input Size of buffer" 
read B_size

echo $home_path$mod_name B_size=$B_size
insmod $home_path$mod_name B_size=$B_size 
echo "Loading modul" 
echo "checking module status"
cat $proc_file_path
echo "writing to device"
echo  "0123HELLO456789" > $dev_file_path
echo "checking module status"
cat $proc_file_path
echo "clearing a buffer"
echo 1 >$sys_file_path
echo "checking module status"
cat $proc_file_path
echo "writing to device"
echo  "0123HELLOHELLLO456789" > $dev_file_path
echo "checking module status"
cat $proc_file_path
echo "reading data from device"
cat /dev/chtest
echo "unloading module"
rmmod $mod_name
