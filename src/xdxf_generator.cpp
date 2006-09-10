/*
 * This file part of makedict - convertor from any dictionary format to any
 * http://xdxf.sourceforge.net
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

#include <iostream>
#include <fstream>
#include <glib/gi18n.h>

#include "generator.hpp"

namespace xdxf {
	class Generator : public GeneratorBase {
	public:
		Generator() {
			abbr_ = false;
			set_format("xdxf");
			set_version("xdxf_generator, version 0.1");
		}
	protected:
		std::ofstream dict_;
		bool abbr_;

		int generate();
		void on_have_data(const StringList&, const std::string&);
		void on_new_dict_info(const std::string&, const std::string&);
		void on_abbr_begin();
		void on_abbr_end();
		bool on_prepare_generator(const std::string& workdir,
					  const std::string& bname);
	};
}

using namespace xdxf;

void Generator::on_abbr_begin()
{
	dict_ << "<abbreviations>\n";
	abbr_ = true;
}

void Generator::on_abbr_end()
{
	dict_ << "</abbreviations>\n";
	abbr_ = false;
}

void Generator::on_have_data(const StringList& keys,
			     const std::string& data)
{
	if (!abbr_)
		dict_ << "<ar>" << data << "</ar>\n";
	else {
		dict_ << "<abr_def>";
		for (StringList::const_iterator it = keys.begin();
		     it != keys.end(); ++it)
			dict_ << "<k>" << *it << "</k>";

		dict_ << "<v>" << data << "</v></abr_def>\n";
	}
	if (!dict_) {
		std::cerr << _("I/O error, exiting\n");
		exit(EXIT_FAILURE);
	}
}

void Generator::on_new_dict_info(const std::string& name,
				 const std::string& val)
{
	if (name == "lang_from")
		dict_ << "<xdxf lang_from=\"" << val << '"';
	else if (name == "lang_to")
		dict_ <<  " lang_to=\"" << val << "\" format=\"visual\">\n";
	else
		dict_ << "<" << name << ">" << val << "</" << name << ">\n";
}

bool Generator::on_prepare_generator(const std::string& workdir,
				     const std::string& basename)
{
	std::string dirname(workdir + G_DIR_SEPARATOR + basename);

	if (!make_directory(dirname))
		return false;

	std::string icon = get_dict_info("icon");
	if (!icon.empty()) {
		std::string::size_type pos = icon.rfind(".");
		std::string ext;
		if (pos != std::string::npos)
			ext = icon.substr(pos);
		copy_file(icon, dirname + G_DIR_SEPARATOR_S"dict" + ext);
	}

	std::string dictname = dirname + G_DIR_SEPARATOR_S"dict.xdxf";
	dict_.open(dictname.c_str(),
		   std::ios::out | std::ios::binary | std::ios::trunc);
	if (!dict_)
		return false;

	dict_ <<
		"<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
		"<!DOCTYPE xdxf SYSTEM \"http://xdxf.sourceforge.net/xdxf_lousy.dtd\">\n";

	return !dict_ ? false : true;
}

int Generator::generate()
{
	dict_ << "</xdxf>\n";

	return !dict_ ? EXIT_FAILURE : EXIT_SUCCESS;
}



int main(int argc, char *argv[])
{
	return xdxf::Generator().run(argc, argv);
}
