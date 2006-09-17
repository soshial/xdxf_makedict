#!/bin/sh

rm -fr /tmp/sample-dicts
mkdir /tmp/sample-dicts
touch /tmp/sample-dicts/dict.xdxf
chmod u-w /tmp/sample-dicts/dict.xdxf

if ! ../src/makedict -i xdxf -o xdxf --work-dir /tmp sample-dicts/xdxfgenerator_test1.xdxf
then
	rm -fr /tmp/sample-dicts
	exit 0
fi

echo "makedict do not return error, that not possible write to /tmp/sample-dicts/dict.xdxf" >&2
exit 1
