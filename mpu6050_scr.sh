#!/bin/bash

Power="0x6B"
TEMP_OUT_H="0x41"
TEMP_OUT_L="0x42"
MPU6050="0x68"
WHO_AM_I="0x75"

echo "Probing for mpu6050..."
tmp=$(i2cget -y 1 $MPU6050 $WHO_AM_I)
sleep 0.2


#echo "mpu" $MPU6050
if [ "$tmp" != "$MPU6050" ]
	then echo "MPU6050 not found"
else  printf 'Device answer is OK and = %#x\n' $tmp 
fi

sleep 0.2

#Set device online
i2cset -y 1 $MPU6050 $Power 0

sleep 0.2

#Run cycle polling
for (( a = 1; a<=10; a++ ))
do 
#Request TEMP data
H=$(i2cget -y 1 $MPU6050 $TEMP_OUT_H)
sleep 0.2
L=$(i2cget -y 1 $MPU6050 $TEMP_OUT_L)
echo "Temp H_byte ->" $H
echo "Temp L_byte ->"  $L
#Convert to 
printf -v temp '%+d' $(($H <<8 |$L))
#convert 16 bit unsigned to signed
temp_converted=$(((temp & 0x7fff)- (temp & 0x8000)))
#Convert temperature to 'C
CELS=$(echo "scale = 3; $temp_converted /340 +36.53" |bc)
printf 'CELSIUM temperature is %.3f\n' $CELS
#Convert temperature to 'F
Fahr=$(echo "scale=3; ($CELS * 9 / 5) + 32" | bc)
printf 'Fahrenheit temperature is %.3f\n' $Fahr
sleep 1
done
