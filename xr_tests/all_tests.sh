cd `dirname "${BASH_SOURCE[0]}"`
echo "Running tests..."
for f in ./**/*.xr; do
    ./run_test.sh $f 2> /dev/null
done
