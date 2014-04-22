#!/bin/bash

DIAGD_PID_FILE="/tmp/diagd.pid"

if [ -e $DIAGD_PID_FILE ]; then
  PID=$(cat $DIAGD_PID_FILE)

  if [ -z "$(ps -p $PID | grep diagd)" ]; then
    rm -f $DIAGD_PID_FILE
  else
    # stop the diagd
    echo kill diag deamon
    kill -s SIGINT $PID
  fi
fi

