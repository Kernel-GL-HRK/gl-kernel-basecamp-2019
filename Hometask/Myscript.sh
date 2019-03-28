#!/bin/bash

VALUE=/tmp/guesanumber

if [ ! -d $VALUE ]
then
 mkdir -v $VALUE
fi

cp *.cpp $VALUE

tar -zcvf guesanumber.tar.gz $VALUE

if [ ! -d release ]
then
 mkdir -v release
fi

mv guesanumber.tar.gz release
