#!/bin/bash

TEMP=/tmp/guesanumber

if [ ! -d "${TEMP}" ]
then
	mkdir "${TEMP}"
	find . -type f -name '*.c' -exec cp {} "${TEMP}" \;
	
else
	echo "The directory /tmp/guesanumber already exists"
fi

