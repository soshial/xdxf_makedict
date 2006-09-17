#!/bin/sh

source funcs.sh
#set_md_plugin_dir
create_signle_dummy_dir
CREATER="../src/makedict -i xdxf -o dummy --work-dir work-directory"

check() {
		$CREATER  $1 > $2
		if ! diff -u $3 $2; then
				echo $4 >&2
				exit 1
		fi		
		rm -f $2
}

check "./sample-dicts/opt_tag.xdxf" "/tmp/a_dummy.txt" "./sample-dicts/opt_tag_dummy_standard.txt" "opt tag test failed"

check "./sample-dicts/quote_amp_etc.xdxf" "/tmp/a_dummy.txt" "./sample-dicts/quote_amp_etc_standard.dummy" "quote, amp, gt, lt test failed"

check "./sample-dicts/tag_attribute.xdxf" "/tmp/a_dummy.txt" "./sample-dicts/tag_attribute_standard.dummy" "tag attribute test failed"

export CREATER="../src/makedict -i xdxf -o single-dummy --work-dir work-directory"
check "./sample-dicts/opt_tag.xdxf" "/tmp/a_dummy.txt" "./sample-dicts/opt_tag_dummy_standard.txt" "opt tag test failed"
check "./sample-dicts/quote_amp_etc.xdxf" "/tmp/a_dummy.txt" "./sample-dicts/quote_amp_etc_standard.dummy" "quote, amp, gt, lt test failed"

check "./sample-dicts/tag_attribute.xdxf" "/tmp/a_dummy.txt" "./sample-dicts/tag_attribute_standard.dummy" "tag attribute test failed"

delete_single_dummy_dir

exit 0
