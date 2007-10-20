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

/*
 * parser of sdictionary format, see http://sdict.com/en/
 */

#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif

#include <cerrno>
#include <cstring>
#include <glib/gi18n.h>
#include <map>
#include <zlib.h>


#include "langs_2to3.hpp"
#include "normalize_tags.hpp"
#include "utils.hpp"
#include "file.hpp"
#include "xml.hpp"

#include "parser.hpp"

//#define DEBUG
//#define TEST

namespace sdict {

#if defined(_MSC_VER)
#pragma pack(push, 1)
#define __attribute__(arg) /**/
#endif
	struct Header {
		guint8 signature_[4];
		guint8 lang_from_[3];
		guint8 lang_to_[3];
		guint8 compress_method_ : 4;
	        guint8 index_level_: 4;
		guint32 nwords_;
		guint32 short_index_len_;
		guint32 title_offset_;
		guint32 copyright_offset_;
		guint32 version_offset_;
		guint32 short_index_offset_;
		guint32 full_index_offset_;
		guint32 articles_offset_;

		void from_le_to_host() {
			nwords_ = GUINT32_FROM_LE(nwords_);
			short_index_len_ = GUINT32_FROM_LE(short_index_len_);
			title_offset_ = GUINT32_FROM_LE(title_offset_);
			copyright_offset_ = GUINT32_FROM_LE(copyright_offset_);
			version_offset_ = GUINT32_FROM_LE(version_offset_);
			short_index_offset_ = GUINT32_FROM_LE(short_index_offset_);
			full_index_offset_ = GUINT32_FROM_LE(full_index_offset_);
			articles_offset_ = GUINT32_FROM_LE(articles_offset_);
		}
#ifdef DEBUG
		friend File& operator<<(File& out, const Header& head) {
			out << "Articles offset: " << head.articles_offset_ << "\n";
			return out;
		}
#endif
	} __attribute__((packed));
#if defined(_MSC_VER)
#pragma pack(pop)
#endif

#if defined(_MSC_VER)
#pragma pack(push, 1)
#endif
	struct FullIndexEntry {
		guint16 next_word_;
		guint16 prev_word_;
		guint32 article_pointer_;

		void from_le_to_host() {
			next_word_ = GUINT16_FROM_LE(next_word_);
			prev_word_ = GUINT16_FROM_LE(prev_word_);
			article_pointer_ = GUINT32_FROM_LE(article_pointer_);
		}
	} __attribute__((packed));
#if defined(_MSC_VER)
#pragma pack(pop)
#endif

	class Parser : public ParserBase {
	public:
		Parser();
		~Parser() {}
	protected:
		int parse(const std::string& filename);
		bool is_my_format(const std::string& url) {
			return g_str_has_suffix(url.c_str(), ".dct");
		}
	private:
		enum CompressMethod { cmNONE = 0, cmGZIP = 1, cmBZIP2 = 2} compress_method_;
		std::string title, copyright, version;
		std::vector<char> databuf_;
		StringMap langs_;

		static Str2StrTable replace_table;
		static TagInfoList taginfo_list;
		typedef std::map<std::string, guint32> ArticlesMap;
		ArticlesMap articles_map_;

		bool read_unit(File& f, guint32 offset);
		std::string convert_lang(const char *lang, size_t len);
		void convert_article(std::string& encoded_data);
	};
	REGISTER_PARSER(Parser,sdict);
}

using namespace sdict;

Str2StrTable Parser::replace_table;
TagInfoList Parser::taginfo_list;

std::string Parser::convert_lang(const char *lang, size_t len)
{
	char *res = (char *)memchr(lang, '\0', len);
	if (res)
		len = res - lang;
	std::string buf(lang, len);

	tolower(buf);

	StringMap::const_iterator it = langs_.find(buf);
	if (it == langs_.end())
		return "unkown language: " + buf;

	return it->second;
}

Parser::Parser() : langs_(langs_2to3, langs_2to3 + langs_2to3_count)
{
	set_parser_info("format", "sdict");
	set_parser_info("version", "sdict_parser, version 0.2");

	parser_options_["lang_from"] = "";
	parser_options_["lang_to"] = "";
	parser_options_["remove-duplication"] = "no";

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

	taginfo_list.push_back(TagInfo("<sub>", "</sub>", "<sub>", "</sub>",
				       TagInfo::tSub));
	taginfo_list.push_back(TagInfo("<sup>", "</sup>", "<sup>", "</sup>",
				       TagInfo::tSup));
	taginfo_list.push_back(TagInfo("<b>", "</b>", "<b>", "</b>",
				       TagInfo::tB));
	taginfo_list.push_back(TagInfo("<i>", "</i>", "<i>", "</i>",
				       TagInfo::tI));
	taginfo_list.push_back(TagInfo("<u>", "</u>", "<c>", "</c>",
				       TagInfo::tColor));
}

void Parser::convert_article(std::string& encoded_data)
{	
	const char *p = &databuf_[0];
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
			if (i == replace_table.end()) {
				p=beg;
				xml::add_and_encode(encoded_data, *p);
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
}

int Parser::parse(const std::string& filename)
{
	title = copyright = version = "";
	articles_map_.clear();
	bool dupl_mode = false;

	if (parser_options_["remove-duplication"] == "yes")
		dupl_mode = true;

	File f(fopen(filename.c_str(), "rb"));
	if (!f) {
		StdErr.printf(_("Can not open: %s\n"), filename.c_str());
		return EXIT_FAILURE;
	}

	Header header;
	if (!f.read((char *)&header, sizeof(header))) {
		StdErr<<_("Corrupted dictionary or problem with hard disk\n");
		return EXIT_FAILURE;
	}

	header.from_le_to_host();
#ifdef DEBUG
	StdOut << header;
#endif
	if (strncmp((const char *)header.signature_, "sdct", 4) != 0) {
		StdErr << _("Signature did not match\n");
		return EXIT_FAILURE;
	}

	if (parser_options_["lang_from"].empty())
		set_dict_info("lang_from", convert_lang((const char *)header.lang_from_, 3));
	else
		set_dict_info("lang_from", parser_options_["lang_from"]);

	if (parser_options_["lang_to"].empty())
		set_dict_info("lang_to", convert_lang((const char *)header.lang_to_, 3));
	else
		set_dict_info("lang_to", parser_options_["lang_to"]);
	compress_method_ = CompressMethod(header.compress_method_);
	switch (compress_method_) {
	case cmNONE:
	case cmGZIP:
		break;
	case cmBZIP2:
		//break; //TODO: bzip2 not supported yet
	default:
		StdErr.printf(_("Unkown compression method: %d\n"),
			      int(header.compress_method_));
		return EXIT_FAILURE;
	}

	if (!read_unit(f, header.title_offset_))
		return EXIT_FAILURE;
	title = &databuf_[0];
	g_info(_("Sdictionary title: %s\n"), title.c_str());

	if (!read_unit(f, header.copyright_offset_))
		return EXIT_FAILURE;
	copyright = &databuf_[0];
	g_info(_("Sdictionary copyrights: %s\n"), copyright.c_str());

	if (!read_unit(f, header.version_offset_))
		return EXIT_FAILURE;
	version = &databuf_[0];
	g_info(_("Sdictionary version: %s\n"), version.c_str());

	if (!f.seek(header.full_index_offset_)) {
		StdErr << _("Corrupted dictionary or problem with hard disk\n");
		return EXIT_FAILURE;
	}
	set_dict_info("full_name", title);
	set_dict_info("description", std::string("Copyright: ")+copyright+"; Version: "+version);

	if (!begin())
		return EXIT_FAILURE;

	std::string encoded_data;
	FullIndexEntry index_entry;
	guint32 wordcount = header.nwords_;
	size_t index_size;

	for (gulong i = 0; i < wordcount; ++i) {
		if (!f.read((char *)&index_entry, sizeof(index_entry))) {
			StdErr << _("Can not read full index entry\n");
			return EXIT_FAILURE;
		}

		index_entry.from_le_to_host();

		index_size = index_entry.next_word_ - sizeof(index_entry);

		std::vector<char> index_value(index_size + 1);
		if (!f.read(&index_value[0], index_size)) {
			StdErr << _("Can not read word/phrase\n");
			return EXIT_FAILURE;
		}
		index_value[index_size] = '\0';
		
		const char *q = &index_value[0];
		std::string encoded_index;
		while (*q) {
			xml::add_and_encode(encoded_index, *q);
			++q;
		}
		remove_not_valid(encoded_index);


		if (!read_unit(f, header.articles_offset_ +
			       index_entry.article_pointer_)) {
			StdErr << _("Can not read article\n");
			return EXIT_FAILURE;
		}

		if (dupl_mode) {
			ArticlesMap::const_iterator ami =
				articles_map_.find(encoded_index);
			if (ami != articles_map_.end()) {
				std::vector<char> origin = databuf_;
				if (!read_unit(f, header.articles_offset_ +
					       ami->second)) {
					StdErr << _("Can not read article\n");
					return EXIT_FAILURE;
				}
				if (strcmp(&databuf_[0], &origin[0]) == 0) {
					g_info(_("Warrning: Duplication of the `%s' article. "
						 "Skipped.\n"),
					       encoded_index.c_str());
					continue;
				}
				databuf_ = origin;
			}
			articles_map_[encoded_index] = index_entry.article_pointer_;
		}
		encoded_data.resize(0);//may be this help clear std::string without free memory
		convert_article(encoded_data);
#if 1
		if (!g_utf8_validate(encoded_data.c_str(), gssize(-1), NULL)) {
			StdErr.printf(_("Not valid UTF-8\n"
					"Original before conversation: %s\n"),
				      &databuf_[0]);
			return EXIT_FAILURE;
		}
#endif

		if (!article(encoded_index, encoded_data, false))
			return EXIT_FAILURE;
	}


	return EXIT_SUCCESS;
}

bool Parser::read_unit(File& f, guint32 offset)
{
	long cur_off = f.tell();
	if (cur_off == -1) {
		StdErr.printf(_("Can not get current positions in the file: %s\n"),
			      strerror(errno));
		return false;
	}

	if (!f.seek(offset)) {
		StdErr.printf(_("Can not set current position to: %u\n"), offset);
		return false;
	}
	guint32 record_size;
	
	if (!f.read((char *)&record_size, sizeof(guint32))) {
		StdErr << _("Can not read unit size\n");
		return false;
	}

	record_size = GUINT32_FROM_LE(record_size);
	
	if (databuf_.size() < record_size+1)
		databuf_.resize(record_size + 1);
	
	if (!f.read(&databuf_[0], record_size)) {
		StdErr << _("Can not read unit data\n");
		return false;
	}
	
	if (compress_method_ == cmGZIP) {
		std::vector<char> dest(record_size * 4);
		uLongf dest_len;
		for(;;) {
			dest_len = dest.size();
			int res = uncompress((Bytef *)&dest[0], (uLongf *)&dest_len,
					     (Bytef *)&databuf_[0], record_size);
			
			if (Z_OK == res)
				break;
			if (Z_BUF_ERROR == res) {
				dest.resize(dest.size() + record_size);
				continue;
			}
			
			return false;
		}
		
		databuf_ = dest;
		
		if (databuf_.size() <= dest_len + 1)
			databuf_.resize(dest_len + 1);
		databuf_[dest_len] = '\0';
	} else
		databuf_[record_size] = '\0';

	if (!f.seek(cur_off)) {
		StdErr.printf(_("Can not set position in the file to %ld: %s\n"),
			      cur_off, strerror(errno));
		return false;
	}	

	return true;
}

#if 0
int main(int argc, char *argv[])
{
	 return Parser().run(argc, argv);
}
#endif
