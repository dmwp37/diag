#!/bin/sh

SEC_NAME="LED Test"

echo "#-----------------------$SEC_NAME-----------------------"

#Define array for command and description: following Diag and DVT Spec

#command array
array_command[0]=0001010000
array_command[1]=0001010100
array_command[2]=00010000
array_command[3]=00010001

#command description array, need match with command array above.
array_des[0]="Enable Power LED"
array_des[1]="Enable Status LED"
array_des[2]="Disable Power LED"
array_des[3]="Disable Status LED"

FILE_FAILURE_LOG="$0"_failure_log.txt
FILE_TEMP=temp_log.txt

#delete log file firstly if exist
if [ -f $FILE_FAILURE_LOG ]; then
    rm -rf $FILE_FAILURE_LOG
fi

#define cmd
CMD="../out/install/bin/diag_send"

exit_status=0
index=0;

for var in ${array_command[@]}; do

    echo ${array_des[$index]} : $var

    $CMD $var > $FILE_TEMP

    grep success $FILE_TEMP > /dev/null

    if [ $? != 0 ]; then
        exit_status=-1;

        echo -e diag_send $var failed!!!!!!!!!!!!!!!!!! '\n'
        echo -e '\n'$SEC_NAME ${array_des[$index]} : $var failed!!! '\n' >> $FILE_FAILURE_LOG
        cat $FILE_TEMP>>$FILE_FAILURE_LOG

        if [ ! -z $2 ]; then
            break;
        fi

    else
        echo -e diag_send $var succeeded! '\n'
    fi

    let index+=1;
done

#delete temp log file
if [ -f $FILE_TEMP ]; then
    rm -rf $FILE_TEMP
fi
exit $exit_status

