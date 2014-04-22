#!/bin/bash

CUR_PATH=$(dirname $0)

SEC_NAME="Voltage Test"
OPCODE=0005

GET_ACTION=00
SET_ACTION=01

CPU_CHIP_0=01
CHANNEL=04

SYS_VOL_LEVEL=01

echo "#-----------------------$SEC_NAME-----------------------"

#Define array for command and description: following Diag and DVT Spec

#command array
COUNTER=0
array_command[$((COUNTER++))]=01000102
array_command[$((COUNTER++))]="$OPCODE""$GET_ACTION""$CPU_CHIP_0""$CHANNEL"
array_command[$((COUNTER++))]="$OPCODE""$SET_ACTION""$SYS_VOL_LEVEL"

#command description array, need match with command array above.
COUNTER=0
array_des[$((COUNTER++))]="Suspend the DUT"
array_des[$((COUNTER++))]="Get CPU CHIP 0 CHANNEL $CHANNEL voltage"
array_des[$((COUNTER++))]="Set system voltage level"

COUNTER=0
array_command_fail[$((COUNTER++))]="$OPCODE"03

INVALID_CHIP=0f
array_command_fail[$((COUNTER++))]="$OPCODE""$GET_ACTION""$INVALID_CHIP""$CHANNEL"

INVALID_CHANNEL=0f
array_command_fail[$((COUNTER++))]="$OPCODE""$GET_ACTION""$CPU_CHIP_0""$INVALID_CHANNEL"

COUNTER=0
array_des_fail[$((COUNTER++))]="Invalid action"
array_des_fail[$((COUNTER++))]="Invalid voltage chip"
array_des_fail[$((COUNTER++))]="Invalid voltage channel"


. $CUR_PATH/diag_verify.sh

diag_verify true array_command[@] array_des[@]

diag_verify false array_command_fail[@] array_des_fail[@]

echo -e '\n\n'
echo "#-------------------$SEC_NAME passed!-------------------"
echo -e '\n\n'
exit 0
