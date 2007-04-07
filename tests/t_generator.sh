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

source "${CMAKE_CURRENT_SOURCE_DIR}/funcs.sh"

#set_md_plugin_dir
create_signle_dummy_dir
CREATER="${PATH_TO_MAKEDICT}/makedict -i xdxf -o dummy --work-dir work-directory"

check() {
		$CREATER  $1 > $2
		if ! diff -u $3 $2; then
				echo $4 >&2
				exit 1
		fi		
		rm -f $2
}

check "${CMAKE_CURRENT_SOURCE_DIR}/sample-dicts/opt_tag.xdxf" "/tmp/a_dummy.txt" "${CMAKE_CURRENT_SOURCE_DIR}/sample-dicts/opt_tag_dummy_standard.txt" "opt tag test failed"

check "${CMAKE_CURRENT_SOURCE_DIR}/sample-dicts/quote_amp_etc.xdxf" "/tmp/a_dummy.txt" "${CMAKE_CURRENT_SOURCE_DIR}/sample-dicts/quote_amp_etc_standard.dummy" "quote, amp, gt, lt test failed"

check "${CMAKE_CURRENT_SOURCE_DIR}/sample-dicts/tag_attribute.xdxf" "/tmp/a_dummy.txt" "${CMAKE_CURRENT_SOURCE_DIR}/sample-dicts/tag_attribute_standard.dummy" "tag attribute test failed"

export CREATER="${PATH_TO_MAKEDICT}/makedict -i xdxf -o single-dummy --work-dir work-directory"
check "${CMAKE_CURRENT_SOURCE_DIR}/sample-dicts/opt_tag.xdxf" "/tmp/a_dummy.txt" "${CMAKE_CURRENT_SOURCE_DIR}/sample-dicts/opt_tag_dummy_standard.txt" "opt tag test failed"
check "${CMAKE_CURRENT_SOURCE_DIR}/sample-dicts/quote_amp_etc.xdxf" "/tmp/a_dummy.txt" "${CMAKE_CURRENT_SOURCE_DIR}/sample-dicts/quote_amp_etc_standard.dummy" "quote, amp, gt, lt test failed"

check "${CMAKE_CURRENT_SOURCE_DIR}/sample-dicts/tag_attribute.xdxf" "/tmp/a_dummy.txt" "${CMAKE_CURRENT_SOURCE_DIR}/sample-dicts/tag_attribute_standard.dummy" "tag attribute test failed"

delete_single_dummy_dir

exit 0
