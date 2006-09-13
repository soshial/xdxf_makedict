#!/bin/sh

DATA=sample-dicts/dummy_simple_input.txt
RES=makedict_parser_options_test

if ! ../src/makedict -i dummy -o xdxf --parser-option "lang_from=eng" \
	--parser-option "lang_to=eng" --work-dir /tmp "${RES}" < "${DATA}"; then
	echo "makedict return error" >&2
	exit 1
fi

if ! diff "/tmp/${RES}/dict.xdxf" sample-dicts/parser_options_std.xdxf; then
	echo "/tmp/${RES}/dict.xdxf sample-dicts/parser_options_std.xdxf not the same" >&2
	exit 1
fi

rm -fr "/tmp/${RES}"
