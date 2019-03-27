#!/bin/bash
if [ ! -d /tmp/guess_a_number ]; then
    mkdir -v /tmp/guess_a_number
fi
cp -v task_1.c /tmp/guess_a_number

if [ ! -d /release ]; then
    mkdir -v /release
fi
tar -czvf /release/guess_a_number.tar.gz /tmp/guess_a_number