#!/bin/bash

CUR_PATH=$(dirname $0)

#need to restart the diag daemon
$CUR_PATH/kill_diagd.sh

SEC_NAME="Suspend Test"
OPCODE=0100

GET=00
SET=01

NORMAL_MODE=01
TEST_MODE=02

echo "#-----------------------$SEC_NAME-----------------------"

#Define array for command and description: following Diag and DVT Spec

#command array
array_command[0]="$OPCODE""$GET"
array_command[1]="$OPCODE""$SET""$NORMAL_MODE"
array_command[2]="$OPCODE""$SET""$TEST_MODE"
array_command[3]="$OPCODE""$SET""$TEST_MODE"
array_command[4]="$OPCODE""$GET"

#command description array, need match with command array above.
array_des[0]="Get suspend mode"
array_des[1]="Suspend to normal mode"
array_des[2]="Suspend to test mode"
array_des[3]="Suspend to test mode"
array_des[4]="Get suspend mode"


array_command_fail[0]="$OPCODE"03
array_command_fail[1]="$OPCODE""$SET"04
array_command_fail[2]="$OPCODE""$SET""$NORMAL_MODE"

array_des_fail[0]="Suspend invalid action"
array_des_fail[1]="Suspend invalid mode"
array_des_fail[2]="Suspend cannot set back to normal mode"

. $CUR_PATH/diag_verify.sh

diag_verify true array_command[@] array_des[@]

diag_verify false array_command_fail[@] array_des_fail[@]

echo -e '\n\n'
echo "#-------------------$SEC_NAME passed!-------------------"
echo -e '\n\n'

exit 0
