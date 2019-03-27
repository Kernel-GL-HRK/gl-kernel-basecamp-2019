#!/bin/bash
if [ ! -d /tmp/guess_a_number ]; then
    mkdir -v /tmp/guess_a_number
fi
cp -v task_1.c /tmp/guess_a_number