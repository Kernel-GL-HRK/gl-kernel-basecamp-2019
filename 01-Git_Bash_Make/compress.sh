#!/bin/bash

path=$(pwd)
cp -r . /tmp/guessanumber
cd /tmp
tar -zcvf guessanumber.tar.gz guessanumber/
release_path="$path/../release"
mkdir -p "$release_path"
cp guessanumber.tar.gz "$release_path"

