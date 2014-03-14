#!/bin/bash

CUR_PATH=$(dirname $0)

SEC_NAME="FPGA_FLASH Test"
OPCODE=0018

GET_INF_ACTION=00
GET_CFI_ACTION=01
GET_MEM_ACTION=02
SET_MEM_ACTION=03
ADDR=ABCD0000
LEN=10
MEM=01020304010203040102030401020304

echo "#-----------------------$SEC_NAME-----------------------"

#Define array for command and description: following Diag and DVT Spec

#command array
array_command[0]=01000102
array_command[1]="$OPCODE""$GET_INF_ACTION"
array_command[2]="$OPCODE""$GET_CFI_ACTION"
array_command[3]="$OPCODE""$GET_MEM_ACTION""$ADDR""$LEN"
array_command[4]="$OPCODE""$SET_MEM_ACTION""$ADDR""$LEN""$MEM"

#command description array, need match with command array above.
array_des[0]="Suspend the DUT"
array_des[1]="Get information"
array_des[2]="Get CFI query"
array_des[3]="Get memory array"
array_des[4]="Set memory array"

array_command_fail[0]="$OPCODE"04
array_des_fail[0]="Read invalid action"

. $CUR_PATH/diag_verify.sh

diag_verify true array_command[@] array_des[@]

diag_verify false array_command_fail[@] array_des_fail[@]

echo -e '\n\n'
echo "#-------------------$SEC_NAME passed!-------------------"
echo -e '\n\n'
exit 0
