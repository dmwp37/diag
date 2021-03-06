#!/bin/bash

CUR_PATH=$(dirname $0)

SEC_NAME="I2C Test"
OPCODE=0010

RD_ACTION=00
WR_ACTION=01

PCH_SMB=00
ADDRESS=50
RD_OFFSET=00
RD_LEN=ff

WR_OFFSET=fd
WR_LEN=02
WR_DATA1=0102
WR_DATA2=ffff


echo "#-----------------------$SEC_NAME-----------------------"

#Define array for command and description: following Diag and DVT Spec

#command array
array_command[0]=01000102
array_command[1]=0ffd010000
array_command[2]="$OPCODE""$RD_ACTION""$PCH_SMB""$ADDRESS""$RD_OFFSET""$RD_LEN"
array_command[3]="$OPCODE""$WR_ACTION""$PCH_SMB""$ADDRESS""$WR_OFFSET""$WR_LEN""$WR_DATA1"
array_command[4]="$OPCODE""$RD_ACTION""$PCH_SMB""$ADDRESS""$RD_OFFSET""$RD_LEN"
array_command[5]="$OPCODE""$WR_ACTION""$PCH_SMB""$ADDRESS""$WR_OFFSET""$WR_LEN""$WR_DATA2"
array_command[6]="$OPCODE""$RD_ACTION""$PCH_SMB""$ADDRESS""$RD_OFFSET""$RD_LEN"
array_command[7]=0ffd010001

#command description array, need match with command array above.
array_des[0]="Suspend the DUT"
array_des[1]="Enable Data Dump"
array_des[2]="Read I2C Bus=$PCH_SMB Address=$ADDRESS Offset=$RD_OFFSET Len=$RD_LEN"
array_des[3]="Write I2C Bus=$PCH_SMB Address=$ADDRESS Offset=$WR_OFFSET Len=$WR_LEN Data=$WR_DATA1"
array_des[4]="Read I2C Bus=$PCH_SMB Address=$ADDRESS Offset=$RD_OFFSET Len=$RD_LEN"
array_des[5]="Write I2C Bus=$PCH_SMB Address=$ADDRESS Offset=$WR_OFFSET Len=$WR_LEN Data=$WR_DATA2"
array_des[6]="Read I2C Bus=$PCH_SMB Address=$ADDRESS Offset=$RD_OFFSET Len=$RD_LEN"
array_des[7]="Disable Data Dump"

array_command_fail[0]="$OPCODE""$RD_ACTION""08""$ADDRESS""$RD_OFFSET""$RD_LEN"
array_des_fail[0]="Read invalid Bus"

. $CUR_PATH/diag_verify.sh

diag_verify true array_command[@] array_des[@]

diag_verify false array_command_fail[@] array_des_fail[@]

echo -e '\n\n'
echo "#-------------------$SEC_NAME passed!-------------------"
echo -e '\n\n'
exit 0
