#!/bin/sh

CMAKE_CURRENT_SOURCE_DIR="."
if [ ! -z "$1" ]; then
		CMAKE_CURRENT_SOURCE_DIR="$1"
fi

PATH_TO_MAKEDICT="../src"
if [ ! -z "$2" ]; then
		PATH_TO_MAKEDICT="$2"
fi

source "${CMAKE_CURRENT_SOURCE_DIR}/funcs.sh"
set_md_plugin_dir

RES=/tmp/dsl-test.dummy

check() {
		"${PATH_TO_MAKEDICT}/makedict" -i dsl -o dummy "$1" > "$2"

		if ! diff -u "$2" "$3"; then
				echo "$0 failed with data: $1, see $RES" >&2
				exit 1
		fi
		rm -f $RES
}

check ${CMAKE_CURRENT_SOURCE_DIR}/sample-dicts/skip_comments.dsl $RES ${CMAKE_CURRENT_SOURCE_DIR}/sample-dicts/skip_comments_dsl_std.dummy 
check ${CMAKE_CURRENT_SOURCE_DIR}/sample-dicts/ref_param.dsl $RES ${CMAKE_CURRENT_SOURCE_DIR}/sample-dicts/ref_param_std.dummy
check ${CMAKE_CURRENT_SOURCE_DIR}/sample-dicts/right_tag_order.dsl $RES ${CMAKE_CURRENT_SOURCE_DIR}/sample-dicts/right_tag_order_std.dummy
check ${CMAKE_CURRENT_SOURCE_DIR}/sample-dicts/col_param.dsl $RES ${CMAKE_CURRENT_SOURCE_DIR}/sample-dicts/col_param_std.dummy
check ${CMAKE_CURRENT_SOURCE_DIR}/sample-dicts/dsl_transc.dsl $RES ${CMAKE_CURRENT_SOURCE_DIR}/sample-dicts/dsl_transc_std.dummy
check ${CMAKE_CURRENT_SOURCE_DIR}/sample-dicts/opt_nu.dsl $RES ${CMAKE_CURRENT_SOURCE_DIR}/sample-dicts/opt_nu_std.dummy

exit 0
