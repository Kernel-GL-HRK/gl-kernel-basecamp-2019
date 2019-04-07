#! /bin/bash

help()
{
	printf "This script is designed to test the kernel module\n"
	printf "The syntax for selecting actions: \n"
	printf "  $0 [actions] [args]\n"
	printf "List of available actions:\n"
	printf "  help - output of the message prompts\n"
	printf "  imod [path] [size_buffer] - loads the module into the kernel\n"
	printf "    Arguments:\n"
	printf "      path - kernel module path\n"
	printf "      size_buffer - desired buffer size is not less
	       (optional argument, default 1024)\n"
	printf "  rmod [name] - unload module from kernel\n"
	printf "    Arguments:\n"
	printf "      name - the name of the module that will be unloaded
	       (optional argument, default \"concmod\")\n"
	printf "  dout - displays text from a dev file\n"
	printf "  din \"[text]\" - write text to dev file\n"
	printf "    Arguments:\n"
	printf "      text - writeable text\n"
	printf "  memout - output buffer status\n"
	printf "  clear - clears buffer\n"
}

imod()
{
	if [[ $1 != "" ]]; then
		insmod $1 $2
	else
		insmod $1
	fi
}

rmod()
{
	if [[ $1 != "" ]]; then
		rmmod $1
	else
		rmmod concmod
	fi
}

memout()
{
	printf "Full capacity: "
	cat /proc/concdev | awk '{print $1}'
	printf "Free memory: "
	cat /proc/concdev | awk '{print $2}'
}

if [[ $1 == "" || $1 == "help" ]]; then
	help
elif [[ $1 == "imod" ]]; then
	imod $2 $3
elif [[ $1 == "rmod" ]]; then
	rmod $2
elif [[ $1 == "dout" ]]; then
	cat /dev/concdev
elif [[ $1 == "din" ]]; then
	echo $2 >> /dev/concdev
elif [[ $1 == "memout" ]]; then
	memout
elif [[ $1 == "clear" ]]; then
	echo 1 >> /sys/devices/concdev/clear
else
	help
fi