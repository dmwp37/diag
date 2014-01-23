#!/bin/bash

CUR_PATH=$(dirname $0)

SEC_NAME="I2C Test"

echo "#-----------------------$SEC_NAME-----------------------"

#Define array for command and description: following Diag and DVT Spec

#command array
array_command[0]=01000102
array_command[1]=0ffd00000000
array_command[2]=00100000510010
array_command[3]=001001005300020203
array_command[4]=0010020054000201020008
array_command[5]=0ffd00000001

#command description array, need match with command array above.
array_des[0]="Suspend the DUT"
array_des[1]="Enable Data Dump"
array_des[2]="Read I2C Bus"
array_des[3]="Write I2C Bus"
array_des[4]="Write&Read I2C Bus"
array_des[5]="Disable Data Dump"

. $CUR_PATH/diag_verify.sh
diag_verify true array_command[@] array_des[@]

echo -e '\n\n'
echo "#-------------------$SEC_NAME passed!-------------------"
echo -e '\n\n'
exit 0
