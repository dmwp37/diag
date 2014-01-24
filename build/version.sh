#!/bin/bash
CUR_PATH=$(dirname $0)
VERSION_FILE=$CUR_PATH/../out/diag_version.h

count=$(git rev-list HEAD | wc -l | xargs -n1 printf %04d)
commit=`git rev-list -1 --abbrev-commit HEAD`

VERSION_STRING=\"$count.$commit\"
#echo $VERSION_STRING

update_version()
{
mkdir -p `dirname $VERSION_FILE`
cat > $VERSION_FILE << EOF
#ifndef _DG_DIAG_VERSION_H_
#define _DG_DIAG_VERSION_H_

#define DIAG_VERSION $VERSION_STRING

#endif
EOF
}


test_version()
{
    pattern="^#define DIAG_VERSION "
    old_version=`grep "$pattern" $VERSION_FILE | sed -e "s!$pattern!!"`
    if [ $old_version != $VERSION_STRING ]; then
        echo update with new version: $VERSION_STRING
        update_version
    fi
}


if [ -e "$VERSION_FILE" ]; then
    test_version
else
    echo create version: $VERSION_STRING
    update_version
fi


