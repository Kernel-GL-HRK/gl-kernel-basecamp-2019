#! /bin/bash

TEMPDIR=/tmp/guessanumber

ARCHIVENAME=guessanumber.tar.gz

if [ ! -d $TEMPDIR ]
then
	mkdir $TEMPDIR
fi
