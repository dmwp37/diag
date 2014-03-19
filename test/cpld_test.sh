#!/bin/bash

CUR_PATH=$(dirname $0)

SEC_NAME="CPLD Test"
OPCODE=000E

GET_ACTION=00
SET_ACTION=01

CPLD_ID=00
CPLD_OFFSET=0005

echo "#-----------------------$SEC_NAME-----------------------"

#Define array for command and description: following Diag and DVT Spec

#command array
array_command[0]=01000102
array_command[1]="$OPCODE""$GET_ACTION""$CPLD_ID""$CPLD_OFFSET"
array_command[2]="$OPCODE""$SET_ACTION""$CPLD_ID""$CPLD_OFFSET"33

#command description array, need match with command array above.
array_des[0]="Suspend the DUT"
array_des[1]="Get CPLD $CPLD_ID $CPLD_OFFSET value"
array_des[2]="Set CPLD $CPLD_ID $CPLD_OFFSET value"

array_command_fail[0]="$OPCODE""02""$CPLD_ID""$CPLD_OFFSET"
array_command_fail[1]="$OPCODE""$GET_ACTION""02""$CPLD_OFFSET"

array_des_fail[0]="Invalid action"
array_des_fail[1]="Invalid CPLD ID"


. $CUR_PATH/diag_verify.sh

diag_verify true array_command[@] array_des[@]

diag_verify false array_command_fail[@] array_des_fail[@]

echo -e '\n\n'
echo "#-------------------$SEC_NAME passed!-------------------"
echo -e '\n\n'
exit 0
