#!/bin/bash

CUR_PATH=$(dirname $0)

SEC_NAME="Normal Loopback Test"
OPCODE=0023

TX_PORT=01
RX_PORT=04
SIZE=0400
PATTERN=AA
NUM=0020

echo "#-----------------------$SEC_NAME-----------------------"

#Define array for command and description: following Diag and DVT Spec

#command array
array_command[0]=01000102
array_command[1]="$OPCODE""$TX_PORT""$RX_PORT""$SIZE""$PATTERN""$NUM"

#command description array, need match with command array above.
array_des[0]="Suspend the DUT"
array_des[1]="normal loopback test"

RX_PORT=40
array_command_fail[0]="$OPCODE""$TX_PORT""$RX_PORT""$SIZE""$PATTERN""$NUM"

array_des_fail[0]="Invalide Port"

. $CUR_PATH/diag_verify.sh

diag_verify true array_command[@] array_des[@]

diag_verify false array_command_fail[@] array_des_fail[@]

echo -e '\n\n'
echo "#-------------------$SEC_NAME passed!-------------------"
echo -e '\n\n'
exit 0
