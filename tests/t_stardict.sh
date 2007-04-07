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

FILE=/tmp/dummy_parser_input
NWORDS=10

generate_file $NWORDS $FILE

cat $FILE | "${PATH_TO_MAKEDICT}/makedict" -i dummy -o stardict --work-dir /tmp $FILE

RES=/tmp/stardict-dummy_parser_input-2.4.2

if ! "${CMAKE_CURRENT_BINARY_DIR}/validate_stardict" `ls "${RES}"/*.ifo`; then
	echo "test failed, see $FILE and $RES" >&2
	exit 1
fi

rm -fr "${RES}"
rm -f $FILE

exit $res
