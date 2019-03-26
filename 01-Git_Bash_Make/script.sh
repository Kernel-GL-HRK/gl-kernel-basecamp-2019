#!/bin/bash

mkdir -p /tmp/guesanumber
cp *.c /tmp/guesanumber/

mkdir -p release
tar -zcf release/guesanumber.tar.gz  -C  /tmp/guesanumber *.c