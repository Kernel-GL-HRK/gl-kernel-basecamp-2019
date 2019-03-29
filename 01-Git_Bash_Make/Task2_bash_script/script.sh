#!/bin/bash

TEMP=/tmp/guesanumber

if [ ! -d "${TEMP}" ]
then
	mkdir "${TEMP}"
	find . -type f -name '*.c' -exec cp {} "${TEMP}" \;
	mkdir ./release
	tar -cvzf ./release/Task1_game.tar.gz "${TEMP}"
	rm -rf "${TEMP}"
	
else
	echo "The directory $TEMP already exists"
fi

