#!/usr/bin/env bash
dir="$(dirname "$(readlink -f "$0")")"

# Call nvgraph
"$dir/nvgraph" "$@"
