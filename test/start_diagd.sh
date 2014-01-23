#!/bin/bash

CUR_PATH=$(dirname $0)
DIAGD_APP=$CUR_PATH/../out/install/bin/diagd
DIAGD_PID_FILE="/tmp/diagd.pid"

if [ ! -e $DIAGD_PID_FILE ]; then
  # start the diagd
  echo start the diag daemon

  $DIAGD_APP > /dev/null 2>&1 &

  # wait the daemon started
  sleep 1
fi

