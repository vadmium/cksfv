#!/bin/sh

cksfv="../src/cksfv"

# test case 1
$cksfv -f c1.sfv >/dev/null 2>/dev/null
if test "$?" != "0" ; then
    echo "test case 1 unsuccesful. should report correct checksum."
    exit -1
fi
echo test case 1 succesful

# test case 2
$cksfv -f c2.sfv >/dev/null 2>/dev/null
if test "$?" = "0" ; then
    echo "test case 2 unsuccesful. should report incorrect checksum."
    exit -1
fi
echo test case 2 succesful

# test case 3
$cksfv a b c > tmp.sfv 2> /dev/null
if test "$?" != "0" ; then
    echo "test case 3 unsuccesful. sfv creation failed."
    exit -1
fi
$cksfv -f tmp.sfv >/dev/null 2>/dev/null
if test "$?" != "0" ; then
    echo "test case 3 unsuccesful. sfv checking failed."
    exit -1
fi
echo test case 3 succesful

# test case 4
$cksfv a d >/dev/null 2>/dev/null
if test "$?" = "0" ; then
    echo "test case 3 unsuccesful. sfv creation should have failed."
    exit -1
fi
echo test case 4 succesful

# test case 5
$cksfv -i -f c3.sfv >/dev/null 2>/dev/null
if test "$?" != "0" ; then
    echo "test case 1 unsuccesful. in-casesensitive checking failed."
    exit -1
fi
echo test case 5 succesful

# test case 6
$cksfv -q -f c1.sfv 2>/dev/null > tmpfile
$cksfv -q -f c2.sfv 2>/dev/null >> tmpfile
$cksfv -q -i -f c3.sfv 2>/dev/null >> tmpfile
rm -f emptyfile
touch emptyfile
if test "`wc -l < tmpfile`" != "`wc -l < emptyfile`" ; then
    echo "test case 6 unsuccesful. tmpfile not empty."
    exit -1
fi
echo test case 6 succesful

# tset case 7 (checksum code with illegal characters)
$cksfv -f c4.sfv >/dev/null 2>/dev/null
if test "$?" = "0" ; then
    echo "test case 7 unsuccesful."
    exit -1
fi
echo test case 7 succesful

# tset case 8 (checksum code with one too few characters)
$cksfv -f c5.sfv >/dev/null 2>/dev/null
if test "$?" = "0" ; then
    echo "test case 8 unsuccesful."
    exit -1
fi
echo test case 8 succesful
