#!/bin/bash

CUR_PATH=$(dirname $0)

SEC_NAME="PCI Test"
OPCODE=0013

RD_CFG_ACTION=00
RD_MEM_ACTION=01

DOMAIN=00
BUS=00
DEV=1f
FUNC=03
OFFSET=00000001
LEN=00000006

echo "#-----------------------$SEC_NAME-----------------------"

#Define array for command and description: following Diag and DVT Spec

#command array
array_command[0]=0ffd010000
array_command[1]="$OPCODE""$RD_CFG_ACTION""$DOMAIN""$BUS""$DEV""$FUNC""$OFFSET""$LEN"
array_command[2]=0ffd010001

#command description array, need match with command array above.
array_des[0]="Enable Data Dump"
array_des[1]="Read PCI config space=$DOMAIN:$BUS:$DEV.$FUNC Offset=$OFFSET Len=$LEN"
array_des[2]="Disable Data Dump"

DOMAIN=01
array_command_fail[0]="$OPCODE""$RD_CFG_ACTION""$DOMAIN""$BUS""$DEV""$FUNC""$OFFSET""$LEN"
array_command_fail[1]="$OPCODE""$RD_MEM_ACTION""$DOMAIN""$BUS""$DEV""$FUNC""$OFFSET""$LEN"
array_des_fail[0]="Read PCI config space, Invalid parameter. $DOMAIN_INVALID:$BUS:$DEV.$FUNC Offset=$OFFSET Len=$LEN"
array_des_fail[1]="Unsupport memory space read.$DOMAIN:$BUS:$DEV.$FUNC Offset=$OFFSET Len=$LEN"

. $CUR_PATH/diag_verify.sh

diag_verify true array_command[@] array_des[@]

diag_verify false array_command_fail[@] array_des_fail[@]

echo -e '\n\n'
echo "#-------------------$SEC_NAME passed!-------------------"
echo -e '\n\n'
exit 0
