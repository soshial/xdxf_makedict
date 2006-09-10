#!/bin/sh
#-*-utf-8-*-

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

if ../src/makedict -o dummy "${TMPDSL}.dsl" >/dev/null 2>&1; then
	echo "FAILED: Parser do not report about error"
	exit 1
fi

