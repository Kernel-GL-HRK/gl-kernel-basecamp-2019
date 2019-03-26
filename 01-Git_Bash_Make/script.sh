#!/bin/bash
# Bash script


DIR=/tmp/guesanumber
ARCH=guesanumber.tar.gz


if ! [ -d $DIR ]
then
	mkdir $DIR
fi

cp *.c $DIR

tar -czf $ARCH $DIR

















