#!/bin/bash
CUR_PATH=$(dirname $0)
DIAG_CMD=$CUR_PATH/../out/install/bin/diag_send
DIAG_IN="/tmp/diag_in"
DIAG_OUT="/tmp/diag_out"
CLI_PID_FILE="/tmp/diag_cli.pid"

if [ $# == 0 ]; then
  echo Need parameter
  exit 1
fi

if [ $1 == 'stop' ]; then
  if [ -e $CLI_PID_FILE  ]; then
    PID=`cat $CLI_PID_FILE`
    kill -TERM -$PID
    rm -f $CLI_PID_FILE
  fi

  if [ -e $DIAG_IN   ]; then
    rm -f $DIAG_IN
  fi

  if [ -e $DIAG_OUT   ]; then
    rm -f $DIAG_OUT
  fi

  exit 0
fi

if [ ! -e $CLI_PID_FILE ]; then
  $CUR_PATH/cli_server.sh &
  echo $$ > $CLI_PID_FILE
  sleep 1
fi

echo $@ > $DIAG_IN

exec 3<$DIAG_OUT
timeout=10
while read -t $timeout -u 3 line; do echo "$line";timeout=0.1; done

