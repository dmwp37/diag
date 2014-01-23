#!/bin/bash

CUR_PATH=$(dirname $0)

#need to restart the diag daemon
$CUR_PATH/kill_diagd.sh

SEC_NAME="Suspend Test"

echo "#-----------------------$SEC_NAME-----------------------"

#Define array for command and description: following Diag and DVT Spec

#command array
array_command[0]=010000
array_command[1]=01000101
array_command[2]=01000102
array_command[3]=01000102
array_command[4]=010000

#command description array, need match with command array above.
array_des[0]="Get suspend mode"
array_des[1]="Suspend to normal mode"
array_des[2]="Suspend to test mode"
array_des[3]="Suspend to test mode"
array_des[4]="Get suspend mode"


array_command_fail[0]=010003
array_command_fail[1]=01000104
array_command_fail[2]=01000101

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
