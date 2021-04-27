#!/usr/bin/env bash
dir="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"

echo "$dir/nvgraph" "$@"
"$dir/nvgraph" "$@"
