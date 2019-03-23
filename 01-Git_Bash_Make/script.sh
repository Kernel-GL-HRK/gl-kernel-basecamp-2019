#!/bin/bash

DIR=/tmp/guessanumber
ARCH=guessanumber.tar.gz
REL=release

if [ ! -d $DIR ]
then
    mkdir $DIR
fi
cp *.c $DIR
tar -zcf $ARCH $DIR

if [ ! -d $REL ]
then
    mkdir $REL
fi
mv $ARCH $REL
