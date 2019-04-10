#!/bin/bash

path=$(pwd)
cp -r . /tmp/guessanumber
cd /tmp
tar -zcvf guessanumber.tar.gz guessanumber/
