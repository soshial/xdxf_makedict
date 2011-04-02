#ifndef _LANG_TBL_HPP_
#define _LANG_TBL_HPP_

#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif

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
	// alpha-3 (bibliographic) code
	const char* code3;
	// alpha-2 code, an empty string when not given
	const char* code2;
	// English name
	const char* name;
};

/* An array of languages terminated with a NULL record: { NULL, NULL, NULL }
 * Note. One language may have a number of English names,
 * each name is contained in a separate array element. */
extern LangTblItem lang_tbl[];

#endif //!_LANG_TBL_HPP_
