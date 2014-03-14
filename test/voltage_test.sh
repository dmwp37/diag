#!/bin/bash

CUR_PATH=$(dirname $0)

SEC_NAME="Voltage Test"
OPCODE=0005

GET_ACTION=00
SET_ACTION=01

FEB_CHIP_0=02
CHANNEL=04

SYS_VOL_LEVEL=01

echo "#-----------------------$SEC_NAME-----------------------"

#Define array for command and description: following Diag and DVT Spec

#command array
array_command[0]=01000102
array_command[1]="$OPCODE""$GET_ACTION""$FEB_CHIP_0""$CHANNEL"
array_command[2]="$OPCODE""$SET_ACTION""$SYS_VOL_LEVEL"

#command description array, need match with command array above.
array_des[0]="Suspend the DUT"
array_des[1]="Get FEB CHIP 0 CHANNEL $CHANNEL voltage"
array_des[2]="Set system voltage level"

array_command_fail[0]="$OPCODE"03

INVALID_CHIP=0f
array_command_fail[1]="$OPCODE""$GET_ACTION""$INVALID_CHIP""$CHANNEL"

INVALID_CHANNEL=0f
array_command_fail[2]="$OPCODE""$GET_ACTION""$FEB_CHIP_0""$INVALID_CHANNEL"

array_des_fail[0]="Invalid action"
array_des_fail[1]="Invalid voltage chip"
array_des_fail[2]="Invalid voltage channel"


. $CUR_PATH/diag_verify.sh

diag_verify true array_command[@] array_des[@]

diag_verify false array_command_fail[@] array_des_fail[@]

echo -e '\n\n'
echo "#-------------------$SEC_NAME passed!-------------------"
echo -e '\n\n'
exit 0
