#!/bin/sh

directory=/tmp/guesanumber
archive=game.tar.gz

if [ ! -d $directory ]
then
    mkdir $directory
fi

cp *.c $directory

tar -zcf $archive $directory
rm -r $directory
rel=release
if [ ! -d $rel ]
then
    mkdir $rel
fi
mv $archive $rel
