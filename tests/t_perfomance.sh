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


RES=/tmp/makedict_performance.dsl
OUTPUT=/tmp/makedict_performance.dummy
STD=/tmp/makedict_performance_std.dummy
rm -f "${RES}" "${OUTPUT}" "${STD}"

for ((i = 0; i < 60000; ++i)); do
	echo $i >> "${RES}"
	echo " $i" >> "${RES}"
	echo " $i" >> "${RES}"
	echo "key: $i" >> "${STD}"
	echo "data: <k>$i</k>" >> "${STD}"
	echo "$i" >> "${STD}"
	echo "$i" >> "${STD}"
done

time "${PATH_TO_MAKEDICT}/makedict" -i dsl -o dummy --parser-option "full_name=a" \
	--parser-option "lang_from=eng" --parser-option "lang_to=eng" "${RES}" > "${OUTPUT}"

if (($? != 0)); then
	echo "makedict return error" >&2
	exit 1
fi

if ! diff -u "${OUTPUT}" "${STD}"; then
	echo "${OUTPUT} ${STD} are not the same" >&2
	exit 1
fi

rm -f "${RES}" "${OUTPUT}" "${STD}"
