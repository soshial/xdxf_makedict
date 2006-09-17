/*
 * This file is part of makedict - convertor from any
 * dictionary format to any http://xdxf.sourceforge.net
 *
 * Copyright (C) Evgeniy Dushistov, 2005
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
#include <iostream>
#include <fstream>
#include <vector>

#include "charset_conv.hpp"
#include "mapfile.hpp"
#include "utils.hpp"

#include "parser.hpp"

using std::string;
using std::vector;
using std::cerr;
using std::cout;
using std::endl;

//#define DEBUG

class dictionary {
public:
	explicit dictionary(const string& tmpfilename);
	~dictionary();
	void add_article(const string& keyval, const string& data);
	size_t narticles() const { return keylist.size(); }
	void get_article(size_t i, string &key, string& data);
private:
	string datafilename;
	std::fstream datafile;
	struct key {
		string val;
		gulong off, size;
		key(const string& v, gulong o, gulong s) : val(v), off(o), size(s) {}
	};
	vector<key> keylist;
};

void dictionary::add_article(const string& keyval, const string& data)
{
	keylist.push_back(key(keyval, datafile.tellp(), data.length()));
	if (!datafile.write(data.c_str(), data.length()))
		cerr<<_("WARRNING: dictionary::add_article: write failed")<<endl;
}

dictionary::dictionary(const string& tmpfilename)
{
	datafilename=tmpfilename;
	datafile.open(tmpfilename.c_str(), std::ios::in|std::ios::out|std::ios::binary|std::ios::trunc);
	if (!datafile)
		cerr<<_("WARRNING: dictionary::dictionary: can not open: ")<<datafilename<<endl;
}

void dictionary::get_article(size_t i, string &key, string& data)
{
	if (i>=keylist.size())
		return;
	if (!datafile.seekg(keylist[i].off, std::ios::beg)) {
		cerr<<_("WARRNING: dictionary::get_article: seek failed")<<endl;
		return;
	}
	key=keylist[i].val;
	data.resize(keylist[i].size);
	if (!datafile.read(&data[0], data.length()))
		cerr<<_("WARRNING: dictionary::get_article: read failed")<<endl;
}

dictionary::~dictionary()
{
	remove(datafilename.c_str());
}

class dictd_parser : public ParserBase {
public:
  dictd_parser();
  ~dictd_parser() {} 
  int parse(const string & filename);
	bool is_my_format(const std::string& url) { 
		return g_str_has_suffix(url.c_str(), ".index");
	}
private:
  vector<char> data_buffer;
  static Str2StrTable replace_table;
};

Str2StrTable dictd_parser::replace_table;

dictd_parser::dictd_parser()
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

int dictd_parser::parse(const string& filename)
{  
	int res=EXIT_FAILURE;

	dictionary dict(filename+".tmp");
	if (filename.substr(filename.length()-6, 6)!=".index")
		return res;
	if (!is_file_exist(filename)) {
		cerr<<filename<<_(" does not exist\n");
		return res;
	}
	string data_filename(filename);
	data_filename.replace(filename.length()-6, 6, ".dict");

	if (!is_file_exist(data_filename)) {
		cerr<<_("File does not exist: ")<<data_filename<<endl;
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
		cerr<<_("Corrupted dictionary or problem with hard disk\n");
		return res;
	}

	std::ifstream data(data_filename.c_str(), std::ios::in | std::ios::binary);
	if (!data) {
		cerr<<_("Can not open: ")<<data_filename<<endl;
		return res;
	}

	set_dict_info("lang_from", parser_options_["lang_from"]);
	set_dict_info("lang_to", parser_options_["lang_to"]);

	char *offset_p, *size_p;
	string enc_str;
	char *p=mapfile.begin();
	string description;
	char *end=mapfile.end();
	
	while (p!=end) {
		if ((offset_p=mapfile.find_str(p, "\t"))==end) {
			cerr<<_("Invalid index file\n");
			return res;
		}
		string key(p, offset_p-p);
		string tmp;
		replace(replace_table, key.c_str(), tmp);
		string conv_str;
		if (encoding)
			conv.convert(tmp, key);
		else
			key=tmp;

		if (!g_utf8_validate(key.c_str(), -1, NULL)) {
			cerr<<_("Not valid utf8 index string\n");
			return res;
		}

		offset_p++;
    
		if ((size_p=mapfile.find_str(offset_p, "\t"))==end) {
			cerr<<_("Invalid index file\n");
			return res;
		}
		string offset_str(offset_p, size_p-offset_p);

		size_p++;
		if ((p=mapfile.find_str(size_p, "\n"))==end) {
			cerr<<_("Invalid index file\n");
			return res;
		}
		string size_str(size_p, p-size_p);

		p++;

		long data_offset=b64_decode(offset_str.c_str());
		long data_size=b64_decode(size_str.c_str());
		if (!data.seekg(data_offset)) {
			cerr<<_("Corrupted dictionary or problem with hard disk\n");
			return res;
		}
		data_buffer.resize(data_size);
		if (!data.read(&data_buffer[0], data_buffer.size())) {
			cerr<<_("Corrupted dictionary or problem with hard disk\n");
			return res;
		}

		vector<char>::reverse_iterator ri;
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
			cerr<<_("Not valid utf8 string: ")<<&data_buffer[0]<<endl;
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
				description+=string("\nSource url: ")+s;
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
	begin();
	string key, dictdata;
	for (size_t i=0; i<dict.narticles(); ++i) {
		dict.get_article(i, key, dictdata);
		if (!key.empty())
			article(StringList(1, key), dictdata, false);		
	}

	res=EXIT_SUCCESS;

	return res;
}

int main(int argc, char *argv[])
{
	dictd_parser parser;
	return parser.run(argc, argv);
}
