#!/bin/bash

CUR_PATH=$(dirname $0)
DIAGD_APP=$CUR_PATH/../out/install/bin/diagd
DIAGD_PID_FILE="/tmp/diagd.pid"
DIAGD_LOG_FILE="/tmp/diagd.log"

if [ -e $DIAGD_PID_FILE  ]; then
  PID=$(cat $DIAGD_PID_FILE)

  if [ -z "$(ps -p $PID | grep diagd)"  ]; then
    rm -f $DIAGD_PID_FILE
  fi
fi

if [ ! -e $DIAGD_PID_FILE ]; then
  # start the diagd
  echo start the diag daemon

  rm -f $DIAGD_LOG_FILE
  $DIAGD_APP > $DIAGD_LOG_FILE 2>&1 &
  chmod 666 $DIAGD_LOG_FILE

  # wait the daemon started
  sleep 1
fi

