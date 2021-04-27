#!/usr/bin/env bash
dir="$(dirname "$(readlink -f "$0")")"

echo "$dir/nvgraph" "$@"
"$dir/nvgraph" "$@"
