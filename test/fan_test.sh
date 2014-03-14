#!/bin/bash

CUR_PATH=$(dirname $0)

SEC_NAME="FAN Test"
OPCODE=0002

GET_RPM=00
GET_RPM_LIMIT=01
SET_PWM=02
SET_PWM_MAX=03
GET_STATUS=04

PWM=32 #50%
PWM_MAX=50 #80%

FAN_ID=02

echo "#-----------------------$SEC_NAME-----------------------"

#Define array for command and description: following Diag and DVT Spec

#command array
array_command[0]="$OPCODE""$GET_RPM""$FAN_ID"
array_command[1]="$OPCODE""$GET_RPM_LIMIT""$FAN_ID"
array_command[2]="$OPCODE""$SET_PWM_MAX""$FAN_ID""$PWM_MAX"
array_command[3]="$OPCODE""$SET_PWM""$FAN_ID""$PWM"
array_command[4]="$OPCODE""$GET_STATUS""$FAN_ID"

#command description array, need match with command array above.
array_des[0]="Get FAN $FAN_ID RPM"
array_des[1]="Get FAN $FAN_ID RPM limit"
array_des[2]="Set FAN $FAN_ID PWM_MAX"
array_des[3]="Set FAN $FAN_ID PWM"
array_des[4]="Get FAN $FAN_ID Status"

array_command_fail[0]="$OPCODE""05""$FAN_ID"

FAN_ID=05
array_command_fail[1]="$OPCODE""$GET_RPM""$FAN_ID"

FAN_ID=03
PWM_MAX=70
array_command_fail[2]="$OPCODE""$SET_PWM_MAX""$FAN_ID""$PWM_MAX"

FAN_ID=02
PWM=5F #95%
array_command_fail[3]="$OPCODE""$SET_PWM""$FAN_ID""$PWM"


array_des_fail[0]="Invalid action"
array_des_fail[1]="Invalid fan"
array_des_fail[2]="Invalid set PWM_MAX 112%"
array_des_fail[3]="Invalid set PWM exceed PWM_MAX"


. $CUR_PATH/diag_verify.sh

diag_verify true array_command[@] array_des[@]

diag_verify false array_command_fail[@] array_des_fail[@]

echo -e '\n\n'
echo "#-------------------$SEC_NAME passed!-------------------"
echo -e '\n\n'
exit 0
