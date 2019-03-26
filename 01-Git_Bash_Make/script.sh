#!/bin/bash
# Bash script

DIR=/tmp/guesanumber
ARCH=guesanumber.tar.gz
REL=release

if ! [ -d $DIR ]
then
	mkdir $DIR
fi

cp *.c $DIR

tar -czf $ARCH $DIR

if ! [ -d $REL ]
then
	mkdir $REL
fi

