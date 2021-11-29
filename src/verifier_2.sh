#!/bin/bash

set -e
for i in {1..99}
do
  expected=$(sed -n ${i}'p' verifier_1_99.txt)
  echo "PERCENTILE: ${i}:"

  if diff <(echo $expected) <(./poc $1 $i $2) ; then
    echo "ok"
  else
    echo "nok"
  fi
done
