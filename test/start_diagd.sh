#!/bin/bash

CUR_PATH=$(dirname $0)
DIAGD_APP=$CUR_PATH/../out/install/bin/diagd
DIAGD_PID_FILE="/tmp/diagd.pid"
DIAGD_LOG_FILE="/tmp/diagd.log"

if [ ! -e $DIAGD_PID_FILE ]; then
  # start the diagd
  echo start the diag daemon

  rm -f $DIAGD_LOG_FILE
  $DIAGD_APP > $DIAGD_LOG_FILE 2>&1 &

  # wait the daemon started
  sleep 1
fi

