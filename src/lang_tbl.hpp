#ifndef _LANG_TBL_HPP_
#define _LANG_TBL_HPP_

#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif
#include <string>
#include <vector>

/* Codes for the Representation of Names of Languages
 * according to ISO 639.2 standard http://www.loc.gov/standards/iso639-2/
 * code2 and code3 fields may contain only ASCII letters.
 * name - a utf-8 string.
 *
 * Unfortunately, there is no portable way to embed a string literal
 * containing non-ASCII characters into C++ source code.
 * How compiler reads C++ source files? What encoding is used?
 * What encoding will be used for strings at runtime?
 * To mitigate string encoding problem, all characters with codes >= 128
 * are represented with octal escape sequences. */
struct LangTblItem {
	LangTblItem()
	{
	}
	LangTblItem(const std::string& code3_, const std::string& code2_, const std::string& name_):
		code3(code3_), code2(code2_), name(name_)
	{
	}
	// alpha-3 (bibliographic) code
	std::string code3;
	// alpha-2 code, an empty string when not given
	std::string code2;
	// English name
	std::string name;
};

/* An array of languages
 * Note. One language may have a number of English names,
 * each name is contained in a separate array element. */
extern std::vector<LangTblItem> lang_tbl;

void load_iso_639_2_langs();
void load_languages(const char* file_path);

#endif //!_LANG_TBL_HPP_
