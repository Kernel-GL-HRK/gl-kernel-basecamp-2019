!#/bin/bash
home_path=
proc_file_path=
dev_file_path=
sys_file_path=
test_file_path=
mod_name=
test_name=

echo "input path to your module as path/to/your/module/module_name.ko" 
read $home_path
echo "input path to your proc file as path/to/your/file" 
read $proc_file_path
echo "input path to your sys file as path/to/your/file" 
read $sys_file_path
echo "input path to your test file as path/to/your/file" 
read $test_file_path
echo "input name of your module as module_name.kp" 
read $mod_nmae
echo "input name of your test program" 
read $test_name
