#!/usr/bin/env bash
dir="$(dirname "$(readlink -f "$0")")"

help() {
  fil="$dir/man/$1.txt"
  url="https://github.com/nodef/nvgraph.sh/issues"
  if   [[ "$1" == "" ]]; then less "$dir/man/help.txt"
  elif [ -f "$fil" ];    then less "$fil"
  else echo "No help found for '$1'. You can report this at ${url}"; fi
}


# Call nvgraph
if   [[ "$2" == "--help" ]]; then help "$1"
elif [[ "$1" == "--help" ]]; then help
elif [[ "$1" == "help" ]];   then help "$2"
else "$dir/nvgraph" "$@"; fi
