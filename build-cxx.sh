#!/usr/bin/env bash
nam="nvgraph.sh"
out="/home/resources/Documents/subhajit/$nam.log"
ulimit -s unlimited
printf "" > "$out"

# Download program
rm -rf $nam
git clone https://github.com/nodef/$nam
cd $nam

# Run
nvcc -std=c++17 -Xcompiler -lnvgraph -O3 src/cxx/main.cu
