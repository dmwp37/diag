#!/bin/bash

CUR_PATH=$(dirname $0)

SEC_NAME="PTTM Test"
OPCODE=0009

GET_ACTION=00
SET_ACTION=01

PTTM_CHIP=01

RDAC_VALUE=1f

echo "#-----------------------$SEC_NAME-----------------------"

#Define array for command and description: following Diag and DVT Spec

#command array
array_command[0]=01000102
array_command[1]="$OPCODE""$GET_ACTION""$PTTM_CHIP"
array_command[2]="$OPCODE""$SET_ACTION""$PTTM_CHIP""$RDAC_VALUE"
array_command[3]="$OPCODE""$GET_ACTION""$PTTM_CHIP"

#command description array, need match with command array above.
array_des[0]="Suspend the DUT"
array_des[1]="Get PTTM $PTTM_CHIP RDAC"
array_des[2]="Set PTTM $PTTM_CHIP RDAC"
array_des[3]="Get PTTM $PTTM_CHIP RDAC"

array_command_fail[0]="$OPCODE""04""$PTTM_CHIP"

PTTM_CHIP=04
array_command_fail[1]="$OPCODE""$GET_ACTION""$PTTM_CHIP"

PTTM_CHIP=00
RDAC_VALUE=f0
array_command_fail[2]="$OPCODE""$SET_ACTION""$PTTM_CHIP""$RDAC_VALUE"

array_des_fail[0]="Invalid action"
array_des_fail[1]="Invalid chip"
array_des_fail[2]="Invalid set value"


. $CUR_PATH/diag_verify.sh

diag_verify true array_command[@] array_des[@]

diag_verify false array_command_fail[@] array_des_fail[@]

echo -e '\n\n'
echo "#-------------------$SEC_NAME passed!-------------------"
echo -e '\n\n'
exit 0
