#!/bin/sh

cksfv="../src/cksfv"

# test case 1
$cksfv -f c1.sfv >/dev/null 2>/dev/null
if test "$?" != "0" ; then
    echo "test case 1 unsuccessful. should report correct checksum."
    exit -1
fi
echo test case 1 successful

# test case 2
$cksfv -f c2.sfv >/dev/null 2>/dev/null
if test "$?" = "0" ; then
    echo "test case 2 unsuccessful. should report incorrect checksum."
    exit -1
fi
echo test case 2 successful

# test case 3
$cksfv a b c > tmp.sfv 2> /dev/null
if test "$?" != "0" ; then
    echo "test case 3 unsuccessful. sfv creation failed."
    exit -1
fi
$cksfv -f tmp.sfv >/dev/null 2>/dev/null
if test "$?" != "0" ; then
    echo "test case 3 unsuccessful. sfv checking failed."
    exit -1
fi
echo test case 3 successful

# test case 4
$cksfv a d >/dev/null 2>/dev/null
if test "$?" = "0" ; then
    echo "test case 3 unsuccessful. sfv creation should have failed."
    exit -1
fi
echo test case 4 successful

# test case 5
$cksfv -i -f c3.sfv >/dev/null 2>/dev/null
if test "$?" != "0" ; then
    echo "test case 1 unsuccessful. in-casesensitive checking failed."
    exit -1
fi
echo test case 5 successful

# test case 6
$cksfv -q -f c1.sfv 2>/dev/null > tmpfile
$cksfv -q -f c2.sfv 2>/dev/null >> tmpfile
$cksfv -q -i -f c3.sfv 2>/dev/null >> tmpfile
rm -f emptyfile
touch emptyfile
if test "`wc -l < tmpfile`" != "`wc -l < emptyfile`" ; then
    echo "test case 6 unsuccessful. tmpfile not empty."
    exit -1
fi
echo test case 6 successful

# tset case 7 (checksum code with illegal characters)
$cksfv -f c4.sfv >/dev/null 2>/dev/null
if test "$?" = "0" ; then
    echo "test case 7 unsuccessful."
    exit -1
fi
echo test case 7 successful

# test case 8 (checksum code with one too few characters)
$cksfv -f c5.sfv >/dev/null 2>/dev/null
if test "$?" = "0" ; then
    echo "test case 8 unsuccessful."
    exit -1
fi
echo test case 8 successful

# test case 9 (checksum code with one too many characters)
$cksfv -f c6.sfv >/dev/null 2>/dev/null
if test "$?" = "0" ; then
    echo "test case 9 unsuccessful."
    exit -1
fi
echo test case 9 successful

# test case 10 (partial checks)
$cksfv a b c > tmp.sfv 2> /dev/null
$cksfv -f tmp.sfv a b d > /dev/null 2> /dev/null
if test "$?" = "0" ; then
    echo "test case 10 unsuccessful."
    exit -1
fi
$cksfv -f tmp.sfv c > /dev/null 2> /dev/null
if test "$?" != "0" ; then
    echo "test case 10 unsuccessful."
    exit -1
fi
echo test case 10 successful

ln -sfn ../dir3 dir1/link
ln -sfn ../dir3 dir2/link

# test case 11 (recursive without symlinks, successful)
$cksfv -C dir1 -r > /dev/null 2> /dev/null
if test "$?" != "0" ; then
    echo "test case 11 unsuccessful. should report correct checksum."
    exit -1
fi
echo test case 11 successful

# test case 12 (recursive without symlinks, unsuccessful)
$cksfv -C dir2 -r > /dev/null 2> /dev/null
if test "$?" = 0 ; then
    echo "test case 12 unsuccessful. should report incorrect checksum."
    exit -1
fi
echo test case 12 successful

# test case 13 (recursive with symlinks, successful)
$cksfv -C dir1 -r -L > /dev/null 2> /dev/null
if test "$?" != "0" ; then
    echo "test case 13 unsuccessful. should report correct checksum."
    exit -1
fi
echo test case 13 successful

# test case 14 (recursive with symlinks, unsuccessful)
$cksfv -C dir2 -r -L > /dev/null 2> /dev/null
if test "$?" = 0 ; then
    echo "test case 14 unsuccessful. should report incorrect checksum."
    exit -1
fi
echo test case 14 successful
