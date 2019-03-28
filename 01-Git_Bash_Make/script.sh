#!/bin/bash

TMP_DIR=$tmpDir"tmp/guesanumber"
REALISE_DIR=$realiseDir"realise"

mkdir -p $TMP_DIR
if [ -d "${TMP_DIR}" ]; then
   mkdir -p $REALISE_DIR
   cp *.c $TMP_DIR
   tar -zcf $REALISE_DIR/guesanumber.tar.gz  -C  $TMP_DIR *.c
   
   rm ${TMP_DIR}/*.c
   rmdir -p $TMP_DIR
else
   echo "dir can not be created"
fi
