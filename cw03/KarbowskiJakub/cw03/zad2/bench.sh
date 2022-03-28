#!/usr/bin/env sh

for n in $(seq 1 30)
do
  echo "n = $n"
  (time -p "$1" 0.0000000005 "$n" > /dev/null) 2>&1
done

echo ''

for dx in 0.0000001 0.00000001 0.000000001 0.0000000005
do
  echo "dx = $dx"
  (time -p "$1" "$dx" 1 > /dev/null) 2>&1
done
