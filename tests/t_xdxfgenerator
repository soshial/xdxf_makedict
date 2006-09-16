#!/bin/sh

source funcs.sh
set_md_plugin_dir

TESTDIR=/tmp/xdxf_generator_test1

rm -fr $TESTDIR
mkdir $TESTDIR

../src/makedict -i xdxf -o xdxf --work-dir $TESTDIR sample-dicts/xdxfgenerator_test1.xdxf

if ! diff -u $TESTDIR/sample-dicts/dict.xdxf sample-dicts/xdxfgenerator_test1_std.xdxf; then
		exit 1
fi

rm -fr $TESTDIR

exit 0
