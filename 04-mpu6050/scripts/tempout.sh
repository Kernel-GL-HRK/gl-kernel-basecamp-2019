#! /bin/bash

PWR_MGMT_1="0x6b"

TEMP_OUT_H="0x41"
TEMP_OUT_L="0x42"

i2cset -y 0 0x68 $PWR_MGMT_1 0x00

while :
do
	RES_H=$(i2cget -y 0 0x68 $TEMP_OUT_H)
	RES_L=$(i2cget -y 0 0x68 $TEMP_OUT_L)

	SIGN_BIT=$(( $RES_H >> 7 ))

	RES=$((RES_H << 8 | RES_L))

	if [ $SIGN_BIT == 1 ]
	then
		RES=$(( -(0x10000 - RES) ))
	fi

	RES_CELCIUM=$(echo "scale=3; $RES / 340.000 + 36.53" | bc)

	printf "Celcium:\t%.3f\n" "$RES_CELCIUM"

	RES_FAHRENHEIT=$(echo "scale=3; ($RES_CELCIUM * 9.0 / 5.0) + 32.0" | bc)

	printf "Fahrenheit:\t%.3f\n\n" "$RES_FAHRENHEIT"

	sleep 1
done
