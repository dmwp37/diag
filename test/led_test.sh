#!/bin/bash

CUR_PATH=$(dirname $0)
SEC_NAME="LED Test"

echo "#-----------------------$SEC_NAME-----------------------"

#Define array for command and description: following Diag and DVT Spec

#command array
array_command[0]=0001010000
array_command[1]=0001010100
array_command[2]=00010000
array_command[3]=00010001

#command description array, need match with command array above.
array_des[0]="Enable Power LED"
array_des[1]="Enable Status LED"
array_des[2]="Disable Power LED"
array_des[3]="Disable Status LED"

. $CUR_PATH/diag_verify.sh
diag_verify true array_command[@] array_des[@]

echo -e '\n\n'
echo "#-------------------$SEC_NAME passed!-------------------"
echo -e '\n\n'
exit 0
