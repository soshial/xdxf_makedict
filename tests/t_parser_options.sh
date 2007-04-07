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
set_md_plugin_dir

DATA="${CMAKE_CURRENT_SOURCE_DIR}/sample-dicts/dummy_simple_input.txt"
RES=makedict_parser_options_test

if ! "${PATH_TO_MAKEDICT}/makedict" -i dummy -o xdxf --parser-option "lang_from=eng" \
	--parser-option "lang_to=eng" --work-dir /tmp "${RES}" < "${DATA}"; then
	echo "makedict return error" >&2
	exit 1
fi

if ! diff -u "/tmp/${RES}/dict.xdxf" "${CMAKE_CURRENT_SOURCE_DIR}/sample-dicts/parser_options_std.xdxf"; then
	echo "/tmp/${RES}/dict.xdxf ${CMAKE_CURRENT_SOURCE_DIR}/sample-dicts/parser_options_std.xdxf not the same" >&2
	exit 1
fi

rm -fr "/tmp/${RES}"
