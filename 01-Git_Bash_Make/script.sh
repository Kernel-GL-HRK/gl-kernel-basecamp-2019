#! /bin/bash

TEMPDIR=/tmp/guessanumber

ARCHIVENAME=guessanumber.tar.gz

if [ ! -d $TEMPDIR ]
then
	mkdir $TEMPDIR
fi

cp ./*.c $TEMPDIR

tar -czf $ARCHIVENAME $TEMPDIR

if [ ! -d ./release ]
then
	mkdir ./release
fi

mv $ARCHIVENAME ./release

rm -rf $TEMPDIR
