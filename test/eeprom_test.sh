#!/bin/bash

CUR_PATH=$(dirname $0)

SEC_NAME="EEPROM Test"
OPCODE=0004

READ_ACTION=00
WRITE_ACTION=01

EEPROM=00
ADDR=0010
DATA=ff

echo "#-----------------------$SEC_NAME-----------------------"

#Define array for command and description: following Diag and DVT Spec

#command array
array_command[0]=01000102
array_command[1]="$OPCODE""$READ_ACTION""$EEPROM""$ADDR"
array_command[2]="$OPCODE""$WRITE_ACTION""$EEPROM""$ADDR""$DATA"

#command description array, need match with command array above.
array_des[0]="Suspend the DUT"
array_des[1]="READ EEPROM"
array_des[2]="WRITE EEPROM"

array_command_fail[0]="$OPCODE"03
array_command_fail[1]="$OPCODE""$READ_ACTION""0f""$ADDR"
array_des_fail[0]="Invalid action"
array_des_fail[1]="Invalid eeprom"

. $CUR_PATH/diag_verify.sh

diag_verify true array_command[@] array_des[@]

diag_verify false array_command_fail[@] array_des_fail[@]

echo -e '\n\n'
echo "#-------------------$SEC_NAME passed!-------------------"
echo -e '\n\n'
exit 0
