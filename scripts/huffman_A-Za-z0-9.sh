#!/bin/bash

echo " ---------------------\"A-Za-z0-9\" test---------------------"
let cnt=1000
while [ $cnt -le 10000000 ]
do
  let i=0
  let size_sum=0
  let compressed_sum=0
  while [ $i -le 100 ]
  do
    cat /dev/urandom | head -c $cnt | tr -dc \"A-Za-z0-9\" | sort | uniq > test_file
    ./huffman --src test_file --dest test_deflated
    ./huffman -d --src test_deflated --dest test_inflated
    diff -q test_file test_inflated
    status=$?
    if ! [ $status -eq 0 ]
    then
      echo "INCORRECT COMPRESSION OR DECOMPRESSION! ABORTING!!!"
      break
    fi
    let size=$(wc --bytes test_file)
    let compressed=$(wc --bytes test_deflated)
    ((size_sum += $size))
    ((compressed_sum += $compressed))
#    echo "Compression: $compressed from $size"
    ((i++))
  done
  echo "  Average compression rate for approximately $(($cnt * 24 / 100)) bytes:"
  echo "$compressed_sum / $size_sum" | bc -l | sed 's/^/      /'
  echo ""
  ((cnt *= 2))
done
echo " ------------------\"A-Za-z0-9\" test done!------------------"