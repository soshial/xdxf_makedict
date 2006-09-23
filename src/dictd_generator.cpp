/*
 * This file part of makedict - convertor from any
 * dictionary format to any http://xdxf.sourceforge.net
 * Copyright (C) 2006 Evgeniy <dushistov@mail.ru>
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

#include <cstdlib>
#include <cstring>
#include <cerrno>
#include <glib/gi18n.h>

#include <string>
#include <vector>
#include <algorithm>

#include "generator.hpp"

namespace dictd {
	class Generator : public GeneratorBase {
	public:
		Generator(): GeneratorBase(true)
			{
				set_format("dictd");
				set_version("dictd_generator, version 0.1");
			}
		~Generator()
			{
				for (std::vector<dictd_key>::const_iterator p=keys_list.begin();
				     p!=keys_list.end(); ++p)
					delete [] p->val;
				for (std::vector<char *>::const_iterator p=coord_list.begin();
				     p!=coord_list.end(); ++p)
					delete [] *p;
			}
	protected:
		struct dictd_key {
			char *val;
			char *coord;
			dictd_key(char *v, char *c) : val(v), coord(c) {}
		};

		struct dictd_less {
			bool operator()(const dictd_key& lh, const dictd_key& rh) {
				return strcmp(lh.val, rh.val)<0;
			}
		};

		std::vector<dictd_key> keys_list;
		std::vector<char *> coord_list;
		std::string dict_file_name;
		File dict_file_;
		File idx_file_;

		int generate();
		bool on_have_data(const StringList&, const std::string&);
		void add_headword(const std::string& name,
				  const std::string& val) {
			on_have_data(StringList(1, name), val);
		}
		bool on_prepare_generator(const std::string& workdir,
					  const std::string& basename);
	};
	REGISTER_GENERATOR(Generator, dictd);
}

using namespace dictd;

bool Generator::on_prepare_generator(const std::string& workdir,
					   const std::string& basename)
{
	std::string dirname=workdir+G_DIR_SEPARATOR+basename;

	if (!make_directory(dirname))
		return false;

	std::string realbasename = dirname+G_DIR_SEPARATOR+basename;

	std::string file_name=realbasename+".index";
	idx_file_.reset(fopen(file_name.c_str(), "wb"));

	if (!idx_file_) {
		StdErr.printf(_("Can not open/create: %s\n"), file_name.c_str());
		return false;
	}
	g_message(_("Write index to %s\n"), file_name.c_str());
	dict_file_name=realbasename+".dict";
	dict_file_.reset(fopen(dict_file_name.c_str(), "wb"));

	if (!dict_file_) {
		StdErr.printf(_("Can not open/create: %s\n"), dict_file_name.c_str());
		return false;
	}
	g_message(_("Write data to %s\n"), dict_file_name.c_str());
	return true;
}

int Generator::generate()
{
	add_headword("00databaseutf8", "00-database-utf8\n\n");
	add_headword("00databaseshort", get_dict_info("full_name"));
	add_headword("00databaseinfo", get_dict_info("description"));

	std::sort(keys_list.begin(), keys_list.end(), dictd_less());

	for (std::vector<dictd_key>::const_iterator p=keys_list.begin(); p!=keys_list.end(); ++p)
		idx_file_ << p->val << p->coord << "\n";

	return EXIT_SUCCESS;
}

static inline char *new_string(const std::string& str)
{
	char *ptr=new char [str.length()+1];
	memcpy(ptr, str.c_str(), str.length()+1);
	return ptr;
}

bool Generator::on_have_data(const StringList& keys, const std::string& data)
{
	long off = dict_file_.tell();
	if (off == -1) {
		StdErr.printf(_("Can not get position in the file: %s\n"),
			      strerror(errno));
		return false;
	}
	size_t size = data.size();
	if (!dict_file_.write(&data[0], data.size())) {
		StdErr.printf(_("Can not write to: %s\n"), dict_file_name.c_str());
		return false;
	}

	char *coord = new_string(std::string("\t") + 
		b64_encode(off) + "\t" + b64_encode(guint32(size)));
	coord_list.push_back(coord);
	for (StringList::const_iterator p = keys.begin(); p != keys.end(); ++p)
		keys_list.push_back(dictd_key(new_string(*p), coord));

	return true;
}

#if 0
int main(int argc, char *argv[])
{
	return Generator().run(argc, argv);
}
#endif
