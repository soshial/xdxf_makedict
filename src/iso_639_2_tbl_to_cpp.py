#!/usr/bin/env python
# -*- coding: utf-8 -*-
import sys, re

def convert_to_cpp_str(text):
	'''Convert utf-8-encoded string text into a c++ string literal
	Characters with codes >= 128 are represented with octal escape sequences.
	Backslashes and double quotes are escaped.
	'''
	res = '"'
	for c in text:
		code = ord(c)
		if code >= 128:
			res += "\\{0:0>3o}".format(code)
		elif c == '\\':
			res += '\\\\'
		elif c == '"':
			res += '\\"'
		else:
			res += c
	res += '"'
	return res

print \
"""/* 
 * WARNING generated automatically by
 * cat ISO-639-2_utf-8.txt | iso_639_2_tbl_to_cpp.py  > langs_tbl.cpp
 * DO not edit
 *
 * To get ISO-639-2_utf-8.txt follow the link
 * http://www.loc.gov/standards/iso639-2/ascii_8bits.html
 * Download the version in UTF-8 encoding. 
*/

#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif

#include <cstddef>
#include "lang_tbl.hpp"

LangTblItem lang_tbl[] = {"""

contents = sys.stdin.read()
if contents.startswith("\xEF\xBB\xBF"): # UTF-8 BOM
	contents = contents[3:]
for line in re.split("[\r\n]+", contents):
	if line == '':
		continue
	parts = line.split('|')
	if (len(parts[0]) != 3):
		# for example, "qaa-qtz"
		continue
	code3 = convert_to_cpp_str(parts[0])
	code2 = convert_to_cpp_str(parts[2])
	names = parts[3].split(';')
	for name in names:
		name = name.strip()
		if name == '':
			continue
		name = convert_to_cpp_str(name)
		print "\t{{ {0}, {1}, {2} }},".format(code3, code2, name)

print \
"""	{ NULL, NULL, NULL }
};
"""
