#!/bin/sh
for x in ./samples/*; do
    echo "$x"
    ./parse_dump "$x" > /dev/null
done
