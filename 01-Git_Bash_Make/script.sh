#!/bin/bash

DEST=/tmp/guesanumber

if [ ! -d $DEST ]
then
 mkdir -v $DEST
fi

cp *.cpp $DEST

tar -zcvf guesanumber.tar.gz $DEST










