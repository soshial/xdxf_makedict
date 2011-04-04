/* 
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

#include <glib/gi18n.h>
#include "mapfile.hpp"
#include "file.hpp"
#include "lang_tbl.hpp"

std::vector<LangTblItem> lang_tbl;

/* return value:
 * true - parsed successfully
 * false - error */
static bool parse_lang_line(const char* line_beg, const char* line_end,
	LangTblItem& item)
{
	const char *p = line_beg;
	const char *q;
	q = p;
	while(p < line_end && *p != '|')
		++p;
	if(p >= line_end)
		return false;
	if(q + 3 != p)
		return false;
	item.code3.assign(q, 3);
	++p;
	q = p;
	while(p < line_end && *p != '|')
		++p;
	if(p >= line_end)
		return false;
	if(p == q+1)
		item.code2.clear();
	else {
		if(q + 2 != p)
			return false;
		item.code2.assign(q, 2);
	}
	++p;
	q = p;
	// skip trailing spaces
	p = line_end-1;
	while(line_beg <= p && (*p == ' ' || *p == '\t'))
		--p;
	if(q > p)
		return false;
	item.name.assign(q, 1 + p - q);
	return true;
}

/* language file format
 * file encoding - utf-8
 * Each line describes one language (not more).
 * Empty line are ignored.
 * A language may be described in a number of lines thus introducing alternative language names.
 * All lines describing one language should have identical first and second fields,
 * only the the third field (English language name) may differ.
 * Each line contains the following fields:
 * alpha-3 (bibliographic) code, alpha-2 code, English language name
 * alpha-2 code may be blank.
 * Fields are separated with '|'.
 * For example:
 * spa|es|Spanish
 * eng|en|English
 * apa||Apache languages
 * */
void load_languages(const char* file_path)
{
	MapFile in;
	if (!in.open(file_path)) {
		StdErr.printf(_("Can not open languages file %s\n"), file_path);
		return;
	}
	LangTblItem item;
	const char *p = in.begin();
	// skip BOM
	if(in.end()-p >= 3 && p[0] == '\xef' && p[1] == '\xbb' && p[2] == '\xbf')
		p += 3;
	const char *line_beg;
	const char *line_end;
	while(p < in.end()) {
		line_beg = p;
		// search end of line
		while(p < in.end() && *p != '\r' && *p != '\n')
			++p;
		line_end = p;
		// parse line
		if(line_beg < line_end)
			if(parse_lang_line(line_beg, line_end, item))
				lang_tbl.push_back(item);
			else {
				std::string t(line_beg, line_end - line_beg);
				StdErr.printf(_("Language file. Invalid line: %s\n"), t.c_str());
			}
		// goto next line
		while(p < in.end() && (*p == '\r' || *p == '\n'))
			++p;
	}
}
