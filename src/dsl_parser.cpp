/*
 * This file is part of makedict - convertor from any
 * dictionary format to any http://xdxf.sourceforge.net
 *
 * Copyright (C) Evgeniy Dushistov, 2005-2006
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Library General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

// $Id$
// module for work with dsl format(http://lingvo.ru)

#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif

#include <cctype>
#include <cerrno>
#include <algorithm>
#include <iterator>
#include <list>
#include <glib/gi18n.h>

#include "charset_conv.hpp"
#include "dsl_ipa.hpp"
#include "mapfile.hpp"
#include "normalize_tags.hpp"
#include "utils.hpp"
#include "file.hpp"
#include "xml.hpp"
#include "resource.hpp"

#include "parser.hpp"

//#define DEBUG
namespace {

class DslParser : public ParserBase {
public:
	DslParser();
	~DslParser() {}
protected:
	bool is_my_format(const std::string& url) {
		//To fix compiler warning bool<-gboolean conversation
		if (g_str_has_suffix(url.c_str(), ".dsl"))
			return true;
		else
			return false;
	}
	int parse(const std::string& filename);
private:
	typedef enum {
		NAME=0, INDEX_LANGUAGE=1,
		CONTENTS_LANGUAGE=2, SOURCE_CODE_PAGE=3,
		UNKNOWN=4
	} enHeaderTags;


	static TagInfoList taginfo_list;
	std::string basename;

	bool utf16;//mean that source file in utf16
	bool little_endian;//mean that order of bytes in little_endian

	guint32 linenum;//number of current line
	std::string line;
	std::string name, index_language, contents_language;
	std::string from_codeset;

	static Str2StrTable code_page_table;
	static Str2StrTable short_lang_table;
	static Str2StrTable replace_table;

	std::map<gunichar, std::string> ipa_to_unicode_;
	char *end_of_file;
	bool not_close_comment;

	bool getline(MapFile& in);
	enHeaderTags is_line_has_tag(void);
	bool get_tag_value(const char* tag_name, std::string& value);
	static bool long_to_short(std::string& longlang, std::string& shortlang);

	//this is a real function which parse file,
	//because of in additition to dsl by itself,
	//may be need parse file with abbrevations,
	//which have the same format
	int parse(MapFile& in, bool only_info, bool abr);
	int print_info();
	bool parse_header(MapFile& in, CharsetConv& conv);
	bool read_keys(MapFile& in, const CharsetConv& conv, StringList& key_list);
	void utf16_to_machine(guint16 &ch);
	void parse_abbrs(const std::string&, const std::string&);
	void trans_ipa_to_utf(const char *p, const char *end, std::string& resstr);
	bool encode_article(CharsetConv& conv, std::string& datastr);
	void article2xdxf(StringList&, std::string&);
};

REGISTER_PARSER(DslParser,dsl);


Str2StrTable DslParser::code_page_table;
Str2StrTable DslParser::replace_table;
Str2StrTable DslParser::short_lang_table;
TagInfoList DslParser::taginfo_list;

DslParser::DslParser() : 
	ipa_to_unicode_(ipa_to_unicode_tbl().first, ipa_to_unicode_tbl().second)
{
	not_close_comment=false;

	set_parser_info("format", "dsl");
	set_parser_info("version", "dsl_parser, version 0.1");

	parser_options_["full_name"]="";
	parser_options_["encoding"]="";
	parser_options_["lang_from"]="";
	parser_options_["lang_to"]="";

	if (!code_page_table.empty())
		return;
	code_page_table["Latin"]="CP1252";
	code_page_table["Cyrillic"]="CP1251";
	code_page_table["EasternEuropean"]="CP1250";

	short_lang_table["afrikaans"]="AFR";
	short_lang_table["basque"]="BAQ";
	short_lang_table["belarusian"]="BEL";
	short_lang_table["bulgarian"]="BUL";
	short_lang_table["czech"]="CZE";
	short_lang_table["danish"]="DAN";
	short_lang_table["dutch"]="DUT";
	short_lang_table["english"]="ENG";
	short_lang_table["finnish"]="FIN";
	short_lang_table["french"]="FRA";
	short_lang_table["german"]="GER";
	short_lang_table["germannewspelling"]="GER";
	short_lang_table["hungarian"]="HUN";
	short_lang_table["indonesian"]="IND";
	short_lang_table["italian"]="ITA";
	short_lang_table["norwegianbokmal"]="NOB";
	short_lang_table["norwegiannynorsk"]="NNO";
	short_lang_table["polish"]="POL";
	short_lang_table["portuguesestandard"]="POR";
	short_lang_table["russian"]="RUS";
	short_lang_table["serbiancyrillic"]="SCC";
	short_lang_table["spanishmodernsort"]="SPA";
	short_lang_table["spanishtraditionalsort"]="SPA";
	short_lang_table["swahili"]="SWA";
	short_lang_table["swedish"]="SWE";
	short_lang_table["ukrainian"]="UKR";
	short_lang_table["chinese"] = "CHI";


	replace_table["[/m]"]="";
#if 0
	replace_table["[ref]"]="<kref>";
	replace_table["[/ref]"]="</kref>";
#endif
	replace_table["[url]"]="<iref>";
	replace_table["[/url]"]="</iref>";
	replace_table["[!trs]"]="";
	replace_table["[/!trs]"]="";
	replace_table["[/lang]"]="";
	replace_table["[*]"]="";
	replace_table["[/*]"]="";
	replace_table["{{"]="<!--";
	replace_table["}}"]="-->";
	replace_table["<<"]="<kref>";
	replace_table[">>"]="</kref>";
	replace_table["[s]"]="<rref>";
	replace_table["[/s]"]="</rref>";
	replace_table["[m]"]="";//handle errors in dsl
	replace_table["[m1]"]=" ";
	replace_table["[m2]"]="  ";
	replace_table["[m3]"]="   ";
	replace_table["[m4]"]="    ";
	replace_table["[m5]"]="     ";
	replace_table["[m6]"]="      ";
	replace_table["[m7]"]="       ";
	replace_table["[m8]"]="        ";
	replace_table["[m9]"]="         ";
	replace_table["[']"] = "<nu />'<nu />";
	replace_table["[/']"] = "";
	
	taginfo_list.push_back(TagInfo("[b]", "[/b]", "<b>", "</b>",
				       TagInfo::tB));
	taginfo_list.push_back(TagInfo("[i]", "[/i]", "<i>", "</i>",
				       TagInfo::tI));
	taginfo_list.push_back(TagInfo("[t]", "[/t]", "<tr>", "</tr>",
				       TagInfo::tTranscription));
	taginfo_list.push_back(TagInfo("[com]", "[/com]", "<co>", "</co>",
				       TagInfo::tComment));
	taginfo_list.push_back(TagInfo("[c", "[/c]", "<c>", "</c>",
				       TagInfo::tColor, true));
	taginfo_list.push_back(TagInfo("[p]", "[/p]", "<abr>", "</abr>",
				       TagInfo::tAbr));
	taginfo_list.push_back(TagInfo("[sub]", "[/sub]", "<sub>", "</sub>",
				       TagInfo::tSub));
	taginfo_list.push_back(TagInfo("[sup]", "[/sup]", "<sup>", "</sup>",
				       TagInfo::tSup));
	taginfo_list.push_back(TagInfo("[trn]", "[/trn]", "<dtrn>", "</dtrn>",
				       TagInfo::tDtrn));
	taginfo_list.push_back(TagInfo("[ex]", "[/ex]", "<ex>", "</ex>",
				       TagInfo::tExample));
	taginfo_list.push_back(TagInfo("[u]", "[/u]", "<c>", "</c>",
				       TagInfo::tColor));
	taginfo_list.push_back(TagInfo("[ref", "[/ref]", "<kref>", "</kref>",
				       TagInfo::tKref, true));
}

void DslParser::trans_ipa_to_utf(const char *p, const char *end, std::string& resstr)
{
	std::map<gunichar, std::string>::const_iterator it;	
	gunichar ch;
	char buf[7];

	while (*p && p < end) {
		ch = g_utf8_get_char(p);
#ifdef DEBUG_IPA
		g_debug("%s: before %llX\n", __FUNCTION__, (unsigned long long)ch);
#endif
		it = ipa_to_unicode_.find(ch);

		if (it != ipa_to_unicode_.end()) {
			resstr += it->second;
#ifdef DEBUG_IPA
			g_debug("%s: converted: %s\n", __FUNCTION__, it->second.c_str());
#endif
		} else {
#ifdef DEBUG_IPA
			g_debug("%s: not changed %llX\n", __FUNCTION__);
#endif
			buf[g_unichar_to_utf8(ch, buf)] = '\0';
			resstr += buf;
		}

		p = g_utf8_next_char(p);
	}

}

//convert from Long language name, like English
//to short, like eng
inline bool DslParser::long_to_short(std::string& longlang,
				      std::string& shortlang)
{
	tolower(longlang);
	Str2StrTable::iterator lang=short_lang_table.find(longlang.c_str());
	if (lang==short_lang_table.end()) {
		StdErr.printf(_("Unknwon language %s\nPossible languages:\n"), longlang.c_str());

		for (lang=short_lang_table.begin(); lang!=short_lang_table.end(); ++lang)
			StdErr << lang->first << "\t";
		StdErr<<"\n";
		return false;
	}
	shortlang=lang->second;

	return true;
}

void DslParser::parse_abbrs(const std::string& dirname,
				   const std::string& basename)
{
	MapFile map_file;
	std::list<std::string> variants;

	variants.push_back(dirname+G_DIR_SEPARATOR+"abbrev.dsl");
	variants.push_back(basename+"_abrv.dsl");
	variants.push_back(basename+"_abbrev.dsl");

	for (std::list<std::string>::const_iterator it = variants.begin();
	     it != variants.end(); ++it) {
		map_file.open(it->c_str());

		if (map_file) {
			StdErr.printf(_("Parse file with abbrevations: %s\n"), it->c_str());
			name=index_language=contents_language=from_codeset="";
			if (parse(map_file, false, true) != EXIT_SUCCESS) {
				StdErr << _("Error during parse abbrevation this file: ")
				     << *it << "\n";
				break;
			}
			StdErr<<_("done")<<"\n";
			break;
		}
	}
}

int DslParser::parse(const std::string& filename)
{
	int res=EXIT_FAILURE;

	basename.assign(filename);
	std::string::size_type pos=basename.rfind('.');
	if (pos!=std::string::size_type(-1))
		basename.erase(pos, basename.length()-pos);


	//search icon
	std::string icon_name=basename+".bmp";
	if (is_file_exist(icon_name)) {
		set_dict_info("icon", icon_name);
		g_message(_("Icon is found: %s\n"), icon_name.c_str());
	}

	std::string dirname(filename);
	pos=dirname.rfind(G_DIR_SEPARATOR);
	if (pos != std::string::npos)
		dirname.erase(pos, dirname.length()-pos);
	else
		dirname=".";

	{
		MapFile in;
		if (!in.open(filename.c_str())) {
			StdErr.printf(_("Can not open %s\n"), filename.c_str());
			return res;
		}
		if ((res=parse(in, true, false))!=EXIT_SUCCESS)
			return res;
		name=index_language=contents_language=from_codeset="";
	}


	parse_abbrs(dirname, basename);

	MapFile in;
	if (!in.open(filename.c_str())) {
		StdErr.printf(_("Can not open %s\n"), filename.c_str());
		return res;
	}

	name=index_language=contents_language=from_codeset="";

	if ((res=parse(in, false, false))!=EXIT_SUCCESS)
		return res;

	return EXIT_SUCCESS;
}

int DslParser::print_info()
{
	int res=EXIT_FAILURE;
	if (name.empty() && parser_options_["full_name"].empty()) {
		StdErr<<_("NAME not defined\n");
		return res;
	} else if (!parser_options_["full_name"].empty()) {
		name=parser_options_["full_name"];
	}

	set_dict_info("full_name", name);
	//set_dict_info("description", std::string("Copyright: ")+copyright+"; Version: "+version);

	std::string lang;

	if (index_language.empty() && parser_options_["lang_from"].empty()) {
		StdErr<<_("INDEX_LANGUAGE not defined\n");
		return res;
	} else if (!parser_options_["lang_from"].empty()) {
		lang=parser_options_["lang_from"];
	} else//parse check that all ok
		long_to_short(index_language, lang);

	set_dict_info("lang_from", lang);
	lang.clear();
	if (contents_language.empty() && parser_options_["lang_to"].empty()) {
		StdErr<<_("CONTENTS_LANGUAGE not defined\n");
		return res;
	} else if (!parser_options_["lang_to"].empty()) {
		lang=parser_options_["lang_to"];
	} else//parse check that all ok
		long_to_short(contents_language, lang);

	set_dict_info("lang_to", lang);

	//read annotation
	std::string anot_name = basename + ".ann";
	if (is_file_exist(anot_name)) {
		StdErr.printf(_("Reading: %s\n"), anot_name.c_str());
		glib::CharStr content;
		glib::Error err;
		gsize anot_len;
		if (!g_file_get_contents(anot_name.c_str(), get_addr(content),
					 &anot_len, get_addr(err))) {
			StdErr.printf(_("Can not read %s: %s\n"), anot_name.c_str(),
				      err->message);
			goto out;
		}

		StringList from_codesets;
		if (!from_codeset.empty())
			from_codesets.push_back(from_codeset);
		from_codesets.push_back("UTF-16");
		from_codesets.push_back("UCS-2");
		std::string convstr;
		StringList::iterator it;
		for (it = from_codesets.begin(); it != from_codesets.end(); ++it) {
			CharsetConv conv(it->c_str(), "UTF-8");
			convstr.clear();

			if (!conv.convert(get_impl(content), anot_len, convstr))
				convstr = get_impl(content);

			if (g_utf8_validate(convstr.c_str(), -1, NULL))
				break;
		}

		if (it == from_codesets.end()) {
			StdErr.printf(_("Annotation of dictionary is not in %s\n"
					"Recode it to %s, and try again\n"),
				      from_codesets[0].c_str(),
				      from_codesets[0].c_str());
			goto out;
		}

		Str2StrTable end_of_line;
		end_of_line["\r\n"]="\n";
		end_of_line["\n\r"]="\n";
		std::string new_convstr;
		replace(end_of_line, convstr.c_str(), new_convstr);

		xml::encode(new_convstr, convstr);

		set_dict_info("description", convstr);
	}
out:	
	return begin() ? EXIT_SUCCESS : EXIT_FAILURE;
}

inline void DslParser::utf16_to_machine(guint16 &ch)
{
	if (little_endian)
		ch = GINT16_FROM_LE(ch);
	else
		ch = GINT16_FROM_BE(ch);
}

//read one line from file
//and convert it to utf8, if file in utf16
bool DslParser::getline(MapFile& in)
{
reread_line:
	++linenum;
	line.clear();

	if (!utf16) {
		char ch;

		while (in.cur < end_of_file) {
			ch = *in.cur;
			++in.cur;
			if (ch == '\r' || ch == '\n')
				break;
			line+=ch;
		}
		if (in.cur < end_of_file) {
			ch=*in.cur;
			if (ch == '\r' || ch=='\n')
				++in.cur;
		}
	} else {
		static CharsetConv conv("UTF-16", "UTF-8");
		guint16 ch;

		while (in.cur+1<end_of_file) {
			ch=*reinterpret_cast<gint16 *>(in.cur);
			in.cur+=sizeof(ch);
			utf16_to_machine(ch);
			if (ch=='\r' || ch=='\n')
				break;
			line+=*reinterpret_cast<char *>(&ch);
			line+=*(reinterpret_cast<char *>(&ch)+1);
		}
		if (in.cur+1<end_of_file) {
			ch=*reinterpret_cast<gint16 *>(in.cur);
			utf16_to_machine(ch);
			if (ch=='\r' || ch=='\n')
				in.cur+=sizeof(ch);
		} else
			in.cur++;//so in::eof return true

		std::string convstr;
		if (conv.convert(line, convstr))
			line=convstr;
	}

	if (not_close_comment) {
		std::string::size_type com_end=line.find("}}");
		if (com_end == std::string::npos) {
			if (!(in.cur<end_of_file)) {
				line.clear();
				return false;
			}
			goto reread_line;
		}
		line.erase(0, com_end+2);
		not_close_comment=false;
	}

	std::string::size_type pos;
	while ((pos = line.find("{{")) != std::string::npos) {
		std::string::size_type com_end = line.find("}}", pos+2);
		if (com_end == std::string::npos) {
			if (line.find("}", pos+2) != std::string::npos)
				g_warning(_("%s, line %d: comment is opened '{{', "
					    "and there is '}' on the same line, "
					    "may be you mean '}}'?:\n%s\n"),
					  in.filename().c_str(), linenum, line.c_str());
			
			not_close_comment = true;
			if (!(in.cur<end_of_file)) {
				line.clear();
				return false;
			}
			if (pos == 0)
				goto reread_line;
		}
		line.erase(pos, com_end-pos+2);
	}

	return in.cur < end_of_file;
}

DslParser::enHeaderTags DslParser::is_line_has_tag(void)
{
	static const char* possible_tags[]={
		"#NAME", "#INDEX_LANGUAGE", "#CONTENTS_LANGUAGE",
		"#SOURCE_CODE_PAGE", NULL
	};
	int i;
	for (i=0; i<UNKNOWN; ++i)
		if (line.find(possible_tags[i])!=std::string::size_type(-1))
			return enHeaderTags(i);
	return enHeaderTags(i);
}

bool DslParser::get_tag_value(const char* tag_name, std::string& value)
{
	value.resize(0);

	std::string::size_type pos, end;

	if ((pos=line.find(tag_name))==std::string::size_type(-1))
		return false;

	pos+=strlen(tag_name);
	while (pos<=line.length()-1 && (line[pos]=='\t' || line[pos]==' '))
		++pos;
	if (line[pos]!='"') {
		StdErr.printf(_("%d: value of tag must be in \"\n"), linenum);
		return false;
	}
	end=++pos;
	while (end<=line.length()-1 && line[end]!='"')
		++end;
	if (end>=line.length()) {
		StdErr.printf(_("%d: there are no twin \"\n"), linenum);
		return false;
	}
	value=line.substr(pos, end-pos);
	return true;
}

bool DslParser::parse_header(MapFile& in, CharsetConv& conv)
{
	//read all tags
	while (in) {
		while (getline(in) && line.empty())
			;
		if (!in) {
			StdErr << _("There are no not empty stirngs\n");
			return false;
		}
		enHeaderTags tag=is_line_has_tag();
		if (UNKNOWN==tag)
			break;
		switch (tag) {
		case NAME:
			if (!get_tag_value("#NAME", name))
				return false;
			break;

		case INDEX_LANGUAGE:
		{
			if (!get_tag_value("#INDEX_LANGUAGE", index_language))
				return false;
			std::string shortlang;
			if (!long_to_short(index_language, shortlang))
				return false;
		}
		break;

		case CONTENTS_LANGUAGE:
		{
			if (!get_tag_value("#CONTENTS_LANGUAGE", contents_language))
				return false;
			std::string shortlang;
			if (!long_to_short(contents_language, shortlang))
				return false;
		}
		break;

		case SOURCE_CODE_PAGE:
		{
			std::string source_code_page;
			if (!get_tag_value("#SOURCE_CODE_PAGE", source_code_page))
				return false;
			Str2StrTable::iterator codeset=code_page_table.find(source_code_page.c_str());
			if (codeset==code_page_table.end()) {
				StdErr.printf(_("This value is not valid #SOURCE_CODE_PAGE value: %s\n"
						"Possible values:\n"),
					      source_code_page.c_str());
				for (Str2StrTable::iterator cp = code_page_table.begin();
				     cp!=code_page_table.end(); ++cp)
					StdErr<<cp->first<<"\n";
				return false;
			}

			from_codeset=codeset->second;
			conv.workwith(from_codeset.c_str(), "UTF-8");
			utf16=false;
		}
		break;

		default:
			/*this not should happen*/;
		}//switch (tag) {
	}
	return true;
}

bool DslParser::read_keys(MapFile& in, const CharsetConv& conv,
			   StringList& key_list)
{	
	std::string *cur_line;

	do {
		std::string utf8str;

		if (!from_codeset.empty()) {
			if (!conv.convert(line, utf8str)) {
				StdErr.printf(_("Can not convert from %s to UTF-8"),
					      from_codeset.c_str());
				return false;
			}
			cur_line = &utf8str;
		} else
			cur_line = &line;

		if (!g_utf8_validate(cur_line->c_str(), gssize(-1), NULL)) {
			StdErr.printf(_("Not valid UTF-8 string: %d\n"
					"Convert it to UTF-8, and try again\n"), linenum);
			return false;
		}

		std::string key_enc;
		const char *p = cur_line->c_str();

		while (*p) {
			if (*p=='\\') {//skip first '\'
				if (!(*++p))
					break;
				goto add_char;
			} else if (*p == '(') {
				key_enc += "<opt>";
			} else if (*p == ')') {
				key_enc += "</opt>";
			} else if (*p == '{') {
				key_enc += "<nu />";
			} else if (*p == '}') {
				key_enc += "<nu />";
			} else if (strncmp(p, "[sub]", sizeof("[sub]") - 1) == 0) {
				key_enc += "<sub>";
				p += sizeof("[sub]") - 2;
			} else if (strncmp(p, "[/sub]", sizeof("[/sub]") - 1) == 0) {
				key_enc += "</sub>";
				p += sizeof("[/sub]") - 2;
			} else {
			add_char:
				xml::add_and_encode(key_enc, *p);
			}
			++p;
		}

		//remove last blank characters
		std::string::reverse_iterator ri;
		for (ri=key_enc.rbegin(); ri!=key_enc.rend(); ++ri)
			if (*ri!=' ' && *ri!='\t')
				break;
		key_enc.erase(ri.base(), key_enc.end());
		if (!key_enc.empty())
			key_list.push_back(key_enc);
	} while (getline(in) && line[0]!='\t' && line[0]!=' ');

	return true;
}

bool DslParser::encode_article(CharsetConv& conv, std::string& datastr)
{
	std::string encoded_str;
	size_t tr_pos, tr_prev_pos = 0;

	if (!from_codeset.empty()) {
		if (!conv.convert(datastr, encoded_str)) {
			StdErr.printf(_("Can not convert from %s to UTF-8\n"),
				      from_codeset.c_str());
			return false;
		}
		datastr = encoded_str;
		encoded_str.clear();
	}

	while ((tr_pos = datastr.find("\\[[t]", tr_prev_pos)) != std::string::npos ||
	       (tr_pos = datastr.find("[t]", tr_prev_pos)) != std::string::npos) {

		encoded_str += datastr.substr(tr_prev_pos, tr_pos - tr_prev_pos);
		tr_prev_pos = tr_pos;

		if ((tr_pos = datastr.find("[/t]", tr_prev_pos)) == std::string::npos) {
			StdErr.printf(_("[t] doesn't close, article: %s\n"),
				      datastr.c_str());
			return false;
		}

		encoded_str += "[t]";
		size_t offset = datastr[tr_prev_pos] == '\\' ? sizeof("\\[[t]") - 1 :
			sizeof("[t]") - 1;
		trans_ipa_to_utf(datastr.c_str() + tr_prev_pos + offset,
				 datastr.c_str() + tr_pos, encoded_str);

		encoded_str += "[/t]";
		if (tr_pos + sizeof("[/t]\\]") - 2 < datastr.length() &&
		    datastr.substr(tr_pos, sizeof("[/t]\\]") - 1) == "[/t]\\]")
			offset = sizeof("[/t]\\]") -1;
		else
			offset = sizeof("[/t]") - 1;
		tr_prev_pos = tr_pos + offset;
	}

	encoded_str += datastr.substr(tr_prev_pos, datastr.length() - tr_prev_pos);
	datastr = encoded_str;

	return true;
}

void DslParser::article2xdxf(StringList& key_list, std::string& datastr)
{	
	std::string resstr;
	NormalizeTags norm_tags(taginfo_list);
	const char *p=datastr.c_str();
	bool have_subarticle=false;
	while (*p)
		if (*p=='\\') {
			if (!(*++p))
				break;
			if (*p!='\n') {
				xml::add_and_encode(resstr, *p);
				++p;
			}
		} else if (*p=='~') {
			resstr += key_list.back();
			++p;
		} else if (*p=='[') {//may be this is a tag ?

			if (*(p+1)!='/') {//not close tag
				if (strncmp(p, "[lang", sizeof("[lang")-1)==0) {
					//just skip it
					const char *closed_braket = strchr(p, ']');
					if (closed_braket != NULL)
						p = closed_braket + 1;
					else
						StdErr << _("Tag [lang didn't closed\n");
				} else {
					if (!norm_tags.add_open_tag(resstr, p))
						goto end_of_handle;
				}

			} else {//if (*(p+1)!='/') {
				if (!norm_tags.add_close_tag(resstr, p))
					goto end_of_handle;
			}
		} else if (*p=='@') { //handle subarticles
			if (!have_subarticle) {
				++p;
				while (*p && (*p=='\t' || *p==' '))
					++p;
			subarticle_key:
				if (!*p)
					break;
				const char *end_of_line=strchr(p, '\n');
				if (end_of_line==NULL)
					end_of_line=p+strlen(p)-1;
				const char *not_blank=end_of_line;
				while (not_blank>p && (*not_blank=='\t' || *not_blank==' '))
					--not_blank;
				std::string key(p, not_blank-p);
				resstr+=std::string("<su>")+"<k>"+key+"</k>\n";
				have_subarticle=true;
				p=end_of_line+1;
				continue;
			} else {
				if (resstr[resstr.length()-1]=='\n')
					resstr.erase(resstr.begin()+resstr.length()-1);

				resstr+="</su>";
				have_subarticle=false;
				++p;
				while (*p && (*p=='\t' || *p==' '))
					++p;
				if (*p=='\n') {
					++p;
					continue;
				}
				goto subarticle_key;
			}
		} else if (strncmp(p, "^~", 2)==0) {
			gunichar ch=g_utf8_get_char(key_list.back().c_str());
			if (g_unichar_islower(ch))
				ch=g_unichar_toupper(ch);
			else if (g_unichar_isupper(ch))
				ch=g_unichar_tolower(ch);
			char buf[7];
			gint size=g_unichar_to_utf8(ch, buf);
			buf[size]='\0';
			resstr+=std::string(buf)+g_utf8_next_char(key_list.back().c_str());
			p+=2;
		} else {

		end_of_handle:
			const char *beg = p;
			Str2StrTable::const_iterator i;
			for (i=replace_table.begin(); i!=replace_table.end(); ++i) {

				p=beg;
				const char *q=i->first;
				while (*p && *q && *p==*q)
					++p, ++q;

				if (*q=='\0') {
					resstr+=i->second;
					break;
				}
			}

			if (i == replace_table.end()) {
				p = beg;
				xml::add_and_encode(resstr, *p);
				++p;
			}
		}

	if (resstr[resstr.length()-1] == '\n')
		resstr.erase(resstr.begin()+resstr.length()-1);

	norm_tags(resstr, datastr);
}

int DslParser::parse(MapFile& in, bool only_info, bool abr)
{
	linenum = 0;

	end_of_file=in.end();

	int res = EXIT_FAILURE;
	CharsetConv conv;

	//try to determine encoding
	//TODO: add utf-32 support

	int ch1;
	if (in.cur<end_of_file)
		ch1=(unsigned char)*in.cur;
	else
		return res;
	int ch2;
	if (in.cur+1<end_of_file)
		ch2=(unsigned char)*(in.cur+1);
	else
		return res;

	utf16=false;

	if (ch1==0xFF && ch2==0xFE) {
		utf16=true;
		little_endian=true;
		g_debug("int DslParser::parse(MapFile& in, bool only_info, bool abr): UTF-16 encoding\n");
	} else if (ch1==0xFE && ch2==0xFF) {
		utf16=true;
		little_endian=false;
	}
	if (!parse_header(in, conv))
		return res;
	//we get all tags from dsl, we have last line, which not tag in
	//"line" variable

	if (from_codeset.empty() && !parser_options_["encoding"].empty()) {
		from_codeset=parser_options_["encoding"];
		conv.workwith(from_codeset.c_str(), "UTF-8");
		utf16=false;
	}

	if (only_info && !abr)
		return print_info();

	if (abr)
		if (!abbrs_begin())
			return EXIT_FAILURE;
	
	do {
		StringList key_list;

		if (!read_keys(in, conv, key_list))
			return res;

		if (!in && line[0]!='\t' && line[0]!=' ')
			break;

		std::string datastr;

		do {
			const char *real_begin=line.c_str()+1;
			while (*real_begin && (*real_begin=='\t' || *real_begin==' '))
				++real_begin;
			if (*real_begin)
				datastr += std::string(real_begin) + "\n";
		} while (getline(in) && (line[0]=='\t' || line[0]==' '));


		if (in.eof() && !line.empty() && (line[0]=='\t' || line[0]==' ')) {
			const char *real_begin=line.c_str()+1;
			while (*real_begin && (*real_begin=='\t' || *real_begin==' '))
				++real_begin;
			if (*real_begin)
				datastr += std::string(real_begin)+"\n";
		}

		if (!encode_article(conv, datastr))
			return res;

		if (!g_utf8_validate(datastr.c_str(), gssize(-1), NULL)) {
			StdErr.printf(_("Not valid UTF-8 string: %s\n"
					"Convert it to UTF-8, and try again\n"),
				      datastr.c_str());
			return res;
		}

		article2xdxf(key_list, datastr);

		if (!g_utf8_validate(datastr.c_str(), gssize(-1), NULL)) {
			StdErr.printf(_("Not valid UTF-8 string: %s\n"
				       "Article header: %s\n"
				       "Convert it to UTF-8, and try again\n"),
				     datastr.c_str(), (*key_list.rbegin()).c_str());
			return res;
		}

		if (abr) {
			if (!abbr(key_list, datastr))
				return EXIT_FAILURE;
		} else
			if (!article(key_list, datastr, false))
				return EXIT_FAILURE;
		key_list.clear();

		while (line == "" && getline(in))
			;
	} while (in);


	if (abr)
		if (!abbrs_end())
			return EXIT_FAILURE;

	return EXIT_SUCCESS;
}
}//end of anonymous namespace

#if 0
int main(int argc, char *argv[])
{
	return DslParser().run(argc, argv);
}
#endif
