#!/bin/bash

CUR_PATH=$(dirname $0)

SEC_NAME="PRBS Test"
OPCODE=0020

HIGIG0_BUS=00
QSGMII0_BUS=01
ORDER=00

echo "#-----------------------$SEC_NAME-----------------------"

#Define array for command and description: following Diag and DVT Spec

#command array
array_command[0]=01000102
array_command[1]="$OPCODE""$HIGIG0_BUS""$ORDER"
array_command[2]="$OPCODE""$QSGMII0_BUS""$ORDER"

#command description array, need match with command array above.
array_des[0]="Suspend the DUT"
array_des[1]="HiGig0 PRBS test"
array_des[2]="QSGMII0 PRBS test"

array_command_fail[0]="$OPCODE""30""$ORDER"
array_command_fail[1]="$OPCODE""$HIGIG0_BUS""04"
array_des_fail[0]="PRBS fail for invalid bus"
array_des_fail[1]="PRBS fail for invalid order"

. $CUR_PATH/diag_verify.sh

diag_verify true array_command[@] array_des[@]

diag_verify false array_command_fail[@] array_des_fail[@]

echo -e '\n\n'
echo "#-------------------$SEC_NAME passed!-------------------"
echo -e '\n\n'
exit 0
