#! /bin/bash

ON=0x00
OFF=0x40
POWER=0xeb
DEVICE=0x68
DEVICE_ID=0

help()
{
        printf "Usage: $0 [OPTION] ...\n"
        printf "Script to help get the temperature from the mpu6050 device.\n\n"
        printf "List of options:\n"
        printf "  -h, --help\t\tdisplays help page.\n"
        printf "  -i, --init\t\tinitializes the device.\n"
        printf "  -d, --deactivate\tdeactivate the device\n"
        printf "  -s, --show TYPE\tdisplays the temperature in the specified TYPE on the screen.\n"
        printf "      --stream_data\tdisplays temperature data every second.\n"

        printf "\nThe TYPE argument can take values that indicate in what form the temperature is displayed.
This argument can only accept values: --show f (output in Fahrenheit) and --show c (output in Celsius).\n"

        printf "\nFor the correct execution of the script, you may need super user rights. Also MUST be installed
package і2с-tools.\n"

        printf "\nAuthor: Maxim Primerov. Email: <primerovmax@gmail.com>. License: GNUv3.\n"
}

init()
{
        sudo i2cset -y ${DEVICE_ID} ${DEVICE} ${POWER} ${ON}
}

deactivate()
{
        sudo i2cset -y ${DEVICE_ID} ${DEVICE} ${POWER} ${OFF}
}

show()
{
        MAJOR=$(sudo i2cget -y 0 0x68 0x41)
        MINOR=$(sudo i2cget -y 0 0x68 0x42)

        SING=$(( (MAJOR & 0x80) >> 7))
        TEMPERATURE=$(( (MAJOR << 8) + MINOR ))

        if [[ ${SIGN} == 1 ]]
        then
                TEMPERATURE=$(( -(0x10000 - TEMPERATURE) ))
        fi

        if [[ $1 == "f" ]]
        then
                RESULT=`bc -l <<< "scale=3; (${TEMPERATURE}/340 + 36.53) * 1.8 + 32"`
        else
                RESULT=`bc -l <<< "scale=3; ${TEMPERATURE}/340 + 36.53"`
        fi

        echo ${RESULT}
}

stream()
{
        while :
        do
                init
                printf "\nTemp: F = $(show f)\n"
                printf "Temp: C = $(show)\n"
                
                sleep 1
        done
}

if [[ $1 == "" ]]
then
        stream
        exit 0
fi

while [ -n "$1" ]
do
        case $1 in
                -h | --help) 
                        help
                        break ;;
                -i | --init)
                        init
                        break ;;
                -d | --deactivate)
                        deactivate
                        break ;;
                -s | --show) 
                        param="$2"
                        show ${param}
                        break ;;
                --stream_data)
                        stream
                        break ;;
                --) 
                        shift
                        break ;;
                *) 
                        help
                        exit -1 ;;
        esac
        shift
done

exit
