#!/bin/bash

path = "/tmp/guess_a_number";
if [ ! -d $path ]; then
    mkdir -v $path
fi
cp -v *.c /release/guess_a_number

if [ ! -d /release ]; then
    mkdir -v /release
fi
tar -czvf /release/guess_a_number.tar.gz /tmp/guess_a_number

