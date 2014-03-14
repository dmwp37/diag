#!/bin/bash

CUR_PATH=$(dirname $0)

SEC_NAME="SSD Test"
OPCODE=0012

GET_STATUS_ACTION=00
GET_MODEL_ACTION=01
GET_SERIAL_ACTION=02
GET_SIZE_ACTION=03
GET_CACHE_ACTION=04
SSD=00

echo "#-----------------------$SEC_NAME-----------------------"

#Define array for command and description: following Diag and DVT Spec

#command array
array_command[0]="$OPCODE""$GET_STATUS_ACTION""$SSD"
array_command[1]="$OPCODE""$GET_MODEL_ACTION""$SSD"
array_command[2]="$OPCODE""$GET_SERIAL_ACTION""$SSD"
array_command[3]="$OPCODE""$GET_SIZE_ACTION""$SSD"
array_command[4]="$OPCODE""$GET_CACHE_ACTION""$SSD"

#command description array, need match with command array above.
array_des[0]="Get SSD status"
array_des[1]="Get SSD Model Number"
array_des[2]="Get SSD Serial Number"
array_des[3]="Get SSD device size"
array_des[4]="Get SSD cache/buffer size"

array_command_fail[0]="$OPCODE"05"$SSD"
array_command_fail[1]="$OPCODE""$GET_STATUS_ACTION"02
array_des_fail[0]="Invalid action"
array_des_fail[1]="Invalid SSD "$SSD

. $CUR_PATH/diag_verify.sh

diag_verify true array_command[@] array_des[@]

diag_verify false array_command_fail[@] array_des_fail[@]

echo -e '\n\n'
echo "#-------------------$SEC_NAME passed!-------------------"
echo -e '\n\n'
exit 0
