#! /bin/bash

PWR_MGMT_1="0x6b"

TEMP_OUT_H="0x41"
TEMP_OUT_L="0x42"

i2cset -y 0 0x68 $PWR_MGMT_1 0x00

while :
do
	RES_H=$(i2cget -y 0 0x68 $TEMP_OUT_H)
	RES_L=$(i2cget -y 0 0x68 $TEMP_OUT_L)

	RES=$((RES_H << 8 | RES_L))
done
