#!/bin/sh

if ! ../src/makedict --verbose=3 -i stardict -o dummy sample-dicts/stardict-dummy.ifo
then
	echo "makedict return error"
	exit 1
fi
