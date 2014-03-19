#!/bin/bash

CUR_PATH=$(dirname $0)
CLIENT_CMD=$CUR_PATH/../out/install/bin/diag_send
DIAGD_TEST=$CUR_PATH/../out/install/bin/diagd_test

#need to restart the diag daemon
$CUR_PATH/kill_diagd.sh
$CUR_PATH/start_diagd.sh

$CLIENT_CMD 0fffaabbccdd
if [ $? != 0 ]; then
  echo 1 test failed!
  exit -1
fi

$DIAGD_TEST 2
if [ $? != 0 ]; then
  echo 2 test failed!
  exit -1
fi

$DIAGD_TEST 3
if [ $? != 0 ]; then
  echo 3 test failed!
  exit -1
fi

$DIAGD_TEST 4
if [ $? != 0 ]; then
  echo 4 test failed!
  exit -1
fi

#$DIAGD_TEST 5
#if [ $? != 0 ]; then
#  echo 5 test failed!
#  exit -1
#fi

$DIAGD_TEST 6
if [ $? != 0 ]; then
  echo 6 test failed!
  exit -1
fi


SUB_TEST_FILES=$(ls $CUR_PATH/*_test.sh)

for test_file in $SUB_TEST_FILES; do
  $test_file
  if [ $? != 0 ]; then
    echo sub test failed!
    exit -1
  fi
done

# stop the diagd
$CUR_PATH/kill_diagd.sh

echo test passed!
exit 0
