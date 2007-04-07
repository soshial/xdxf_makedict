#!/bin/sh

#echo "$0 was started" 

if [ -z "$1" -o -z "$2" ]; then
	echo "Usage: $0 to from" >&2
	exit 1
fi

#TBL="dsl_ipa_tbl.txt"

TBL="${2}"

ipa_to_cpp() {
IFS="
"
    while read -r str
    do
	echo $str | awk -F '#' '{ line=$1; if (line != "") print line }' | \
	    awk -F '-' \
'{ first=$1; second=$2; \
sub(/^  */, "", first); \
sub(/^  */, "", second); \
sub(/  *$/, "", first); \
sub(/  *$/, "", second); \
printf "\t\tipa_to_unicode_make_pair(%s, %s),\n", first, $2 }'
    done < "${TBL}"
}

RES="${1}"
cat <<EOF > "${RES}"
#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif

#include "dsl_ipa.hpp"

static UniToStrPair ipa_to_unicode_make_pair(gunichar from,
                                             gunichar to)
{
        gchar buf[7];

        UniToStrPair res;
        res.first = from;
        buf[g_unichar_to_utf8(to, buf)] = '\0';
        res.second =buf;

        return res;
}

static UniToStrPair ipa_to_unicode_make_pair(gunichar from,
                                             gunichar to1,
                                             gunichar to2)
{
        gchar buf[7];
        
        UniToStrPair res;
        res.first = from;
        buf[g_unichar_to_utf8(to1, buf)] = '\0';
        res.second = buf;
        buf[g_unichar_to_utf8(to2, buf)] = '\0';
        res.second += buf;

        return res;
}

std::pair<UniToStrPair *, UniToStrPair *> ipa_to_unicode_tbl()
{
        static UniToStrPair ipa_to_unicode_tbl[] = {
EOF

ipa_to_cpp >> "${RES}"

cat <<EOF >> "${RES}"
       };

        return std::make_pair(ipa_to_unicode_tbl, ipa_to_unicode_tbl +
                              sizeof(ipa_to_unicode_tbl) / sizeof(UniToStrPair));
}
EOF


