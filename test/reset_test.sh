#!/bin/bash

CUR_PATH=$(dirname $0)

SEC_NAME="RESET Test"
OPCODE=0101

RESET_SYSTEM=00
RESET_ACTION=01
RECOVER_ACTION=02

CHIP_ID=01 #FPGA

echo "#-----------------------$SEC_NAME-----------------------"

#Define array for command and description: following Diag and DVT Spec

#command array
array_command[0]=01000102
array_command[1]="$OPCODE""$RESET_ACTION""$CHIP_ID"
array_command[2]="$OPCODE""$RECOVER_ACTION""$CHIP_ID"

#command description array, need match with command array above.
array_des[0]="Suspend the DUT"
array_des[1]="Reset Chip=$CHIP_ID"
array_des[2]="Recover Chip=$CHIP_ID"

CHIP_ID=0f
array_command_fail[0]="$OPCODE""$RESET_ACTION""$CHIP_ID"
array_command_fail[1]="$OPCODE""$RESET_SYSTEM"

array_des_fail[0]="Reset invalid chip. Chip=$CHIP_ID"
array_des_fail[1]="Reset system not support"

. $CUR_PATH/diag_verify.sh

diag_verify true array_command[@] array_des[@]

diag_verify false array_command_fail[@] array_des_fail[@]

echo -e '\n\n'
echo "#-------------------$SEC_NAME passed!-------------------"
echo -e '\n\n'
exit 0
