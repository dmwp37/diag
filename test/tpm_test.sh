#!/bin/bash

CUR_PATH=$(dirname $0)

SEC_NAME="TPM Test"
OPCODE=0014

GET_CAPABILITY_ACTION=00
GET_RESULT_ACTION=01
GET_PCR_ACTION=02
GET_COUNTER_ACTION=03

echo "#-----------------------$SEC_NAME-----------------------"

#Define array for command and description: following Diag and DVT Spec

#command array
array_command[0]="$OPCODE""$GET_CAPABILITY_ACTION"
array_command[1]="$OPCODE""$GET_RESULT_ACTION"
array_command[2]="$OPCODE""$GET_PCR_ACTION"00
array_command[3]="$OPCODE""$GET_COUNTER_ACTION"01

#command description array, need match with command array above.
array_des[0]="Get TPM Capability"
array_des[1]="Get TPM Test result"
array_des[2]="Get TPM PCR"
array_des[3]="Get TPM Counter"

array_command_fail[0]="$OPCODE"04
array_des_fail[0]="TPM test fail, invalid action"

. $CUR_PATH/diag_verify.sh

diag_verify true array_command[@] array_des[@]

diag_verify false array_command_fail[@] array_des_fail[@]

echo -e '\n\n'
echo "#-------------------$SEC_NAME passed!-------------------"
echo -e '\n\n'
exit 0
