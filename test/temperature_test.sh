#!/bin/bash

CUR_PATH=$(dirname $0)

SEC_NAME="Temperature Test"
OPCODE=0003

GET_ACTION=00

TEMP_CPU=04

echo "#-----------------------$SEC_NAME-----------------------"

#Define array for command and description: following Diag and DVT Spec

#command array
array_command[0]="$OPCODE""$GET_ACTION""$TEMP_CPU"

#command description array, need match with command array above.
array_des[0]="Get CPU Temperature"

array_command_fail[0]="$OPCODE""01""$TEMP_CPU"

TEMP_SENSOR=0f
array_command_fail[1]="$OPCODE""$GET_ACTION""$TEMP_SENSOR"

array_des_fail[0]="Invalid action"
array_des_fail[1]="Invalid temp sensor"


. $CUR_PATH/diag_verify.sh

diag_verify true array_command[@] array_des[@]

diag_verify false array_command_fail[@] array_des_fail[@]

echo -e '\n\n'
echo "#-------------------$SEC_NAME passed!-------------------"
echo -e '\n\n'
exit 0
