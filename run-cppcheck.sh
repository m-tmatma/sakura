#!/bin/bash

export platform=$1
export configuration=$2
export CPPCHECK_OUT=cppcheck-${platform}-${configuration}.xml
export CPPCHECK_LOG=cppcheck-${platform}-${configuration}.log
export CPPCHECK_PLATFORM=
if [ ${platform} = "Win32" ]; then
	export CPPCHECK_PLATFORM=win32W
elif [ ${platform} = "x64" ]; then
	export CPPCHECK_PLATFORM=win64
else
	exit 1
fi

if [ -e ${CPPCHECK_OUT} ]; then
    rm $CPPCHECK_OUT
fi

if [ -e ${CPPCHECK_LOG} ]; then
    rm $CPPCHECK_LOG
fi

export CPPCHECK_PARAMS=
export "CPPCHECK_PARAMS=${CPPCHECK_PARAMS} --force"
export "CPPCHECK_PARAMS=${CPPCHECK_PARAMS} --enable=all"
export "CPPCHECK_PARAMS=${CPPCHECK_PARAMS} --xml"
export "CPPCHECK_PARAMS=${CPPCHECK_PARAMS} --platform=${CPPCHECK_PLATFORM}"
#export "CPPCHECK_PARAMS=${CPPCHECK_PARAMS} --output-file=${CPPCHECK_OUT}"
export "CPPCHECK_PARAMS=${CPPCHECK_PARAMS} ./sakura_core"

echo cppcheck ${CPPCHECK_PARAMS}
     cppcheck ${CPPCHECK_PARAMS} 2> ${CPPCHECK_OUT}

