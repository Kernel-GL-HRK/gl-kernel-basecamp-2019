#!/bin/bash

TEMP=/tmp/guesanumber

if [ ! -d "${TEMP}" ]
then
	mkdir "${TEMP}"	
else
	echo "The directory /tmp/guesanumber already exists"
fi

