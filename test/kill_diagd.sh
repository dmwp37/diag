#!/bin/bash

DIAGD_PID_FILE="/tmp/diagd.pid"

if [ -e $DIAGD_PID_FILE ]; then
  # stop the diagd
  echo kill diag deamon

  PID=`cat $DIAGD_PID_FILE`
  kill -s SIGINT $PID
fi

