#!/bin/bash

CUR_PATH=$(dirname $0)

SEC_NAME="GPIO Test"
OPCODE=0017

GET_ACTION=00
SET_ACTION=01
SET_CFG_ACTION=02
GET_CFG_ACTION=03

CFG_INPUT=00
CFG_OUTPUT=01

GPIO_PORT=03

GPIO_INACTIVE=00
GPIO_ACTIVE=01

echo "#-----------------------$SEC_NAME-----------------------"

#Define array for command and description: following Diag and DVT Spec

#command array
array_command[0]=01000102
array_command[1]="$OPCODE""$GET_ACTION""$GPIO_PORT"
array_command[2]="$OPCODE""$SET_CFG_ACTION""$GPIO_PORT""$CFG_OUTPUT"
array_command[3]="$OPCODE""$SET_ACTION""$GPIO_PORT""$GPIO_ACTIVE"
array_command[4]="$OPCODE""$GET_ACTION""$GPIO_PORT"
array_command[5]="$OPCODE""$GET_CFG_ACTION""$GPIO_PORT"

#command description array, need match with command array above.
array_des[0]="Suspend the DUT"
array_des[1]="Get GPIO $GPIO_PORT value"
array_des[2]="Config GPIO $GPIO_PORT as output"
array_des[3]="Set GPIO $GPIO_PORT to active"
array_des[4]="Get GPIO $GPIO_PORT value"
array_des[5]="Get GPIO $GPIO_PORT configuration"

array_command_fail[0]="$OPCODE""04""$GPIO_PORT"
GPIO_PORT=50
array_command_fail[1]="$OPCODE""$GET_ACTION""$GPIO_PORT"

GPIO_PORT=03
GPIO_VALUE=02
array_command_fail[2]="$OPCODE""$SET_ACTION""$GPIO_PORT""$GPIO_VALUE"

GPIO_CFG=03
array_command_fail[3]="$OPCODE""$SET_CFG_ACTION""$GPIO_PORT""$GPIO_CFG"

GPIO_PORT=00
array_command_fail[4]="$OPCODE""$SET_ACTION""$GPIO_PORT""$GPIO_ACTIVE"

array_des_fail[0]="Invalid action"
array_des_fail[1]="Invalid port"
array_des_fail[2]="Invalid set value"
array_des_fail[3]="Invalid cfg"
array_des_fail[4]="Can't set gpio when configured as input"


. $CUR_PATH/diag_verify.sh

diag_verify true array_command[@] array_des[@]

diag_verify false array_command_fail[@] array_des_fail[@]

echo -e '\n\n'
echo "#-------------------$SEC_NAME passed!-------------------"
echo -e '\n\n'
exit 0
