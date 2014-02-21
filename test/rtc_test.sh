#!/bin/bash

CUR_PATH=$(dirname $0)

SEC_NAME="RTC Test"
OPCODE=0016

GET_ACTION=00
SET_ACTION=01

#2014-02-20 17:50:58
DATE_VALUE=07de021411323a 

echo "#-----------------------$SEC_NAME-----------------------"

#Define array for command and description: following Diag and DVT Spec

#command array
array_command[0]="$OPCODE""$GET_ACTION"
array_command[1]="$OPCODE""$GET_ACTION"
array_command[2]="$OPCODE""$SET_ACTION""$DATE_VALUE"
array_command[3]="$OPCODE""$GET_ACTION"

#command description array, need match with command array above.
array_des[0]="Get RTC date"
array_des[1]="Get RTC date"
array_des[2]="Set RTC date"
array_des[3]="Get RTC date"

array_command_fail[0]=$OPCODE""03
array_des_fail[0]="Invalid action"

. $CUR_PATH/diag_verify.sh

diag_verify true array_command[@] array_des[@]

diag_verify false array_command_fail[@] array_des_fail[@]

echo -e '\n\n'
echo "#-------------------$SEC_NAME passed!-------------------"
echo -e '\n\n'
exit 0
