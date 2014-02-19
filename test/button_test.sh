#!/bin/bash

CUR_PATH=$(dirname $0)

SEC_NAME="BUTTON Test"
OPCODE=000F

GET_ACTION=00

echo "#-----------------------$SEC_NAME-----------------------"

#Define array for command and description: following Diag and DVT Spec

#command array
array_command[0]=01000102
array_command[2]="$OPCODE""$GET_ACTION"
array_command[3]="$OPCODE""$GET_ACTION"

#command description array, need match with command array above.
array_des[0]="Suspend the DUT"
array_des[1]="Get last pressed button"
array_des[2]="Get last pressed button"

array_command_fail[0]=$OPCODE""03
array_des_fail[0]="Invalid action"

. $CUR_PATH/diag_verify.sh

diag_verify true array_command[@] array_des[@]

diag_verify false array_command_fail[@] array_des_fail[@]

echo -e '\n\n'
echo "#-------------------$SEC_NAME passed!-------------------"
echo -e '\n\n'
exit 0
