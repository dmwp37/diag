#!/bin/bash

CUR_PATH=$(dirname $0)

SEC_NAME="I2C Test"
OPCODE=0010

echo "#-----------------------$SEC_NAME-----------------------"

#Define array for command and description: following Diag and DVT Spec

#command array
array_command[0]=01000102
array_command[1]=0ffd00000000
array_command[2]=$OPCODE""0000510210
array_command[3]=$OPCODE""01005302020103
array_command[4]=0ffd00000001

#command description array, need match with command array above.
array_des[0]="Suspend the DUT"
array_des[1]="Enable Data Dump"
array_des[2]="Read I2C Bus"
array_des[3]="Write I2C Bus"
array_des[4]="Disable Data Dump"

array_command_fail[0]=$OPCODE""0001510210

array_des_fail[0]="Suspend invalid Bus"

. $CUR_PATH/diag_verify.sh

diag_verify true array_command[@] array_des[@]

diag_verify false array_command_fail[@] array_des_fail[@]

echo -e '\n\n'
echo "#-------------------$SEC_NAME passed!-------------------"
echo -e '\n\n'
exit 0
