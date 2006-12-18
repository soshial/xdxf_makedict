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

#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif

#include <cerrno>
#include <cstring>
#include <cstdio>
#include <glib/gi18n.h>
#include <vector>

#include "charset_conv.hpp"
#include "mapfile.hpp"
#include "utils.hpp"

#include "parser.hpp"


//#define DEBUG
namespace dictd {
/**
 * Temporary repository for dictionary.
 */
class dictionary {
public:
	explicit dictionary(const std::string& tmpfilename);
	~dictionary();
	void add_article(const std::string& keyval, const std::string& data);
	size_t narticles() const { return keylist.size(); }
	void get_article(size_t i, std::string &key, std::string& data);
private:
	std::string datafilename;
	File datafile;
	struct key {
		std::string val;
		gulong off, size;
		key(const std::string& v, gulong o, gulong s) : val(v), off(o), size(s) {}
	};
	std::vector<key> keylist;
};
}
void dictd::dictionary::add_article(const std::string& keyval,
				    const std::string& data)
{
	keylist.push_back(key(keyval, datafile.tell(), data.length()));
	if (!datafile.write(data.c_str(), data.length()))
		StdErr << _("WARRNING: dictionary::add_article: write failed\n");
}

dictd::dictionary::dictionary(const std::string& tmpfilename)
{
	datafilename=tmpfilename;
	datafile.reset(fopen(tmpfilename.c_str(), "w+b"));
	if (!datafile)
		StdErr.printf(_("WARRNING: dictionary::dictionary: can not open: %s\n"),
			      datafilename.c_str());
}

void dictd::dictionary::get_article(size_t i, std::string &key,
				    std::string& data)
{
	if (i>=keylist.size())
		return;
	if (!datafile.seek(keylist[i].off)) {
		StdErr << _("WARRNING: dictionary::get_article: seek failed\n");
		return;
	}
	key=keylist[i].val;
	data.resize(keylist[i].size);
	if (!datafile.read(&data[0], data.length()))
		StdErr << _("WARRNING: dictionary::get_article: read failed\n");
}

dictd::dictionary::~dictionary()
{
	remove(datafilename.c_str());
}
namespace dictd {
	class Parser : public ParserBase {
	public:
		Parser();
		~Parser() {} 
		int parse(const std::string & filename);
		bool is_my_format(const std::string& url) { 
			//to fix stupid compiler warning
			if (g_str_has_suffix(url.c_str(), ".index"))
				return true;
			else
				return false;
		}
	private:
		std::vector<char> data_buffer;
		static Str2StrTable replace_table;
	};

	REGISTER_PARSER(Parser, dictd);
}

using namespace dictd;

Str2StrTable Parser::replace_table;

Parser::Parser()
{
	set_parser_info("format", "dictd");
	set_parser_info("version", "dictd_parser, version 0.1");

	parser_options_["encoding"]="";
	parser_options_["lang_from"]="";
	parser_options_["lang_to"]="";

	not_valid_chars.insert(0x1A);
	not_valid_chars.insert(0x12);
	not_valid_chars.insert(0x15);
	not_valid_chars.insert(0x07);
	not_valid_chars.insert(0x0E);
	not_valid_chars.insert(0x0B);
	not_valid_chars.insert(0x01);
	not_valid_chars.insert(0x08);
	not_valid_chars.insert(0x1F);
	not_valid_chars.insert(0x04);
	not_valid_chars.insert(0x0C);
	not_valid_chars.insert(0x18);


	if (!replace_table.empty())
		return;
	replace_table["<"]="&lt;";
	replace_table[">"]="&gt;";
	replace_table["&"]="&amp;";
	replace_table["\""]="&quot;";
}

static inline const char *skip(const char *str, const char *skip)
{
	if (g_str_has_prefix(str, skip))
		str+=strlen(skip);
	while (*str && (*str==' ' || *str=='\t' || *str=='\r' || *str=='\n')) 
		++str;
	return str;
}

int Parser::parse(const std::string& filename)
{  
	int res=EXIT_FAILURE;

	dictionary dict(filename+".tmp");
	if (filename.substr(filename.length()-6, 6)!=".index")
		return res;
	if (!is_file_exist(filename)) {
		StdErr.printf(_("%s does not exist\n"), filename.c_str());
		return res;
	}
	std::string data_filename(filename);
	data_filename.replace(filename.length()-6, 6, ".dict");

	if (!is_file_exist(data_filename)) {
		StdErr.printf(_("File does not exist: %s\n"), data_filename.c_str());
		return res;
	}
	bool encoding=false;
	CharsetConv conv;
	if (!parser_options_["encoding"].empty()) {
		encoding=true;
		conv.workwith(parser_options_["encoding"].c_str(), "UTF-8");
	}
	MapFile mapfile;
  
	if (!mapfile.open(filename.c_str())) {
		StdErr << _("Corrupted dictionary or problem with hard disk\n");
		return res;
	}

	File data(fopen(data_filename.c_str(), "rb"));
	if (!data) {
		StdErr.printf(_("Can not open: %s\n"), data_filename.c_str());
		return res;
	}

	set_dict_info("lang_from", parser_options_["lang_from"]);
	set_dict_info("lang_to", parser_options_["lang_to"]);

	char *offset_p, *size_p;
	std::string enc_str;
	char *p=mapfile.begin();
	std::string description;
	char *end=mapfile.end();
	
	while (p!=end) {
		if ((offset_p=mapfile.find_str(p, "\t"))==end) {
			StdErr << _("Invalid index file\n");
			return res;
		}
		std::string key(p, offset_p-p);
		std::string tmp;
		replace(replace_table, key.c_str(), tmp);
		std::string conv_str;
		if (encoding)
			conv.convert(tmp, key);
		else
			key=tmp;

		if (!g_utf8_validate(key.c_str(), -1, NULL)) {
			StdErr << _("Not valid utf8 index string\n");
			return res;
		}

		offset_p++;
    
		if ((size_p=mapfile.find_str(offset_p, "\t"))==end) {
			StdErr << _("Invalid index file\n");
			return res;
		}
		std::string offset_str(offset_p, size_p-offset_p);

		size_p++;
		if ((p=mapfile.find_str(size_p, "\n"))==end) {
			StdErr << _("Invalid index file\n");
			return res;
		}
		std::string size_str(size_p, p-size_p);

		p++;

		long data_offset=b64_decode(offset_str.c_str());
		size_t data_size=b64_decode(size_str.c_str());
		if (!data.seek(data_offset)) {
			StdErr << _("Corrupted dictionary or problem with hard disk\n");
			return res;
		}
		data_buffer.resize(data_size);
		if (!data.read(&data_buffer[0], data_buffer.size())) {
			StdErr << _("Corrupted dictionary or problem with hard disk\n");
			return res;
		}

		std::vector<char>::reverse_iterator ri;
		//remove last not neeadable characters
		for (ri=data_buffer.rbegin(); ri!=data_buffer.rend() && 
			     (*ri=='\r' || *ri=='\n' || *ri==' ' || *ri=='\t'); ++ri)
			;

		if (ri!=data_buffer.rbegin() && ri!=data_buffer.rend())
			*(ri-1)='\0';
		else
			data_buffer.push_back('\0');

		
		if (encoding)
			conv.convert(&data_buffer[0], conv_str);
		else
			conv_str=&data_buffer[0];

		if (!g_utf8_validate(conv_str.c_str(), -1, NULL)) {
			StdErr.printf(_("Not valid utf8 string: %s\n"), &data_buffer[0]);
			return res;
		}

		replace(replace_table, conv_str.c_str(), enc_str);
		remove_not_valid(enc_str);
		data_size=enc_str.length();

		if (g_str_has_prefix(key.c_str(), "00database") || 
		    g_str_has_prefix(key.c_str(), "00-database-")) {
			if (key=="00databaseurl" || key=="00-database-url") {
				const char *s=enc_str.c_str();
				s=skip(s, "00databaseurl");
				s=skip(s, "00-database-url");
				description += std::string("\nSource url: ")+s;
			} else if (key=="00databasescript" || key=="00-database-script" ||
				   key=="00-database-info" || key=="00databaseinfo") {
				description+=enc_str;
			} else if (key=="00databaseshort" || key=="00-database-short") {
				const char *dict_name=enc_str.c_str();
				dict_name=skip(dict_name, "00databaseshort");
				dict_name=skip(dict_name, "00-database-short");
				set_dict_info("full_name", dict_name);
			}			
		} else
			dict.add_article(key, enc_str);		
	}

	set_dict_info("description", description);
	if (!begin())
		return EXIT_FAILURE;
	std::string key, dictdata;
	for (size_t i=0; i<dict.narticles(); ++i) {
		dict.get_article(i, key, dictdata);
		if (!key.empty())
			if (!article(key, dictdata, false))
				return EXIT_FAILURE;
	}

	res=EXIT_SUCCESS;

	return res;
}

#if 0
int main(int argc, char *argv[])
{
	 return Parser().run(argc, argv);
}
#endif
