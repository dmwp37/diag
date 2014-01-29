#!/bin/bash

CUR_PATH=$(dirname $0)

SEC_NAME="LED Test"
OPCODE=0001

ENABLE=01
DISABLE=00

POWER_LED=00
STATUS_LED=01

DEFAULT_COLOR=00

echo "#-----------------------$SEC_NAME-----------------------"

#Define array for command and description: following Diag and DVT Spec

#command array
array_command[0]="$OPCODE""$ENABLE""$POWER_LED""$DEFAULT_COLOR"
array_command[1]="$OPCODE""$ENABLE""$STATUS_LED""$DEFAULT_COLOR"
array_command[2]="$OPCODE""$DISABLE""$POWER_LED"
array_command[3]="$OPCODE""$DISABLE""$STATUS_LED"

#command description array, need match with command array above.
array_des[0]="Enable Power LED"
array_des[1]="Enable Status LED"
array_des[2]="Disable Power LED"
array_des[3]="Disable Status LED"

array_command_fail[0]="$OPCODE""$DISABLE"ff
array_des_fail[0]="LED invalid led type"

. $CUR_PATH/diag_verify.sh

diag_verify true array_command[@] array_des[@]

diag_verify false array_command_fail[@] array_des_fail[@]

echo -e '\n\n'
echo "#-------------------$SEC_NAME passed!-------------------"
echo -e '\n\n'
exit 0
