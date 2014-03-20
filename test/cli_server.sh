#!/bin/bash
CUR_PATH=$(dirname $0)
DIAG_CMD=$CUR_PATH/../out/install/bin/diag_send
DIAG_IN="/tmp/diag_in"
DIAG_OUT="/tmp/diag_out"

DIAG_SERVER="localhost"

if [ ! -p $DIAG_IN ]; then
  mkfifo -m 777 $DIAG_IN
fi

if [ ! -p $DIAG_OUT ]; then
  mkfifo -m 777 $DIAG_OUT
fi

while (true); do cat $DIAG_IN; done | $DIAG_CMD -l$DIAG_SERVER -f > $DIAG_OUT
