#!/bin/bash

echo "--------------------Huffman Test started--------------------"
scripts/huffman_0-9.sh
scripts/huffman_A-Z.sh
scripts/huffman_A-Za-z.sh
scripts/huffman_A-Za-z0-9.sh
rm test_file test_deflated test_inflated
echo "---------------------Huffman Test done!---------------------"