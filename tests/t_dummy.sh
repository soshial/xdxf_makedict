#!/bin/sh

CMAKE_CURRENT_SOURCE_DIR="."
CMAKE_CURRENT_BINARY_DIR="."
if [ ! -z "$1" ]; then
		CMAKE_CURRENT_SOURCE_DIR="$1"
fi


PATH_TO_MAKEDICT="../src"
if [ ! -z "$2" ]; then
		CMAKE_CURRENT_BINARY_DIR="$2"
		PATH_TO_MAKEDICT="$2/.."
fi

source "${CMAKE_CURRENT_SOURCE_DIR}/funcs.sh"

create_signle_dummy_dir
FILE="${CMAKE_CURRENT_SOURCE_DIR}/sample-dicts/dummy_simple_input.txt"
STANDARD="${CMAKE_CURRENT_SOURCE_DIR}/sample-dicts/dummy_simple_output.txt"
OUT=/tmp/output

convert() {
	cat $FILE | "${PATH_TO_MAKEDICT}/makedict" -i $1 -o $2 - > $OUT

	if ! diff -u $STANDARD $OUT; then
		echo "conversation from $1 to $2 failed" >&2
		echo "FAILURE: $STANDARD $OUT is not the same" >&2
		exit 1
	fi
	rm -f $OUT
}

convert dummy dummy
convert single-dummy dummy
convert dummy single-dummy
convert single-dummy single-dummy

rm -fr "${MAKEDICT_PLUGIN_DIR}"
