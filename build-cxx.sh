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
stdbuf --output=L ./a.out sssp           -o=out.json -f ~/data/web-Google.mtx -s=1             2>&1 | tee -a "$out"
stdbuf --output=L ./a.out pagerank       -o=out.json -f ~/data/web-Google.mtx -a=0.85 -t=1e-6  2>&1 | tee -a "$out"
stdbuf --output=L ./a.out triangle-count -o=out.json -f ~/data/web-Google.mtx                  2>&1 | tee -a "$out"
stdbuf --output=L ./a.out traversal-bfs  -o=out.json -f ~/data/web-Google.mtx -s=1             2>&1 | tee -a "$out"
