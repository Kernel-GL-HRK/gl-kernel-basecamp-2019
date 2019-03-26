#!bin/sh

directory=/temp/game
archive=game.tar.gz

if [ ! -d $directory ]
then
    mkdir $directory
fi

cp *.c $DIR

tar -zcf $archive $directory
