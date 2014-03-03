#!/bin/bash

CUR_PATH=$(dirname $0)

SEC_NAME="PHY Test"
OPCODE=000B

READ_REG_ACTION=00
WRITE_REG_ACTION=01

PORT_ID=00
OFST_CTL=00
OFST_STS=01

echo "#-----------------------$SEC_NAME-----------------------"

#Define array for command and description: following Diag and DVT Spec

#command array
array_command[0]=01000102
array_command[1]="$OPCODE""$READ_REG_ACTION""$PORT_ID""$OFST_STS"
array_command[2]="$OPCODE""$WRITE_REG_ACTION""$PORT_ID""$OFST_CTL"0800

#command description array, need match with command array above.
array_des[0]="Suspend the DUT"
array_des[1]="Read PHY register"
array_des[2]="Write PHY register to power down the device"

PORT_ID=01
array_command_fail[0]="$OPCODE""$READ_REG_ACTION""$PORT_ID""$OFST_STS"
array_des_fail[0]="Read invalid PHY Port ID "$PORT_ID

. $CUR_PATH/diag_verify.sh

diag_verify true array_command[@] array_des[@]

diag_verify false array_command_fail[@] array_des_fail[@]

echo -e '\n\n'
echo "#-------------------$SEC_NAME passed!-------------------"
echo -e '\n\n'
exit 0
