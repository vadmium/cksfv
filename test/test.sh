#!/bin/sh

cksfv="../src/cksfv"

ret="0"

$cksfv -f c1.sfv >/dev/null 2>/dev/null
if test "$?" != "0" ; then
    echo "test case 1 unsuccessful. should report correct checksum."
    ret="1"
else
    echo "test case 1 successful"
fi

$cksfv -f c2.sfv >/dev/null 2>/dev/null
if test "$?" = "0" ; then
    echo "test case 2 unsuccessful. should report incorrect checksum."
    ret="1"
else
    echo "test case 2 successful"
fi

$cksfv a b c > tmp.sfv 2> /dev/null
if test "$?" != "0" ; then
    echo "test case 3 unsuccessful. sfv creation failed."
    ret="1"
else
    echo "test case 3 successful"
fi

$cksfv -f tmp.sfv >/dev/null 2>/dev/null
if test "$?" != "0" ; then
    echo "test case 4 unsuccessful. sfv checking failed."
    ret="1"
else
    echo "test case 4 successful"
fi

$cksfv a d >/dev/null 2>/dev/null
if test "$?" = "0" ; then
    echo "test case 5 unsuccessful. sfv creation should have failed."
    ret="1"
else
    echo "test case 5 successful"
fi

$cksfv -i -f c3.sfv >/dev/null 2>/dev/null
if test "$?" != "0" ; then
    echo "test case 6 unsuccessful. in-casesensitive checking failed."
    ret="1"
else
    echo "test case 6 successful"
fi

$cksfv -q -f c1.sfv 2>/dev/null > tmpfile
$cksfv -q -f c2.sfv 2>/dev/null >> tmpfile
$cksfv -q -i -f c3.sfv 2>/dev/null >> tmpfile
rm -f emptyfile
touch emptyfile
if test "`wc -l < tmpfile`" != "`wc -l < emptyfile`" ; then
    echo "test case 7 unsuccessful. tmpfile not empty."
    ret="1"
else
    echo "test case 7 successful"
fi

# Checksum code with illegal characters
$cksfv -f c4.sfv >/dev/null 2>/dev/null
if test "$?" = "0" ; then
    echo "test case 8 unsuccessful."
    ret="1"
else
    echo "test case 8 successful"
fi

# Checksum code with one too few characters
$cksfv -f c5.sfv >/dev/null 2>/dev/null
if test "$?" = "0" ; then
    echo "test case 9 unsuccessful."
    ret="1"
else
    echo "test case 9 successful"
fi

# Checksum code with one too many characters
$cksfv -f c6.sfv >/dev/null 2>/dev/null
if test "$?" = "0" ; then
    echo "test case 10 unsuccessful."
    ret="1"
else
    echo "test case 10 successful"
fi

# Partial checks
$cksfv a b c > tmp.sfv 2> /dev/null
$cksfv -f tmp.sfv a b d > /dev/null 2> /dev/null
if test "$?" = "0" ; then
    echo "test case 11 unsuccessful."
    ret="1"
else
    echo "test case 11 successful"
fi

# Partial checks
$cksfv -f tmp.sfv c > /dev/null 2> /dev/null
if test "$?" != "0" ; then
    echo "test case 12 unsuccessful."
    ret="1"
else
    echo "test case 12 successful"
fi

ln -sfn ../dir4 dir1/link 2> /dev/null
ln -sfn ../dir3 dir2/link 2> /dev/null

# Recursive without symlinks, successful
$cksfv -C dir1 -r > /dev/null 2> /dev/null
if test "$?" != "0" ; then
    echo "test case 13 unsuccessful. should report correct checksum."
    ret="1"
else
    echo "test case 13 successful"
fi

# Recursive without symlinks, unsuccessful
$cksfv -C dir2 -r > /dev/null 2> /dev/null
if test "$?" = 0 ; then
    echo "test case 14 unsuccessful. should report incorrect checksum."
    ret="1"
else
    echo "test case 14 successful"
fi

# Recursive with symlinks, unsuccessful
$cksfv -C dir1 -r -L > /dev/null 2> /dev/null
if test "$?" = "0" ; then
    echo "test case 15 unsuccessful. should report correct checksum."
    ret="1"
else
    echo "test case 15 successful"
fi

# Recursive with symlinks, unsuccessful
$cksfv -C dir2 -r -L > /dev/null 2> /dev/null
if test "$?" = 0 ; then
    echo "test case 16 unsuccessful. should report incorrect checksum."
    ret="1"
else
    echo "test case 16 successful"
fi

# Test backslash transformation
$cksfv -s -f c7.sfv > /dev/null 2> /dev/null
if test "$?" != 0 ; then
    echo "test case 17 unsuccessful. should report incorrect behavior."
    ret="1"
else
    echo "test case 17 successful"
fi

$cksfv -g dir3/c1.sfv >/dev/null 2>/dev/null
if test "$?" != "0" ; then
    echo "test case 18 unsuccessful. should report correct checksum."
    ret="1"
else
    echo "test case 18 successful"
fi

$cksfv -f nullbyte.sfv >/dev/null 2>/dev/null
if test "$?" != "0" ; then
    echo "test case 19 unsuccessful. should report correct checksum."
    ret="1"
else
    echo "test case 19 successful"
fi

$cksfv -f nullbyte2.sfv >/dev/null 2>/dev/null
if test "$?" != "0" ; then
    echo "test case 20 unsuccessful. should report correct checksum."
    ret="1"
else
    echo "test case 20 successful"
fi

$cksfv -R recursive-dir-1 >rec.sfv 2>/dev/null
if test "$?" != "0" ; then
    echo "test case 21 unsuccessful. should report correct checksum."
    ret="1"
else
    $cksfv -f rec.sfv >/dev/null 2>/dev/null
    if test "$?" != "0" ; then
	echo "test case 21 unsuccessful. should report correct checksum."
	ret="1"
    else
	echo "test case 21 successful"
    fi
fi

if test "$ret" != "0" ; then
    echo "One or more of the tests were unsuccessful. Please report."
    exit 1
fi
