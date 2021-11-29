#!/bin/bash

NAIVE_OUT_FILE="verifier.out"
NAIVE_FLOATS_PER_READ=1000

IMPL_OUT_FILE="impl.out"


file_name=$1

if [ -f "$NAIVE_OUT_FILE" ]; then
    rm $NAIVE_OUT_FILE
fi

echo "Running naive solver..."
./verifier $file_name $NAIVE_FLOATS_PER_READ > $NAIVE_OUT_FILE

if [ -f "$IMPL_OUT_FILE" ]; then
    rm $IMPL_OUT_FILE
fi

echo "Running solution..."
for i in {1..9}
do
    percentile=$((10*i))
    echo -n "$percentile. " >> $IMPL_OUT_FILE
    ./poc $file_name $percentile >> $IMPL_OUT_FILE
done

if diff $NAIVE_OUT_FILE $IMPL_OUT_FILE; then
    echo OK
    rm $NAIVE_OUT_FILE $IMPL_OUT_FILE
else
    echo VERIFICATION FAILED
fi

