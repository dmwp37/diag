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
COUNTER=0
array_command[$((COUNTER++))]=01000102
array_command[$((COUNTER++))]=0ffd010000
array_command[$((COUNTER++))]="$OPCODE""$RD_ACTION""$PCH_SMB""$ADDRESS""$RD_OFFSET""$RD_LEN"
#array_command[$((COUNTER++))]="$OPCODE""$WR_ACTION""$PCH_SMB""$ADDRESS""$WR_OFFSET""$WR_LEN""$WR_DATA1"
#array_command[$((COUNTER++))]=0ffe0000000200000400
#array_command[$((COUNTER++))]="$OPCODE""$RD_ACTION""$PCH_SMB""$ADDRESS""$RD_OFFSET""$RD_LEN"
#array_command[$((COUNTER++))]="$OPCODE""$WR_ACTION""$PCH_SMB""$ADDRESS""$WR_OFFSET""$WR_LEN""$WR_DATA2"
#array_command[$((COUNTER++))]=0ffe0000000200000400
#array_command[$((COUNTER++))]="$OPCODE""$RD_ACTION""$PCH_SMB""$ADDRESS""$RD_OFFSET""$RD_LEN"
array_command[$((COUNTER++))]=0ffd010001

#command description array, need match with command array above.
COUNTER=0
array_des[$((COUNTER++))]="Suspend the DUT"
array_des[$((COUNTER++))]="Enable Data Dump"
array_des[$((COUNTER++))]="Read I2C Bus=$PCH_SMB Address=$ADDRESS Offset=$RD_OFFSET Len=$RD_LEN"
#array_des[$((COUNTER++))]="Write I2C Bus=$PCH_SMB Address=$ADDRESS Offset=$WR_OFFSET Len=$WR_LEN Data=$WR_DATA1"
#array_des[$((COUNTER++))]="sleep 1s"
#array_des[$((COUNTER++))]="Read I2C Bus=$PCH_SMB Address=$ADDRESS Offset=$RD_OFFSET Len=$RD_LEN"
#array_des[$((COUNTER++))]="Write I2C Bus=$PCH_SMB Address=$ADDRESS Offset=$WR_OFFSET Len=$WR_LEN Data=$WR_DATA2"
#array_des[$((COUNTER++))]="sleep 1s"
#array_des[$((COUNTER++))]="Read I2C Bus=$PCH_SMB Address=$ADDRESS Offset=$RD_OFFSET Len=$RD_LEN"
array_des[$((COUNTER++))]="Disable Data Dump"

array_command_fail[0]="$OPCODE""$RD_ACTION""08""$ADDRESS""$RD_OFFSET""$RD_LEN"
array_des_fail[0]="Read invalid Bus"

. $CUR_PATH/diag_verify.sh

diag_verify true array_command[@] array_des[@]

diag_verify false array_command_fail[@] array_des_fail[@]

echo -e '\n\n'
echo "#-------------------$SEC_NAME passed!-------------------"
echo -e '\n\n'
exit 0
