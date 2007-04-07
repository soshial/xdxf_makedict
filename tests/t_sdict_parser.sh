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

convert() {
	if ! "${PATH_TO_MAKEDICT}/makedict" --verbose=3 --parser-option "remove-duplication=yes" -i sdict -o xdxf --work-dir /tmp "${1}"; then
		echo "makedict return error" >&2
		exit 1
	fi

	if ! diff -u "${2}/dict.xdxf" "${3}"; then
		echo "${2}/dict.xdxf ${3} are not the same" >&2
		exit 1
	fi

	rm -fr "${2}"
}

convert "${CMAKE_CURRENT_SOURCE_DIR}/sample-dicts/sample1.dct" /tmp/sample1 "${CMAKE_CURRENT_SOURCE_DIR}/sample-dicts/sample1_sdict_std.xdxf"
convert "${CMAKE_CURRENT_SOURCE_DIR}/sample-dicts/sample2.dct" /tmp/sample2 "${CMAKE_CURRENT_SOURCE_DIR}/sample-dicts/sample2_sdict_std.xdxf"
