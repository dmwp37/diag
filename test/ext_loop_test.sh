#!/bin/bash

CUR_PATH=$(dirname $0)

SEC_NAME="EXT_LOOP Test"
OPCODE=0022

SET_EXT_LOOP_ACTION=00
CLEAR_EXT_LOOP_ACTION=01
NODE_PCH=00
NODE_FPGA=01
NODE_SWITCH=02
NODE_PHY_MGT=10
NODE_PHY0_P1=11
NODE_PHY0_P2=12
NODE_PHY0_P3=13
NODE_PHY0_P4=14
NODE_PHY0_P5=15
NODE_PHY0_P6=16
NODE_PHY0_P7=17
NODE_PHY0_P8=18
NODE_PHY1_P1=19
NODE_PHY1_P2=1A
NODE_PHY1_P3=1B
NODE_PHY1_P4=1C

echo "#-----------------------$SEC_NAME-----------------------"

#Define array for command and description: following Diag and DVT Spec

#command array
array_command[0]=01000102
array_command[1]="$OPCODE""$SET_EXT_LOOP_ACTION""$NODE_PCH"
array_command[2]="$OPCODE""$CLEAR_EXT_LOOP_ACTION""$NODE_PCH"

#command description array, need match with command array above.
array_des[0]="Suspend the DUT"
array_des[1]="Set external loopback for PCH"
array_des[2]="Clear external loopback for PCH"

array_command_fail[0]="$OPCODE""$SET_EXT_LOOP_ACTION""$NODE_FPGA"
array_command_fail[1]="$OPCODE"02"$NODE_PHY1_P1"
array_command_fail[2]="$OPCODE""$SET_EXT_LOOP_ACTION"20
array_des_fail[0]="Set external loopback fail for FPGA"
array_des_fail[1]="Set external loopback fail for Invalid action"
array_des_fail[2]="Set external loopback fail for Invalid node"

. $CUR_PATH/diag_verify.sh

diag_verify true array_command[@] array_des[@]

diag_verify false array_command_fail[@] array_des_fail[@]

echo -e '\n\n'
echo "#-------------------$SEC_NAME passed!-------------------"
echo -e '\n\n'
exit 0
