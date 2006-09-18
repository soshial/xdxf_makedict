/*
 * This file is part of makedict - convertor from any
 * dictionary format to any http://xdxf.sourceforge.net
 *
 * Copyright (C) Evgeniy Dushistov, 2006
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
 * parser for stardict format dictionaries from stardict.sf.net
 */

#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif

#include <cerrno>
#include <cstring>
#include <glib/gi18n.h>

#include <algorithm>

#include "file.hpp"
#include "utils.hpp"
#include "mapfile.hpp"

#include "parser.hpp"

namespace stardict {
	class DictData {
	public:
		bool get(guint32 off, guint32 size, std::string& data);
	};

	class Parser : public ParserBase {
	public:
		Parser() {}
	protected:
		int parse(const std::string& filename);
		bool is_my_format(const std::string& url) {
			return g_str_has_suffix(url.c_str(), ".ifo");
		}
	private:
		static const char *magic_;
		gulong nwords_;
		DictData dict_data_;

		bool parse_ifo_file(const std::string&);
	};

	REGISTER_PARSER(Parser, stardict);
}

using namespace stardict;

const char *Parser::magic_ = "StarDict's dict ifo file";

bool DictData::get(guint32 off, guint32 size, std::string& data)
{
	return true;
}

int Parser::parse(const std::string& filename)
{
	if (!g_str_has_suffix(filename.c_str(), ".ifo")) {
		g_warning(_("Expect filename.ifo as argument, got %s\n"), filename.c_str());
		return EXIT_FAILURE;
	}

	if (!parse_ifo_file(filename))
		return EXIT_FAILURE;

	if (!begin())
		return EXIT_FAILURE;

	MapFile index;
	std::string idx_fname = filename;
	idx_fname.erase(idx_fname.length() - 4, 4);
	idx_fname += ".idx";
	if (!index.open(idx_fname.c_str(), true))
		return EXIT_FAILURE;
	const char *p = index.begin(), *end = index.end(), *q;
	guint32 off, size;
	std::string data;
	while ((q = (const char *)memchr(p, '\0', end - p)) != NULL) {
		++q;
		off = g_ntohl(*reinterpret_cast<const guint32 *>(q));
		q += sizeof(guint32);
		size = g_ntohl(*reinterpret_cast<const guint32 *>(q));
		if (!dict_data_.get(off, size, data))
			return EXIT_FAILURE;
		if (!article(p, data, false))
			return EXIT_FAILURE;
		p = q + sizeof(guint32);
	}

	return EXIT_FAILURE;
}

bool Parser::parse_ifo_file(const std::string& filename)
{
	File ifo(fopen(filename.c_str(), "rb"));
	if (!ifo) {
		g_warning(_("Can not open: %s: %s\n"), filename.c_str(),
			strerror(errno));
		return false;
	}
	std::string line;

	if (!File::getline(ifo, line)) {
		g_warning(_("Can not read first line from: %s\n"), filename.c_str());
		return false;
	}

	if (line != magic_) {
		g_warning(_("Magic string did not match, got: %s, expect: %s\n"),
			line.c_str(), magic_);
		return false;
	}
	StringMap ifo_data;

	while (File::getline(ifo, line)) {
		StringList vals = split(line, '=');
		std::for_each(vals.begin(), vals.end(), strip);
		if (vals.size() != 2) {
			g_warning(_("Invalid line, expects such format name=val, got: %s\n"),
				line.c_str());
			return false;
		}
		g_info(_("Read from ifo file: %s: %s\n"), vals[0].c_str(), vals[1].c_str());
		ifo_data[vals[0]] = vals[1];		
	}
#define EXIT_IF_NOT_EXISTS(name) do { \
		if (ifo_data.find(#name) == ifo_data.end()) { \
			g_warning(_("Invalid ifo file content, expect to find %s= into it\n"), \
				#name); \
			return false; \
		} \
	} while (0)

	EXIT_IF_NOT_EXISTS(wordcount);
	EXIT_IF_NOT_EXISTS(idxfilesize);
	EXIT_IF_NOT_EXISTS(version);

	StringMap::const_iterator it;

	if ((it = ifo_data.find("bookname")) != ifo_data.end() &&
	    !it->second.empty())
		set_dict_info("full_name", it->second);
	if ((it = ifo_data.find("description")) != ifo_data.end() &&
	    !it->second.empty())
		set_dict_info("description", it->second);

	return true;
}
