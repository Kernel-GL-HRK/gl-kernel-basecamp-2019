#! /bin/bash
DISTDIR=/tmp/guesanumber
if [ ! -d $DISTDIR ]
    then 
    mkdir $DISTDIR
fi

if [ ! -z "$(ls -A $DISTDIR)" ]
    then
    rm $DISTDIR/*.*
fi
cp ./*.* $DISTDIR
cd /tmp/
tar -czvf guesanumber.tar.gz ./guesanumber

