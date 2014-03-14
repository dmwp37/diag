#!/bin/bash

CUR_PATH=$(dirname $0)

SEC_NAME="BIOS Test"
OPCODE=0015

READ_INFO_ACTION=00
READ_STATUS_ACTION=01
WRITE_STATUS_ACTION=02
GET_CHECKSUM_ACTION=03
ATTACK_ACTION=04
SET_SRC_ACTION=05
GET_SRC_ACTION=06
BIOS_ID=00
REG_STATUS=1234

echo "#-----------------------$SEC_NAME-----------------------"

#Define array for command and description: following Diag and DVT Spec

#command array
array_command[0]=01000102
array_command[1]="$OPCODE""$READ_INFO_ACTION""$BIOS_ID"
array_command[2]="$OPCODE""$READ_STATUS_ACTION""$BIOS_ID"
array_command[3]="$OPCODE""$WRITE_STATUS_ACTION""$BIOS_ID""$REG_STATUS"
array_command[4]="$OPCODE""$GET_CHECKSUM_ACTION""$BIOS_ID"
array_command[5]="$OPCODE""$ATTACK_ACTION""$BIOS_ID"
array_command[6]="$OPCODE""$SET_SRC_ACTION""$BIOS_ID"
array_command[7]="$OPCODE""$GET_SRC_ACTION"

#command description array, need match with command array above.
array_des[0]="Suspend the DUT"
array_des[1]="Read BIOS info"
array_des[2]="Read BIOS status"
array_des[3]="Write BIOS status"
array_des[4]="Get BIOS checksum"
array_des[5]="Attack BIOS"
array_des[6]="Set BIOS src"
array_des[7]="Get BIOS src"

BIOS_ID=02
array_command_fail[0]="$OPCODE""$READ_INFO_ACTION""$BIOS_ID"
array_des_fail[0]="Read invalid BIOS ID "$BIOS_ID

. $CUR_PATH/diag_verify.sh

diag_verify true array_command[@] array_des[@]

diag_verify false array_command_fail[@] array_des_fail[@]

echo -e '\n\n'
echo "#-------------------$SEC_NAME passed!-------------------"
echo -e '\n\n'
exit 0
