#!/bin/sh

# test case 1
cksfv -f c1.sfv
if test "$?" != "0" ; then
    echo "test case 1 unsuccesful"
    exit -1
fi

