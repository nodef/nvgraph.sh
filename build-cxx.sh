#!/usr/bin/env bash
src="nvgraph.sh"
out="$HOME/Logs/$src.log"
ulimit -s unlimited
printf "" > "$out"

# Download program
if [[ "$DOWNLOAD" != "0" ]]; then
  rm -rf $src
  git clone https://github.com/nodef/$src
  cd $src
fi

# Fixed config
: "${TYPE:=float}"
# Define macros (dont forget to add here)
DEFINES=(""
"-DTYPE=$TYPE"
)

# Run
nvcc -std=c++17 -O3 -Xcompiler -lnvgraph -Xcompiler -fopenmp src/cxx/main.cu
stdbuf --output=L ./a.out sssp           -o=out.json -f ~/Data/indochina-2004.mtx -s=1             2>&1 | tee -a "$out"
stdbuf --output=L ./a.out pagerank       -o=out.json -f ~/Data/indochina-2004.mtx -a=0.85 -t=1e-6  2>&1 | tee -a "$out"
stdbuf --output=L ./a.out triangle-count -o=out.json -f ~/Data/indochina-2004.mtx                  2>&1 | tee -a "$out"
stdbuf --output=L ./a.out traversal-bfs  -o=out.json -f ~/Data/indochina-2004.mtx -s=1             2>&1 | tee -a "$out"
