#!/bin/bash

# $1 true verify pass; false verify fail
# $2 command opcode array
# $3 command description array

diag_verify() {

#check the diag daemon first
$CUR_PATH/start_diagd.sh

FILE_FAILURE_LOG="$0"_failure_log.txt
FILE_TEMP=temp_log.txt

#delete log file firstly if exist
if [ -f $FILE_FAILURE_LOG ]; then
    rm -rf $FILE_FAILURE_LOG
fi

#define cmd
CLIENT_CMD="$CUR_PATH/../out/install/bin/diag_send"

declare -i __exit_status=0
local index=0

declare -a __array_command=("${!2}")
declare -a __array_des=("${!3}")

for var in ${__array_command[@]}; do
    echo -e "$index)"

    echo ${__array_des[$index]} : $var

    $CLIENT_CMD $var > $FILE_TEMP

    local test_pass=$?

    if [ $1 = "true" ]; then
        if [ $test_pass != 0 ]; then
            __exit_status=1;

            echo -e $CLIENT_CMD $var failed!!!!!!!!!!!!!!!!!! '\n'
            echo -e '\n'$SEC_NAME ${__array_des[$index]} : $var failed!!! '\n' >> $FILE_FAILURE_LOG
            cat $FILE_TEMP
            cat $FILE_TEMP>>$FILE_FAILURE_LOG
        else
            echo -e $CLIENT_CMD $var succeeded! '\n'
        fi
    else
        if [ $test_pass != 0 ]; then
            echo -e $CLIENT_CMD $var failure test passed! '\n'
            cat $FILE_TEMP
        else
            __exit_status=2

            echo -e $CLIENT_CMD $var verify failure failed!!!!!!!!!!!!!!!!!! '\n'
            echo -e '\n'$SEC_NAME ${__array_des[$index]} : $var verify failure failed!!! '\n' >> $FILE_FAILURE_LOG
            cat $FILE_TEMP>>$FILE_FAILURE_LOG
        fi
    fi
    let index+=1;
done

#delete temp log file
if [ -f $FILE_TEMP ]; then
    rm -rf $FILE_TEMP
fi

if [ $__exit_status != 0 ]; then
    echo -e '\n\n'
    echo "#-------------------$SEC_NAME failed!-------------------"
    echo -e '\n\n'
    exit 1
fi

return $__exit_status
}

