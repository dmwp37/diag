#!/bin/bash

CUR_PATH=$(dirname $0)

SEC_NAME="PSU Test"
OPCODE=000A

DUMP_INFO_ACTION=00
SET_INFO_ACTION=01
GET_STATUS_ACTION=02
SET_CFG_ACTION=03

SLOT0=00
SLOT1=01

EEPROM_CHANNEL=00
PSMI_CHANNEL=01

ADDR=18
DATA=8000

CFG_TURN_OFF=00
CFG_TURN_ON=01

echo "#-----------------------$SEC_NAME-----------------------"

#Define array for command and description: following Diag and DVT Spec

#command array
array_command[0]=01000102
array_command[1]="$OPCODE""$DUMP_INFO_ACTION""$SLOT0""$EEPROM_CHANNEL"
array_command[2]="$OPCODE""$SET_INFO_ACTION""$SLOT1""$PSMI_CHANNEL""$ADDR""$DATA"
array_command[3]="$OPCODE""$GET_STATUS_ACTION""$SLOT0"
array_command[4]="$OPCODE""$SET_CFG_ACTION""$SLOT1""$CFG_TURN_OFF"

#command description array, need match with command array above.
array_des[0]="Suspend the DUT"
array_des[1]="Dump Slot0 EEPROM information"
array_des[2]="Wirte Slot1 PSMI addr=$ADDR data=$DATA"
array_des[3]="Set Slot0 status"
array_des[4]="Turn off Slot1"


array_command_fail[0]="$OPCODE""04""$SLOT0"

SLOT=03
array_command_fail[1]="$OPCODE""$GET_STATUS_ACTION""$SLOT"

CHANNEL=03
array_command_fail[2]="$OPCODE""$DUMP_INFO_ACTION""$SLOT0""$CHANNEL"

CFG=03
array_command_fail[3]="$OPCODE""$SET_CFG_ACTION""$SLOT1""$CFG"

array_des_fail[0]="Invalid action"
array_des_fail[1]="Invalid slot"
array_des_fail[2]="Invalid channel"
array_des_fail[3]="Invalid cfg"


. $CUR_PATH/diag_verify.sh

diag_verify true array_command[@] array_des[@]

diag_verify false array_command_fail[@] array_des_fail[@]

echo -e '\n\n'
echo "#-------------------$SEC_NAME passed!-------------------"
echo -e '\n\n'
exit 0
