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
        mkdir -v release
fi
