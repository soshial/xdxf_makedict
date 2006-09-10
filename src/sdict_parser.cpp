/*
 * This file is part of makedict - convertor from any
 * dictionary format to any http://sdcv.sourceforge.net
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
#include <fstream>
#include <glib/gi18n.h>
#include <iostream>
#include <map>
#include <zlib.h>


#include "langs_2to3.hpp"
#include "normalize_tags.hpp"
#include "utils.hpp"

#include "parser.hpp"

//#define DEBUG
//#define TEST

class sdict_parser : public ParserBase {
public:
  sdict_parser();
  ~sdict_parser() {}
protected:
  int parse(const std::string& filename);
	bool is_my_format(const std::string& url) { 
		return g_str_has_suffix(url.c_str(), ".dct"); 
	}
private:
  enum {none, gzip, bzip2} compression_method;
  std::string title, copyright, version;
  std::vector<char> data_buffer;
  
  static Str2StrTable replace_table;
  typedef std::map<char, char*, std::less<char> > Char2Str; 
  static  Char2Str text2xml;  

  static TagInfoList taginfo_list;
	std::map<std::string, std::string> langs;

  bool read_unit(std::ifstream& f, guint32 offset);
	std::string convert_lang(const std::string& lang)
	{
		std::string buf(lang);
		tolower(buf);
		std::map<std::string, std::string>::iterator it=langs.find(buf);
		if (it==langs.end())
			return "unkown language: "+lang;

		return it->second;
	}
};

Str2StrTable sdict_parser::replace_table;
sdict_parser::Char2Str sdict_parser::text2xml;
TagInfoList sdict_parser::taginfo_list;


sdict_parser::sdict_parser() : langs(langs_2to3, langs_2to3+langs_2to3_count)
{
	set_parser_info("format", "sdict");
	set_parser_info("version", "sdict_parser, version 0.1");

	parser_options_["lang_from"]="";
	parser_options_["lang_to"]="";

	not_valid_chars.insert(0x02);
	not_valid_chars.insert(0x03);
	not_valid_chars.insert(0x05);
	not_valid_chars.insert(0x06);
	not_valid_chars.insert(0x07);
	not_valid_chars.insert(0x08);
	not_valid_chars.insert(0x0C);
	not_valid_chars.insert(0x0E);
	not_valid_chars.insert(0x0F);
	not_valid_chars.insert(0x14);
	not_valid_chars.insert(0x15);
	not_valid_chars.insert(0x16);
	not_valid_chars.insert(0x1B);
	not_valid_chars.insert(0x1C);
	not_valid_chars.insert(0x1D);

	if (!replace_table.empty())
		return;
	replace_table["<br>"]=replace_table["<br />"]="\n";
	replace_table["<p>"]="\n\n";
	replace_table["<r>"]="<kref>";
	replace_table["</r>"]="</kref>";
	replace_table["<t>"]="<tr>";
	replace_table["</t>"]="</tr>";
	replace_table["<l>"]="";
	replace_table["</l>"]="";
	replace_table["<li>"]="";
	replace_table["</li>"]="";

	//FIXME: find out what it form of word and change to aporopriate tag
	replace_table["<f>"]="";
	replace_table["</f>"]="";
	replace_table["&Icirc;"]="Î";

	taginfo_list.push_back(TagInfo("<sub>", "</sub>", "<sub>", "</sub>", TagInfo::tSub));
	taginfo_list.push_back(TagInfo("<sup>", "</sup>", "<sup>", "</sup>", TagInfo::tSup));
	taginfo_list.push_back(TagInfo("<b>", "</b>", "<b>", "</b>", TagInfo::tB));
	taginfo_list.push_back(TagInfo("<i>", "</i>", "<i>", "</i>", TagInfo::tI));
	taginfo_list.push_back(TagInfo("<u>", "</u>", "<c>", "</c>", TagInfo::tColor));

	text2xml['<']="&lt;";
	text2xml['>']="&gt;";
	text2xml['&']="&amp;";
	text2xml['\"']="&quot;";  
}

int sdict_parser::parse(const std::string& filename)
{
	guint16 next_word, prev_word;
	guint32 article_pointer, index_size;
	guint32 wordcount, short_index_length;
	guint32 title_offset, copyright_offset, version_offset,
		short_index_offset, full_index_offset, articles_offset;

	title=copyright=version="";
	int res=EXIT_FAILURE;
	std::ifstream f(filename.c_str(), std::ios::binary | std::ios::in);
	if (!f) {
		std::cerr<<_("Can not open: ")<<filename<<std::endl;
		return res;
	}
	guint8 buffer[10];
	if (!f.read((char *)buffer, sizeof(guint8)*4)) {
		std::cerr<<_("Corrupted dictionary or problem with hard disk\n");
		return res;
	}
	if (strncmp((char *)buffer, "sdct", 4)!=0) {
		std::cerr<<_("Signature did not match\n");
		return res;
	}
	if (!f.read((char *)buffer, sizeof(guint8)*3)) {
		std::cerr<<_("Corrupted dictionary or problem with hard disk\n");
		return res;
	}
	buffer[3]=0;
	if (parser_options_["lang_from"].empty())
		set_dict_info("lang_from", convert_lang((char *)buffer));
	else
		set_dict_info("lang_from", parser_options_["lang_from"]);

	if (!f.read((char *)buffer, sizeof(guint8)*3)) {
		std::cerr<<_("Corrupted dictionary or problem with hard disk\n");
		return res;
	}
	buffer[3]=0;
	if (parser_options_["lang_to"].empty())
		set_dict_info("lang_to", convert_lang((char *)buffer));
	else
		set_dict_info("lang_to", parser_options_["lang_to"]);

	if (!f.read((char *)buffer, sizeof(guint8))) {
		std::cerr<<_("Corrupted dictionary or problem with hard disk\n");
		return res;
	}
	switch (buffer[0]) {
	case '0':
		compression_method=none;
#ifdef DEBUG
		std::cout<<"Compression method: none\n";
#endif
		break;
	case '1':
		compression_method=gzip;
#ifdef DEBUG
		std::cout<<"Compression method: gzip\n";
#endif
		break;
	case '2':
#ifdef DEBUG
		compression_method=bzip2;
#endif
		std::cout<<_("Compression method: bzip2\n");
		break;
	default:
		std::cerr<<_("Unkown compression method: ")<<char(buffer[0])<<std::endl;
		return res;
	}

	if (!f.read((char *)&wordcount, sizeof(guint32))) {
		std::cerr<<_("Corrupted dictionary or problem with hard disk\n");
		return res;
	}
#ifdef DEBUG
	std::cout<<_("Amount of words: ")<<wordcount<<std::endl;
#endif
	if (!f.read((char *)&short_index_length, sizeof(guint32))) {
		std::cerr<<_("Corrupted dictionary or problem with hard disk\n");
		return res;
	}
#ifdef DEBUG
	std::cout<<_("Short index length: ")<<short_index_length<<std::endl;
#endif

	if (!f.read((char *)&title_offset, sizeof(guint32))) {
		std::cerr<<_("Corrupted dictionary or problem with hard disk\n");
		return res;
	}

	if (!f.read((char *)&copyright_offset, sizeof(guint32))) {
		std::cerr<<_("Corrupted dictionary or problem with hard disk\n");
		return res;
	}
	if (!f.read((char *)&version_offset, sizeof(guint32))) {
		std::cerr<<_("Corrupted dictionary or problem with hard disk\n");
		return res;
	}
	if (!f.read((char *)&short_index_offset, sizeof(guint32))) {
		std::cerr<<_("Corrupted dictionary or problem with hard disk\n");
		return res;
	}
	if (!f.read((char *)&full_index_offset, sizeof(guint32))) {
		std::cerr<<_("Corrupted dictionary or problem with hard disk\n");
		return res;
	}
	if (!f.read((char *)&articles_offset, sizeof(guint32))) {
		std::cerr<<_("Corrupted dictionary or problem with hard disk\n");
		return res;
	}
	if (!read_unit(f, title_offset)) 
		return res;
	title=&(data_buffer[0]);
#ifdef DEBUG
	std::cout<<_("Title: ")<<title<<std::endl;
#endif
	if (!read_unit(f, copyright_offset))
		return res;
	copyright=&(data_buffer[0]);
#ifdef DEBUG
	std::cout<<_("Copyright: ")<<copyright<<std::endl;
#endif
	if (!read_unit(f, version_offset))
		return res;
	version=&(data_buffer[0]);
#ifdef DEBUG
	std::cout<<_("Versioin: ")<<version<<std::endl;
#endif
	if (!f.seekg(full_index_offset)) {
		std::cerr<<_("Corrupted dictionary or problem with hard disk\n");
		return res;
	}
	set_dict_info("full_name", title);
	set_dict_info("description", std::string("Copyright: ")+copyright+"; Version: "+version);

	begin();

	std::string encoded_data;  

	for (guint32 i=0; i<wordcount; ++i) {      
		if (!f.read((char *)&next_word, sizeof(guint16))) {
			std::cerr<<_("Can not read next_word field")<<std::endl;
			return res;
		}
		if (!f.read((char *)&prev_word, sizeof(guint16))) {
			std::cerr<<_("Can not read prev_word field")<<std::endl;
			return res;
		}
		if (!f.read((char *)&article_pointer, sizeof(guint32))) {
			std::cerr<<_("Can not read article_pointer")<<std::endl;
			return res;
		}

		index_size=next_word-sizeof(guint16)*2-sizeof(guint32);

		std::string index_value(index_size, char());
		if (!f.read(&index_value[0], index_size)) {
			std::cerr<<_("Can not read word/phrase")<<std::endl;
			return res;
		}

		Char2Str::iterator c2si;
		const char *q=index_value.c_str();
		std::string encoded_index;
		while (*q) {
			if ((c2si=text2xml.find(*q))==text2xml.end())
				encoded_index+=*q;
			else
				encoded_index+=c2si->second;
			++q;
		}
		remove_not_valid(encoded_index);

		long cur_offset=f.tellg();
		if (!read_unit(f, articles_offset+article_pointer)) {
			std::cerr<<_("Can not read article")<<std::endl;
			return res;
		}

		encoded_data.resize(0);//may be this help clear std::string without free memory
		const char *p=&data_buffer[0];
		NormalizeTags norm_tags(taginfo_list);
		while (*p) {
			if (*p=='<') {
				if (*(p+1)!='/') {
					if (!norm_tags.add_open_tag(encoded_data, p))
						goto end_of_handle;
				} else {
					if (!norm_tags.add_close_tag(encoded_data, p))
						goto end_of_handle;
				}
			} else {
			end_of_handle:
				const char *beg=p;
				Str2StrTable::const_iterator i;
				for (i=replace_table.begin(); i!=replace_table.end(); ++i) {
					p=beg;
					const char *q=i->first;
					while (*p && *q && *p==*q)
						++p, ++q;
					
					if (*q=='\0') {
						encoded_data+=i->second;
						break;
					}
				}
				if (i==replace_table.end()) {
					p=beg;
					if ((c2si=text2xml.find(*p))==text2xml.end())
						encoded_data+=*p;
					else
						encoded_data+=c2si->second;
					++p;
				}
			}
		}
		{
			std::string datastr;
			norm_tags(encoded_data, datastr);
			encoded_data=datastr;
		}

		remove_not_valid(encoded_data);

#if 1
		if (!g_utf8_validate(encoded_data.c_str(), gssize(-1), NULL)) {
			std::cerr<<_("Not valid utf-8")<<std::endl;
			return res;
		}
#endif
		
		std::vector<std::string> key_list(1, encoded_index);
		article(key_list, encoded_data);
		
		f.seekg(cur_offset);
	}

  
	res=EXIT_SUCCESS;

	return res;
}

bool sdict_parser::read_unit(std::ifstream& f, guint32 offset)
{
  if (!f.seekg(offset)) {
    std::cerr<<_("Can not set current position to: ")<<offset<<std::endl;
    return false;
  }
  guint32 record_size;
  //FIXME: add reoder bytes in integer
  if (!f.read((char *)&record_size, sizeof(guint32))) {
    std::cerr<<_("Can not read unit size")<<std::endl;
    return false;
  }

  if (data_buffer.size() < record_size+1) 
    data_buffer.resize(record_size+1);
  
  if (!f.read(&data_buffer[0], sizeof(gchar)*record_size)) {
    std::cerr<<_("Can not read unit data")<<std::endl;
    return false;
  }
  
  if (compression_method==gzip) {
    std::vector<char> dest;
    dest.resize(record_size*4);
    uLongf dest_len;
    for(;;) {
      dest_len=dest.size();
      int res=uncompress((Bytef *)&dest[0], (uLongf *)&dest_len, 
			 (Bytef *)&data_buffer[0], record_size);

      if (Z_OK==res)
				break;
      if (Z_BUF_ERROR==res) {
				dest.resize(dest.size()+record_size);
				continue;
      }

      return false;
    }

    data_buffer=dest;
    
    if (data_buffer.size()<=dest_len+1)
      data_buffer.resize(dest_len+1);
    data_buffer[dest_len]='\0';
  } else {
    data_buffer[record_size]='\0';
  }

  return true;
}

int main(int argc, char *argv[])
{
	sdict_parser parser;
	return parser.run(argc, argv);
}
