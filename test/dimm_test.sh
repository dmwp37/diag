#!/bin/bash

CUR_PATH=$(dirname $0)

SEC_NAME="DIMM Test"
OPCODE=0008

GET_SPD_ACTION=00

SLOT0=00
SLOT1=01
SLOT2=02
SLOT3=03

echo "#-----------------------$SEC_NAME-----------------------"

#Define array for command and description: following Diag and DVT Spec

#command array
array_command[0]="$OPCODE""$GET_SPD_ACTION""$SLOT0"
array_command[1]="$OPCODE""$GET_SPD_ACTION""$SLOT1"
array_command[2]="$OPCODE""$GET_SPD_ACTION""$SLOT2"
array_command[3]="$OPCODE""$GET_SPD_ACTION""$SLOT3"

#command description array, need match with command array above.
array_des[0]="Get SLOT0 SPD date"
array_des[1]="Get SLOT1 SPD date"
array_des[2]="Set SLOT2 SPD date"
array_des[3]="Get SLOT3 SPD date"

array_command_fail[0]="$OPCODE"01
array_command_fail[1]="$OPCODE""$GET_SPD_ACTION"04
array_des_fail[0]="Invalid action"
array_des_fail[1]="Invalid slot"

. $CUR_PATH/diag_verify.sh

diag_verify true array_command[@] array_des[@]

diag_verify false array_command_fail[@] array_des_fail[@]

echo -e '\n\n'
echo "#-------------------$SEC_NAME passed!-------------------"
echo -e '\n\n'
exit 0
