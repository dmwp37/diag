#!/bin/bash

CUR_PATH=$(dirname $0)
SEC_NAME="VERSION Test"

echo "#-----------------------$SEC_NAME-----------------------"

#Define array for command and description: following Diag and DVT Spec

#command array
array_command[0]=000000
array_command[1]=000001
array_command[2]=000002
array_command[3]=000003

#command description array, need match with command array above.
array_des[0]="Get Diag version"
array_des[1]="Get SW version"
array_des[2]="Get HW version"
array_des[3]="Get FPGA version"

array_command_fail[0]=0000ff

array_des_fail[0]="Suspend invalid type"

. $CUR_PATH/diag_verify.sh

diag_verify true array_command[@] array_des[@]

diag_verify false array_command_fail[@] array_des_fail[@]

echo -e '\n\n'
echo "#-------------------$SEC_NAME passed!-------------------"
echo -e '\n\n'
exit 0
