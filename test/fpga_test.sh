#!/bin/bash

CUR_PATH=$(dirname $0)

SEC_NAME="FPGA Test"
OPCODE=000D

RD_REG_ACTION=00
WR_REG_ACTION=01
RD_MEM_ACTION=02
WR_MEM_ACTION=03
DMA_RD_ACTION=04
DMA_WR_ACTION=05
SF_TST_ACTION=06
ST_MOD_ACTION=07
DOWNLD_ACTION=08

REG_ADDR=0005
REG_DATA=DEADBEEF

MEM_ADDR=00000010
MEM_SIZE=0004
MEM_DATA=aabbccdd

SUB_TEST=02

MODE=01

IMAGE=00


echo "#-----------------------$SEC_NAME-----------------------"

#Define array for command and description: following Diag and DVT Spec

#command array
array_command[0]=01000102
array_command[1]=0ffd010000
array_command[2]="$OPCODE""$WR_REG_ACTION""$REG_ADDR""$REG_DATA"
array_command[3]="$OPCODE""$RD_REG_ACTION""$REG_ADDR"
array_command[4]="$OPCODE""$WR_MEM_ACTION""$MEM_ADDR""$MEM_SIZE""$MEM_DATA"
array_command[5]="$OPCODE""$RD_MEM_ACTION""$MEM_ADDR""$MEM_SIZE"
array_command[6]="$OPCODE""$DMA_WR_ACTION""$MEM_ADDR""$MEM_SIZE""$MEM_DATA"
array_command[7]="$OPCODE""$DMA_RD_ACTION""$MEM_ADDR""$MEM_SIZE"
array_command[8]="$OPCODE""$SF_TST_ACTION""$SUB_TEST"
array_command[9]="$OPCODE""$ST_MOD_ACTION""$MODE"
array_command[10]="$OPCODE""$DOWNLD_ACTION""$IMAGE"
array_command[11]=0ffd010001

#command description array, need match with command array above.
array_des[0]="Suspend the DUT"
array_des[1]="Enable Data Dump"
array_des[2]="Write FPGA register=$REG_ADDR value=$REG_DATA"
array_des[3]="Read FPGA register=$REG_ADDR"
array_des[4]="Write FPGA memory addr=$MEM_ADDR len=$MEM_SIZE data=$MEM_DATA"
array_des[5]="Read FPGA memory addr=$MEM_ADDR len=$MEM_SIZE"
array_des[6]="DMA WR FPGA memory addr=$MEM_ADDR len=$MEM_SIZE data=$MEM_DATA"
array_des[7]="DMA RD FPGA memory addr=$MEM_ADDR len=$MEM_SIZE"
array_des[8]="Set FPGA sub test=$SUB_TEST"
array_des[9]="Set FPGA mode=$MODE"
array_des[10]="Download FPGA image=$IMAGE"
array_des[11]="Disable Data Dump"

REG_ADDR=1000
MEM_ADDR=01000000
SUB_TEST=0f
MODE=0f
IMAGE=0f

array_command_fail[0]="$OPCODE""$RD_REG_ACTION""$REG_ADDR"
array_command_fail[1]="$OPCODE""$RD_MEM_ACTION""$MEM_ADDR""$MEM_SIZE"
array_command_fail[2]="$OPCODE""$SF_TST_ACTION""$SUB_TEST"
array_command_fail[3]="$OPCODE""$ST_MOD_ACTION""$MODE"
array_command_fail[4]="$OPCODE""$DOWNLD_ACTION""$IMAGE"

array_des_fail[0]="Read invalid FPGA register"
array_des_fail[1]="Read invalid FPGA memory"
array_des_fail[2]="Invalide Self Test"
array_des_fail[3]="Invalide Mode"
array_des_fail[4]="Invalide Download image"

. $CUR_PATH/diag_verify.sh

diag_verify true array_command[@] array_des[@]

diag_verify false array_command_fail[@] array_des_fail[@]

echo -e '\n\n'
echo "#-------------------$SEC_NAME passed!-------------------"
echo -e '\n\n'
exit 0
