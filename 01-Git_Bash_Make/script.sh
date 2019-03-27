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

if [ ! -d $DISTDIR/realese ]
    then 
    mkdir $DISTDIR/realese
elif [ ! -z "$(ls -A $DISTDIR/realese)" ]
    then
    rm $DISTDIR/realese/*.*
else
    cp guesanumber.tar.gz $DISTDIR/realese/
    rm $DISTDIR/*.*
fi

