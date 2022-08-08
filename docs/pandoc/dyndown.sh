#!/bin/bash

if [ "$#" -ne 2 ]; then
	echo "Usage: ./dyndown.sh <input.md> <output.md>"
	exit 0
fi

tmp="$2"
cp -f "$1" "$tmp"

echo "Replacing dynamic blocks in '$1', writing to '$tmp'..."

command=""
find_exec_block() {
	command=$(sed -rn '0,/\@EXEC\(([^)]*)\)/ s/\@EXEC\(([^)]*)\)/\1/p' "$tmp")
}

find_exec_block
while [ -n "$command" ]; do
	out=$(eval "$command")
	out="${out//\\/\\\\}"
	out="${out//\//\\/}"
	out="${out//&/\\&}"
	out="${out//$'\n'/\\n}"
	sed -i '0,/\@EXEC\(([^)]*)\)/ s/\@EXEC\(([^)]*)\)/```\n'"$out"'\n```/' "$tmp" && echo "* Replaced @EXEC(...) block for '$command'"
	find_exec_block
done

echo "Done replacing @EXEC(...) blocks in '$1'"