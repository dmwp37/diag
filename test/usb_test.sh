#!/bin/bash

CUR_PATH=$(dirname $0)

SEC_NAME="USB Test"
OPCODE=0011

RD_INFO_ACTION=00
PORT=00

echo "#-----------------------$SEC_NAME-----------------------"

#Define array for command and description: following Diag and DVT Spec

#command array
array_command[0]="$OPCODE""$RD_INFO_ACTION""$PORT"

#command description array, need match with command array above.
array_des[0]="Read USB info=$PORT"

PORT=03
array_command_fail[0]="$OPCODE""$RD_INFO_ACTION""$PORT"
array_des_fail[0]="Read invalid port "$PORT

. $CUR_PATH/diag_verify.sh

diag_verify true array_command[@] array_des[@]

diag_verify false array_command_fail[@] array_des_fail[@]

echo -e '\n\n'
echo "#-------------------$SEC_NAME passed!-------------------"
echo -e '\n\n'
exit 0
