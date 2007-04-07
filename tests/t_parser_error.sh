#!/bin/sh
#-*-utf-8-*-

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


TMPDSL=/tmp/parser_error

cat >"${TMPDSL}"<<EOF
#NAME "test"
#INDEX_LANGUAGE "English"
#CONTENTS_LANGUAGE "English"

a
 b
тру-ля-ля
 b
EOF

iconv -f utf-8 -t cp1251 "${TMPDSL}" > "${TMPDSL}.dsl" 

if "${PATH_TO_MAKEDICT}"/makedict -o dummy "${TMPDSL}.dsl" >/dev/null 2>&1; then
	echo "Parser do not report about error"
	exit 1
fi

