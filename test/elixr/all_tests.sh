#!/bin/sh
echo "Running tests..."
for f in ./**/*.xr; do
    ./run_test.sh $f
done
