# Instructions for testing the kernel module

## Tasklist

* compile the module with the `make` command
* run the script `./test.sh` to view the help
* load kernel module with the command `./test.sh imod concmod.ko [size_buffer]`
	- as the buffer size, select any value greater than 1024
* output the buffer characteristics with the command `./test.sh memout`
* write text to the buffer with the command `./test.sh din "text"`
* output the recorded text with the command `./test.sh dout`
* check the current buffer status with a command `./test.sh memout`
* clear buffer with command `./test.sh clear`
* output the recorded text with the command `./test.sh dout`
* check the current buffer status with a command `./test.sh memout`
* unload the module with the command `./test.sh rmod`