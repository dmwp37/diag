#!/bin/bash

CUR_PATH=$(dirname $0)

SEC_NAME="Internal Loopback Test"
OPCODE=0021

PORT=01
NODE=02
CFG=01
SIZE=0400
PATTERN=AA
NUM=0020

echo "#-----------------------$SEC_NAME-----------------------"

#Define array for command and description: following Diag and DVT Spec

#command array
array_command[0]=01000102
array_command[1]="$OPCODE""$PORT""$NODE""$CFG""$SIZE""$PATTERN""$NUM"

#command description array, need match with command array above.
array_des[0]="Suspend the DUT"
array_des[1]="Ping MGT PHY node"

PORT=40
array_command_fail[0]="$OPCODE""$PORT""$NODE""$CFG""$SIZE""$PATTERN""$NUM"
PORT=01
NODE=0f
array_command_fail[1]="$OPCODE""$PORT""$NODE""$CFG""$SIZE""$PATTERN""$NUM"
NODE=02
CFG=05
array_command_fail[2]="$OPCODE""$PORT""$NODE""$CFG""$SIZE""$PATTERN""$NUM"

array_des_fail[0]="Invalide Port"
array_des_fail[1]="Invalide Node"
array_des_fail[2]="Invalide CFG"

. $CUR_PATH/diag_verify.sh

diag_verify true array_command[@] array_des[@]

diag_verify false array_command_fail[@] array_des_fail[@]

echo -e '\n\n'
echo "#-------------------$SEC_NAME passed!-------------------"
echo -e '\n\n'
exit 0
