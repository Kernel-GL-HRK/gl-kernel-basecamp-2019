#!/bin/bash

tmpDir=$tmpDir"tmp/guesanumber"
realiseDir=$realiseDir"realise"

mkdir -p $tmpDir
if [ -d "${tmpDir}" ]; then
   mkdir -p $realiseDir
   cp *.c $tmpDir
   tar -zcf $realiseDir/guesanumber.tar.gz  -C  $tmpDir *.c
   
   rm ${tmpDir}/*.c
   rmdir -p $tmpDir
else
   echo "dir can not be created"
fi
