#!/bin/sh

CMAKE_CURRENT_SOURCE_DIR="."
if [ ! -z "$1" ]; then
		CMAKE_CURRENT_SOURCE_DIR="$1"
fi

PATH_TO_MAKEDICT="../src"
if [ ! -z "$2" ]; then
		PATH_TO_MAKEDICT="$2/.."
fi

source "${CMAKE_CURRENT_SOURCE_DIR}/funcs.sh"

set_md_plugin_dir

TESTDIR=/tmp/xdxf_generator_test1

rm -fr $TESTDIR
mkdir $TESTDIR

"${PATH_TO_MAKEDICT}/makedict" -i xdxf -o xdxf \
		--work-dir $TESTDIR \
		${CMAKE_CURRENT_SOURCE_DIR}/sample-dicts/xdxfgenerator_test1.xdxf

if ! diff -u $TESTDIR/sample-dicts/dict.xdxf ${CMAKE_CURRENT_SOURCE_DIR}/sample-dicts/xdxfgenerator_test1_std.xdxf; then
		exit 1
fi

rm -fr $TESTDIR

exit 0
