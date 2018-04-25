#!/bin/sh
for x in ./samples/*; do
    echo "$x"
    ./ecc -e "$x" > /dev/null
done
