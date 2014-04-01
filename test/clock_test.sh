#!/bin/bash

CUR_PATH=$(dirname $0)

SEC_NAME="Clock Test"
OPCODE=0007

GET_ACTION=00
SET_ACTION=01

CLOCK_REG=01

REG_VAL=1f

echo "#-----------------------$SEC_NAME-----------------------"

#Define array for command and description: following Diag and DVT Spec

#command array
array_command[0]=01000102
array_command[1]="$OPCODE""$GET_ACTION""$CLOCK_REG"

#command description array, need match with command array above.
array_des[0]="Suspend the DUT"
array_des[1]="Get CLock Register $CLOCK_REG"

array_command_fail[0]="$OPCODE""04""$CLOCK_REG"

CLOCK_REG=14
array_command_fail[1]="$OPCODE""$GET_ACTION""$CLOCK_REG"

array_des_fail[0]="Invalid action"
array_des_fail[1]="Invalid clock register"


. $CUR_PATH/diag_verify.sh

diag_verify true array_command[@] array_des[@]

diag_verify false array_command_fail[@] array_des_fail[@]

echo -e '\n\n'
echo "#-------------------$SEC_NAME passed!-------------------"
echo -e '\n\n'
exit 0
