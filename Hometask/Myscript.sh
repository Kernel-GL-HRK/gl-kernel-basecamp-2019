#!/bin/bash

VALUE=/tmp/guesanumber

if [ ! -d $VALUE ]
then
 mkdir -v $VALUE
fi

cp *.cpp $VALUE

tar -zcvf guesanumber.tar.gz $VALUE
