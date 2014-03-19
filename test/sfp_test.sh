#!/bin/bash

CUR_PATH=$(dirname $0)

SEC_NAME="SFP Test"
OPCODE=0006

GET_STATUS=00
TX_CONFIG=01
DUMP_REGISTER=02

SFP_ID=03

TX_DISABLE=00
TX_ENABLE=01

REG_A0=00
REG_A2=01

echo "#-----------------------$SEC_NAME-----------------------"

#Define array for command and description: following Diag and DVT Spec

#command array
array_command[0]=01000102
array_command[1]="$OPCODE""$GET_STATUS""$SFP_ID"
array_command[2]="$OPCODE""$TX_CONFIG""$SFP_ID""$TX_DISABLE"
array_command[3]="$OPCODE""$TX_CONFIG""$SFP_ID""$TX_ENABLE"
array_command[4]="$OPCODE""$DUMP_REGISTER""$SFP_ID""$REG_A0"
array_command[5]="$OPCODE""$DUMP_REGISTER""$SFP_ID""$REG_A2"

#command description array, need match with command array above.
array_des[0]="Suspend the DUT"
array_des[1]="Get SFP $SFP_ID status"
array_des[2]="Config SFP $SFP_ID tx disable"
array_des[3]="Config SFP $SFP_ID tx enable"
array_des[4]="Dump SFP $SFP_ID A0 Registers"
array_des[5]="Dump SFP $SFP_ID A2 Registers"

array_command_fail[0]="$OPCODE""04""$SFP_ID"

SFP_ID=50
array_command_fail[1]="$OPCODE""$GET_ACTION""$SFP_ID"

SFP_ID=03
array_command_fail[2]="$OPCODE""$TX_CONFIG""$SFP_ID""03"

array_command_fail[3]="$OPCODE""$DUMP_REGISTER""$SFP_ID""02"


array_des_fail[0]="Invalid action"
array_des_fail[1]="Invalid SFP ID"
array_des_fail[2]="Invalid TX config"
array_des_fail[3]="Invalid dump registers"


. $CUR_PATH/diag_verify.sh

diag_verify true array_command[@] array_des[@]

diag_verify false array_command_fail[@] array_des_fail[@]

echo -e '\n\n'
echo "#-------------------$SEC_NAME passed!-------------------"
echo -e '\n\n'
exit 0
