#!/bin/bash
DEV_REG="0x68"
PWR_REG="0X6B"
LTMP_REG="0x41"
HTMP_REG="0x42"

i2cset -y 0 $DEV_REG $PWR_REG 0
sleep 0.5
while :
do
	HIGH=$(i2cget -y 0 $DEV_REG $LTMP_REG)
	LOW=$(i2cget -y 0 $DEV_REG $HTMP_REG)
	VALUE=$(( -(0x10000 -((HIGH<<8)|LOW) )))
	printf "°C:"
	bc -l <<< "scale=3; $VALUE/340 + 36.53" 
	sleep 0.5
done
