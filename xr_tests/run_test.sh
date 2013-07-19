#!/bin/sh
file=$1

out=`cat .prelude.xr - .finale.xr < $file | ../parse_dump 2> /dev/null`
expect=`cat $file | sed '/\#-/!d; s/.*\#-//'`
if [ "$out" != "$expect" ]; then
    echo "$file: expected <$expect>, but got <$out>";
else
    echo -n .
fi
