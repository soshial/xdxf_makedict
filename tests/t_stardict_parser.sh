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

FILE=/tmp/dummy_parser_input
NWORDS=10
TMP_RES=/tmp/dummy_output.res
TMP_STD=/tmp/dummy_output.std

rm -f "${FILE}" "${FILE}.index"
for ((i = 0; i < NWORDS; ++i)); do
	echo $i 
done >> "${FILE}.index"

sort "${FILE}.index" > "${FILE}"
rm -f "${FILE}.index"

cat $FILE | "${PATH_TO_MAKEDICT}/makedict" -i dummy -o stardict --work-dir /tmp $FILE
cat $FILE | "${PATH_TO_MAKEDICT}/makedict" -i dummy -o dummy --work-dir /tmp $FILE > "${TMP_STD}"

RES=/tmp/stardict-dummy_parser_input-2.4.2

if ! "${PATH_TO_MAKEDICT}/makedict" -i stardict -o dummy "${RES}"/dummy_parser_input.ifo \
 > "${TMP_RES}"
then
	echo "makedict return error" >&2
	exit 1
fi

if ! diff -u "${TMP_RES}" "${TMP_STD}"
then
	echo "${TMP_RES} ${TMP_STD} are not the same" >&2
	exit 1
fi

rm -fr "${RES}"
rm -f "${TMP_RES}" "${TMP_STD}" "${FILE}"
