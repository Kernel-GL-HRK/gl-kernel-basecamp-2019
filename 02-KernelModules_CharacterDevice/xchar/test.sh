#!/bin/bash

DRIVER_NAME=xchar
PROC_USED_BUFFER_VOLUME=used_buffer_volume
PROC_BUFFER_SIZE=buffer_size
SYS_CLEAN_UP_BUFFER=clean_up_buffer

COL_BLACK="30"
COL_RED="31"
COL_GREEN="32"
COL_BROWN="33"
COL_BLUE="34"
COL_PURPLE="35"
COL_CYAN="36"
COL_LIGHT_GRAY="37"
COL_DARK_GRAY="1;30"
COL_LIGHT_RED="1;31"
COL_LIGHT_GREEN="1;32"
COL_YELLOW="1;33"
COL_LIGHT_BLUE="1;34"
COL_LIGHT_PURPLE="1;35"
COL_LIGHT_CYAN="1;36"
COL_WHITE="1;37"

if [[ "$1" != "--nocolor" ]]; then
    COLORIZE=1
fi

COL_FATAL=$COL_LIGHT_RED
COL_ERROR=$COL_RED
COL_WARNING=$COL_WHITE
COL_INFO=$COL_CYAN
COL_DATA=$COL_LIGHT_GRAY

COUNTER=0

_log() {
    if [[ $# != 2 ]]; then
        _log $COL_ERROR "ERROR. Use: _log COLOR MESSAGE"
    elif [[ $COLORIZE ]]; then
        echo -e "\e[$1m$2\e[0m"
    else
        echo $2
    fi
}

_check_fatal() {
    if [[ $? != 0 ]]; then
        _log $COL_FATAL "Command failed."
        exit -1
    fi
    _log $COL_INFO "Ok."
}

_check_error() {
    if [[ $? != 0 ]]; then
        _log $COL_ERROR "Command failed."
    else
        _log $COL_INFO "Ok."
    fi
}

_check_actual() {
    if [[ $? != 0 ]]; then
        _log $COL_ERROR "Command failed."
    elif [[ $# != 2 ]]; then
        _log $COL_ERROR "Use: _check_actual EXPECTED ACTUAL"
    elif [[ $1 == $2 ]]; then
        _log $COL_DATA "\"$2\""
        _log $COL_INFO "Ok."
    else
        _log $COL_ERROR "Actual is \"$2\" but \"$1\" expected."
    fi
}

_log $COL_WARNING "$(( ++COUNTER )). Install module..."
insmod ./${DRIVER_NAME}.ko
_check_fatal
echo

CHECKING=/dev/${DRIVER_NAME}0
DATA=12345
_log $COL_WARNING "$(( ++COUNTER )). Write \"$DATA\" to \"$CHECKING\"..."
echo $DATA > $CHECKING
_check_error
echo

CHECKING=/proc/${DRIVER_NAME}/${PROC_USED_BUFFER_VOLUME}
_log $COL_WARNING "$(( ++COUNTER )). Checking \"$CHECKING\"..."
EXPECTED=6
ACTUAL=$(cat $CHECKING)
_check_actual "$EXPECTED" "$ACTUAL"
echo

CHECKING=/proc/${DRIVER_NAME}/${PROC_BUFFER_SIZE}
_log $COL_WARNING "$(( ++COUNTER )). Checking \"$CHECKING\"..."
EXPECTED=1024
ACTUAL=$(cat $CHECKING)
_check_actual "$EXPECTED" "$ACTUAL"
echo

CHECKING=/dev/${DRIVER_NAME}0
_log $COL_WARNING "$(( ++COUNTER )). Checking \"$CHECKING\"..."
EXPECTED=12345
ACTUAL=$(cat $CHECKING)
_check_actual "$EXPECTED" "$ACTUAL"
echo

CHECKING=/dev/${DRIVER_NAME}0
DATA=abc
_log $COL_WARNING "$(( ++COUNTER )). Write \"$DATA\" to \"$CHECKING\"..."
echo $DATA > $CHECKING
_check_error
echo

CHECKING=/proc/${DRIVER_NAME}/${PROC_USED_BUFFER_VOLUME}
_log $COL_WARNING "$(( ++COUNTER )). Checking \"$CHECKING\"..."
EXPECTED=4
ACTUAL=$(cat $CHECKING)
_check_actual "$EXPECTED" "$ACTUAL"
echo

CHECKING=/sys/class/${DRIVER_NAME}/${SYS_CLEAN_UP_BUFFER}
DATA=1
_log $COL_WARNING "$(( ++COUNTER )). Write \"$DATA\" to \"$CHECKING\"..."
echo $DATA > $CHECKING
_check_error
echo

CHECKING=/proc/${DRIVER_NAME}/${PROC_USED_BUFFER_VOLUME}
_log $COL_WARNING "$(( ++COUNTER )). Checking \"$CHECKING\"..."
EXPECTED=0
ACTUAL=$(cat $CHECKING)
_check_actual "$EXPECTED" "$ACTUAL"
echo

CHECKING=/dev/${DRIVER_NAME}0
_log $COL_WARNING "$(( ++COUNTER )). Checking \"$CHECKING\"..."
EXPECTED=""
ACTUAL=$(cat $CHECKING)
_check_actual "$EXPECTED" "$ACTUAL"
echo

_log $COL_WARNING "$(( ++COUNTER )). Remove module..."
rmmod ${DRIVER_NAME}
_check_fatal
echo

DATA=512
_log $COL_WARNING "$(( ++COUNTER )). Install module with parameters..."
insmod ./${DRIVER_NAME}.ko buffer_size=$DATA
_check_fatal
echo

CHECKING=/proc/${DRIVER_NAME}/${PROC_BUFFER_SIZE}
_log $COL_WARNING "$(( ++COUNTER )). Checking \"$CHECKING\"..."
EXPECTED=$DATA
ACTUAL=$(cat $CHECKING)
_check_actual "$EXPECTED" "$ACTUAL"
echo

_log $COL_WARNING "$(( ++COUNTER )). Remove module..."
rmmod ${DRIVER_NAME}
_check_fatal
echo
