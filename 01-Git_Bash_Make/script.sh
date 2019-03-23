#! /bin/bash
# Bash script

# Directory check
if ! [ -d /tmp/guesanumber ]
then
        mkdir -v /tmp/guesanumber # Create directory
fi

# File check
if ! [ -f /tmp/guesanumber/main.c ]
then
        cp -v main.c /tmp/guesanumber # Copy file
else
        rm -v /tmp/guesanumber/main.c # Remove old file
        cp -v main.c /tmp/guesanumber # Copy file
fi

# Directory check
if ! [ -d release ]
then
        mkdir -v release # Create directory
fi

# Archive check
if ! [ -f release/guesanumber.tar.gz ]
then
        tar -czvf release/guesanumber.tar.gz /tmp/guesanumber # Create archive
else
        rm -v release/guesanumber.tar.gz # Remove old archive
        tar -czvf release/guesanumber.tar.gz /tmp/guesanumber # Create archive
fi
