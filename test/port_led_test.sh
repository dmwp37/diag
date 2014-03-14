#!/bin/bash

CUR_PATH=$(dirname $0)

SEC_NAME="PORT_LED Test"
OPCODE=0019

NORMAL_PORT_LED_ACTION=00
TURN_ON_PORT_LED_ACTION=01
TURN_OFF_PORT_LED_ACTION=02
BLINK_PORT_LED_ACTION=03
LED_ID_LINK=00
PORT_MGT=00

echo "#-----------------------$SEC_NAME-----------------------"

#Define array for command and description: following Diag and DVT Spec

#command array
array_command[0]=01000102
array_command[1]="$OPCODE""$TURN_ON_PORT_LED_ACTION""$LED_ID_LINK""$PORT_MGT"
array_command[2]="$OPCODE""$TURN_OFF_PORT_LED_ACTION""$LED_ID_LINK""$PORT_MGT"
array_command[3]="$OPCODE""$BLINK_PORT_LED_ACTION""$LED_ID_LINK""$PORT_MGT"
array_command[4]="$OPCODE""$NORMAL_PORT_LED_ACTION""$LED_ID_LINK""$PORT_MGT"

#command description array, need match with command array above.
array_des[0]="Suspend the DUT"
array_des[1]="Turn on the Link LED of MGT port"
array_des[2]="Turn off the Link LED of MGT port"
array_des[3]="Make the Link LED of MGT port blink"
array_des[4]="Make the Link LED of MGT port normal"

array_command_fail[0]="$OPCODE"ff"$LED_ID_LINK""$PORT_MGT"
array_command_fail[1]="$OPCODE""$TURN_ON_PORT_LED_ACTION"ff"$PORT_MGT"
array_command_fail[2]="$OPCODE""$TURN_ON_PORT_LED_ACTION""$LED_ID_LINK"ff
array_des_fail[0]="Test port LED fail for invalid action"
array_des_fail[1]="Test port LED fail for invalid LED ID"
array_des_fail[2]="Test port LED fail for invalid port"

. $CUR_PATH/diag_verify.sh

diag_verify true array_command[@] array_des[@]

diag_verify false array_command_fail[@] array_des_fail[@]

echo -e '\n\n'
echo "#-------------------$SEC_NAME passed!-------------------"
echo -e '\n\n'
exit 0
