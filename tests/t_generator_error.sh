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


rm -fr /tmp/sample-dicts
mkdir /tmp/sample-dicts
touch /tmp/sample-dicts/dict.xdxf
chmod u-w /tmp/sample-dicts/dict.xdxf

if ! "${PATH_TO_MAKEDICT}/makedict" -i xdxf -o xdxf \
		--work-dir /tmp "${CMAKE_CURRENT_SOURCE_DIR}/sample-dicts/xdxfgenerator_test1.xdxf"
then
	rm -fr /tmp/sample-dicts
	exit 0
fi

echo "makedict do not return error, that not possible write to /tmp/sample-dicts/dict.xdxf" >&2
exit 1
