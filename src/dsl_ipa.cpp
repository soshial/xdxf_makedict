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
                                             gunichar to2,
                                             gunichar to3 = 0)
{
        gchar buf[7];

        UniToStrPair res;
        res.first = from;
        buf[g_unichar_to_utf8(to1, buf)] = '\0';
        res.second = buf;
        buf[g_unichar_to_utf8(to2, buf)] = '\0';
        res.second += buf;
        if(to3) {
            buf[g_unichar_to_utf8(to3, buf)] = '\0';
            res.second += buf;
        }

        return res;
}

//#define LINGVOX3

std::pair<UniToStrPair *, UniToStrPair *> ipa_to_unicode_tbl()
{
	/* This table contains all transcription characters used in
	 * all dictionaries distributed with ABBYY Lingvo 12.
	 *
	 * How this table was build
	 *
	 * Transcription shown in Lingvo Card window cannot be copied,
	 * but some of the dictionaries are available online through
	 * http://slovari.yandex.ru/ service. Dictionary articles in
	 * Lingvo 12 application and in online service does not always match,
	 * transcription may differ. Most of the time, however,
	 * we can find the required transcription character.
	 *
	 * As a last resort we may scan Unicode character table for similar character.
	 * See http://www.unicode.org/charts/, we are mainly interested in "Phonetic Symbols"
	 * character group. Latin and "Combining Diacritics" groups may be useful too.
	 *
	 * In the table below, characters marked with a question mark were found this way.
	 *
	 * In dictionaries distributed with ABBYY Lingvo x3 transcription characters
	 * do not require any conversion, they are true Unicode characters.
	 * So for Lingvo x3 this table should not be used, transcription characters
	 * should be left intact.
	 *
	 * Chars new to Lingvo x3 are included conditionally in this table.
	 * They all mapped to identical chars. They've been added here for debug purpose.
	 *
	 * Lingvo 12 and Lingvo x3 char sets collide in two points: 0x0405 and 0x2018.
	 * These chars should be translated differently depending on the version of
	 * Lingvo used. Other common chars are mapped to identical chars thus do not
	 * cause a problem.
	 *  */
	static UniToStrPair ipa_to_unicode_tbl[] = {
		ipa_to_unicode_make_pair(0x0020,  0x0020), //
		ipa_to_unicode_make_pair(0x0027,  0x0027), // '
		ipa_to_unicode_make_pair(0x0028,  0x0028), // (
		ipa_to_unicode_make_pair(0x0029,  0x0029), // )
		ipa_to_unicode_make_pair(0x002C,  0x002C), // ,
		ipa_to_unicode_make_pair(0x002D,  0x002D), // -
		ipa_to_unicode_make_pair(0x002E,  0x002E), // .
		ipa_to_unicode_make_pair(0x002F,  0x002F), // /
		ipa_to_unicode_make_pair(0x0030,  0x00DF), // ß ?
		ipa_to_unicode_make_pair(0x0031,  0x0065, 0x0303), // ẽ ?
		ipa_to_unicode_make_pair(0x0032,  0x025C), // ɜ
		ipa_to_unicode_make_pair(0x0033,  0x0129), // ĩ ?
		ipa_to_unicode_make_pair(0x0034,  0x00F5), // õ ?
		ipa_to_unicode_make_pair(0x0035,  0x0169), // ũ ?
		ipa_to_unicode_make_pair(0x0036,  0x028E), // ʎ ?
		ipa_to_unicode_make_pair(0x0037,  0x0263), // ɣ ?
		ipa_to_unicode_make_pair(0x0038,  0x1D4A), // e
		ipa_to_unicode_make_pair(0x003A,  0x003A), // ː
		ipa_to_unicode_make_pair(0x003B,  0x003B), // ;
#ifdef LINGVOX3
		ipa_to_unicode_make_pair(0x0041,  0x0041), // A
		ipa_to_unicode_make_pair(0x0047,  0x0047), // G
		ipa_to_unicode_make_pair(0x0048,  0x0048), // H
		ipa_to_unicode_make_pair(0x004D,  0x004D), // M
		ipa_to_unicode_make_pair(0x004E,  0x004E), // N
		ipa_to_unicode_make_pair(0x004F,  0x004F), // O
		ipa_to_unicode_make_pair(0x0052,  0x0052), // R
		ipa_to_unicode_make_pair(0x0053,  0x0053), // S
		ipa_to_unicode_make_pair(0x0054,  0x0054), // T
		ipa_to_unicode_make_pair(0x0061,  0x0061), // a
		ipa_to_unicode_make_pair(0x0062,  0x0062), // b
		ipa_to_unicode_make_pair(0x0063,  0x0063), // c
		ipa_to_unicode_make_pair(0x0064,  0x0064), // d
		ipa_to_unicode_make_pair(0x0065,  0x0065), // e
		ipa_to_unicode_make_pair(0x0066,  0x0066), // f
		ipa_to_unicode_make_pair(0x0067,  0x0067), // g
		ipa_to_unicode_make_pair(0x0068,  0x0068), // h
		ipa_to_unicode_make_pair(0x0069,  0x0069), // i
		ipa_to_unicode_make_pair(0x006A,  0x006A), // j
		ipa_to_unicode_make_pair(0x006B,  0x006B), // k
		ipa_to_unicode_make_pair(0x006C,  0x006C), // l
		ipa_to_unicode_make_pair(0x006D,  0x006D), // m
		ipa_to_unicode_make_pair(0x006E,  0x006E), // n
#endif
		ipa_to_unicode_make_pair(0x006F,  0x006F), // o
#ifdef LINGVOX3
		ipa_to_unicode_make_pair(0x0070,  0x0070), // p
		ipa_to_unicode_make_pair(0x0071,  0x0071), // q
		ipa_to_unicode_make_pair(0x0072,  0x0072), // r
		ipa_to_unicode_make_pair(0x0073,  0x0073), // s
		ipa_to_unicode_make_pair(0x0074,  0x0074), // t
		ipa_to_unicode_make_pair(0x0075,  0x0075), // u
		ipa_to_unicode_make_pair(0x0076,  0x0076), // v
		ipa_to_unicode_make_pair(0x0077,  0x0077), // w
		ipa_to_unicode_make_pair(0x007A,  0x007A), // z
#endif
		ipa_to_unicode_make_pair(0x0078,  0x0078), // x
		ipa_to_unicode_make_pair(0x0079,  0x0079), // y
		ipa_to_unicode_make_pair(0x00a0,  0x02A7), // ʧ
		ipa_to_unicode_make_pair(0x00a4,  0x0062), // b
		ipa_to_unicode_make_pair(0x00a6,  0x0077), // w
		ipa_to_unicode_make_pair(0x00a7,  0x0066), // f
		ipa_to_unicode_make_pair(0x00a9,  0x0073), // s
		ipa_to_unicode_make_pair(0x00ab,  0x0074), // t
		ipa_to_unicode_make_pair(0x00ac,  0x0064), // d
		ipa_to_unicode_make_pair(0x00ad,  0x006e), // n
		ipa_to_unicode_make_pair(0x00ae,  0x006c), // l
		ipa_to_unicode_make_pair(0x00b0,  0x006b), // k
		ipa_to_unicode_make_pair(0x00b1,  0x0067), // g
		ipa_to_unicode_make_pair(0x00b5,  0x0061), // a
		ipa_to_unicode_make_pair(0x00b6,  0x028A), // ʊ
		ipa_to_unicode_make_pair(0x00b7,  0x00E3), // ã
		ipa_to_unicode_make_pair(0x00bb,  0x0259), // ə
#ifdef LINGVOX3
		ipa_to_unicode_make_pair(0x00DF,  0x00DF), // ß
		ipa_to_unicode_make_pair(0x00E3,  0x00E3), // ã
		ipa_to_unicode_make_pair(0x00E4,  0x00E4), // ä
		ipa_to_unicode_make_pair(0x00E6,  0x00E6), // æ
		ipa_to_unicode_make_pair(0x00E7,  0x00E7), // ç
		ipa_to_unicode_make_pair(0x00EB,  0x00EB), // ë
		ipa_to_unicode_make_pair(0x00F0,  0x00F0), // ð
		ipa_to_unicode_make_pair(0x00F4,  0x00F4), // ô
		ipa_to_unicode_make_pair(0x00F8,  0x00F8), // ø
		ipa_to_unicode_make_pair(0x0101,  0x0101), // ā
		ipa_to_unicode_make_pair(0x0113,  0x0113), // ē
		ipa_to_unicode_make_pair(0x012B,  0x012B), // ī
		ipa_to_unicode_make_pair(0x014B,  0x014B), // ŋ
		ipa_to_unicode_make_pair(0x014D,  0x014D), // ō
		ipa_to_unicode_make_pair(0x014F,  0x014F), // ŏ
		ipa_to_unicode_make_pair(0x0153,  0x0153), // œ
		ipa_to_unicode_make_pair(0x01D0,  0x01D0), // ǐ
		ipa_to_unicode_make_pair(0x01D4,  0x01D4), // ǔ
		ipa_to_unicode_make_pair(0x0250,  0x0250), // ɐ
		ipa_to_unicode_make_pair(0x0251,  0x0251), // ɑ
		ipa_to_unicode_make_pair(0x0252,  0x0252), // ɒ
		ipa_to_unicode_make_pair(0x0254,  0x0254), // ɔ
		ipa_to_unicode_make_pair(0x0259,  0x0259), // ə
		ipa_to_unicode_make_pair(0x025B,  0x025B), // ɛ
		ipa_to_unicode_make_pair(0x025C,  0x025C), // ɜ
		ipa_to_unicode_make_pair(0x0265,  0x0265), // ɥ
		ipa_to_unicode_make_pair(0x0272,  0x0272), // ɲ
		ipa_to_unicode_make_pair(0x0280,  0x0280), // ʀ
		ipa_to_unicode_make_pair(0x0283,  0x0283), // ʃ
		ipa_to_unicode_make_pair(0x028A,  0x028A), // ʊ
		ipa_to_unicode_make_pair(0x028C,  0x028C), // ʌ
		ipa_to_unicode_make_pair(0x028F,  0x028F), // ʏ
		ipa_to_unicode_make_pair(0x0292,  0x0292), // ʒ
		ipa_to_unicode_make_pair(0x02A4,  0x02A4), // ʤ
		ipa_to_unicode_make_pair(0x02A7,  0x02A7), // ʧ
		ipa_to_unicode_make_pair(0x026A,  0x026A), // ɪ
		ipa_to_unicode_make_pair(0x02CC,  0x02CC), // ˌ
		ipa_to_unicode_make_pair(0x02D0,  0x02D0), // ː
		ipa_to_unicode_make_pair(0x0303,  0x0303), // combining tilde
		ipa_to_unicode_make_pair(0x0331,  0x0331), // combining macron below
		ipa_to_unicode_make_pair(0x035C,  0x035C), // combining double breve below
		ipa_to_unicode_make_pair(0x035D,  0x035D), // combining double breve
		ipa_to_unicode_make_pair(0x035E,  0x035E), // combining double macron
		ipa_to_unicode_make_pair(0x035F,  0x035F), // combining double macron below
		ipa_to_unicode_make_pair(0x03B8,  0x03B8), // θ
#endif
		ipa_to_unicode_make_pair(0x0402,  0x0069, 0x02D0), // iː
		ipa_to_unicode_make_pair(0x0403,  0x0251, 0x02D0 ),// ɑː
		ipa_to_unicode_make_pair(0x0404,  0x007a), // z
		ipa_to_unicode_make_pair(0x0405,  0x0153, 0x0303), // œ̃
		ipa_to_unicode_make_pair(0x0406,  0x0068), // h
		ipa_to_unicode_make_pair(0x0407,  0x0072), // r
		ipa_to_unicode_make_pair(0x0408,  0x0070), // p
		ipa_to_unicode_make_pair(0x0409,  0x0292), // ʒ
		ipa_to_unicode_make_pair(0x040a,  0x014b), // ŋ
		ipa_to_unicode_make_pair(0x040b,  0x03b8), // θ
		ipa_to_unicode_make_pair(0x040c,  0x0075), // u
		ipa_to_unicode_make_pair(0x040e,  0x026a), // ɪ
		ipa_to_unicode_make_pair(0x040f,  0x0283), // ʃ
		ipa_to_unicode_make_pair(0x0428,  0x0061), // a
		ipa_to_unicode_make_pair(0x0440,  0x014F), // ŏ
		ipa_to_unicode_make_pair(0x0441,  0x0272), // ɲ
		ipa_to_unicode_make_pair(0x0442,  0x0254, 0x0303), // ɔ̃
		ipa_to_unicode_make_pair(0x0443,  0x00F8), // ø
		ipa_to_unicode_make_pair(0x0444,  0x01D4), // ǔ
		ipa_to_unicode_make_pair(0x0445,  0x025B, 0x0303), //ɛ̃
		ipa_to_unicode_make_pair(0x0446,  0x00E7), // ç
		ipa_to_unicode_make_pair(0x0447,  0x0061, 0x035C, 0x0065), // a͜e
		ipa_to_unicode_make_pair(0x044A,  0x028F), // ʏ
		ipa_to_unicode_make_pair(0x044C,  0x0251, 0x0303), // ɑ̃
		ipa_to_unicode_make_pair(0x044D,  0x026A), // ɪ
		ipa_to_unicode_make_pair(0x044E,  0x025C, 0x02D0), // ɜː
		ipa_to_unicode_make_pair(0x044F,  0x0252), // ɒ
		ipa_to_unicode_make_pair(0x0452,  0x0076), // v
		ipa_to_unicode_make_pair(0x0453,  0x0075, 0x02D0), // uː
		ipa_to_unicode_make_pair(0x0454,  0x0254, 0x035C, 0x00F8), // ɔ͜ø
		ipa_to_unicode_make_pair(0x0455,  0x01D0), // ǐ
		ipa_to_unicode_make_pair(0x0456,  0x006a), // j
		ipa_to_unicode_make_pair(0x0457,  0x0265), // ɥ
		ipa_to_unicode_make_pair(0x0458,  0x0153), // œ
		ipa_to_unicode_make_pair(0x045e,  0x0065), // e
		ipa_to_unicode_make_pair(0x0490,  0x006d), // m
		ipa_to_unicode_make_pair(0x0491,  0x025b), // ɛ
		ipa_to_unicode_make_pair(0x2018,  0x0251), // ɑ
		ipa_to_unicode_make_pair(0x201a,  0x0254, 0x02D0), // ɔː
		ipa_to_unicode_make_pair(0x201e,  0x0259, 0x02D0), // əː ?
		ipa_to_unicode_make_pair(0x2020,  0x0259), // ə
		ipa_to_unicode_make_pair(0x2021,  0x00E6), // æ
		ipa_to_unicode_make_pair(0x2026,  0x028C), // ʌ
		ipa_to_unicode_make_pair(0x2030,  0x00F0), // ð
		ipa_to_unicode_make_pair(0x2039,  0x02A4), // ʤ
		ipa_to_unicode_make_pair(0x20ac,  0x0254), // ɔ
		ipa_to_unicode_make_pair(0x2116,  0x0061, 0x035C, 0x006F), // a͜o
	};

        return std::make_pair(ipa_to_unicode_tbl, ipa_to_unicode_tbl +
                              sizeof(ipa_to_unicode_tbl) / sizeof(UniToStrPair));
}
