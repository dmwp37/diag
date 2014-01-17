#!/bin/bash

# start the diagd
../out/install/bin/diagd > /dev/null 2>&1 &

# wait the daemon started
sleep 1

../out/install/bin/diag_send 0fffaabbccdd
if [ $? != 0 ]; then
  echo 1 test failed!
  exit -1
fi

../out/install/bin/diagd_test 2
if [ $? != 0 ]; then
  echo 2 test failed!
  exit -1
fi

../out/install/bin/diagd_test 3
if [ $? != 0 ]; then
  echo 3 test failed!
  exit -1
fi

../out/install/bin/diagd_test 4
if [ $? != 0 ]; then
  echo 4 test failed!
  exit -1
fi

#../out/install/bin/diagd_test 5
#if [ $? != 0 ]; then
#  echo 5 test failed!
#  exit -1
#fi

../out/install/bin/diagd_test 6
if [ $? != 0 ]; then
  echo 6 test failed!
  exit -1
fi

# stop the diagd
PID=`cat /tmp/diagd.pid`
kill -s SIGINT $PID

echo test passed!
exit 0
