#!/bin/bash

mkdir -p /tmp/guess_a_number
mkdir -p release

cp *.cpp /tmp/guess_a_number/

tar -zcf release/guess_a_number.tar.gz  -C  /tmp/guess_a_number *.cpp
