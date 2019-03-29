#!/bin/bash

PATH = "/tmp/guess_a_number";
if [ ! -d $PATH ]; then
    mkdir -v $PATH
fi
cp -v *.c $PATH

if [ ! -d /release ]; then
    mkdir -v /release
fi
tar -zcvf guess_a_number.tar.gz $PATH

mv guess_a_number.tar.gz /release

