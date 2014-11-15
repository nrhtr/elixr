#!/bin/sh
file=$1

out=`cat .prelude.xr - .finale.xr < $file | ../../parse_dump 2>> stderr.log`
expect=`cat $file | sed 's/#!.*//; /\#/!d; s/.*\#//'`
if [ "$out" != "$expect" ]; then
    echo "$file: expected <$expect>, but got <$out>";
else
    echo -n .
fi
