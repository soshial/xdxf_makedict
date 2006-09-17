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

#include <glib/gi18n.h>
#include <cstring>
#include <cerrno>

#include "file.hpp"

#include "generator.hpp"

namespace xdxf {
	class Generator : public GeneratorBase {
	public:
		Generator(): GeneratorBase(false) {
			abbr_ = false;
			set_format("xdxf");
			set_version("xdxf_generator, version 0.2");
		}
	protected:
		File dict_;
		bool abbr_;

		int generate();
		bool on_have_data(const StringList&, const std::string&);
		bool on_new_dict_info(const std::string&, const std::string&);
		bool on_abbr_begin();
		bool on_abbr_end();
		bool on_prepare_generator(const std::string& workdir,
					  const std::string& bname);
	};
	REGISTER_GENERATOR(Generator, xdxf);
}

using namespace xdxf;

bool Generator::on_abbr_begin()
{
	dict_ << "<abbreviations>\n";
	abbr_ = true;
	return !dict_ ? false : true;
}

bool Generator::on_abbr_end()
{
	dict_ << "</abbreviations>\n";
	abbr_ = false;
	return !dict_ ? false : true;
}

bool Generator::on_have_data(const StringList& keys,
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
		StdErr << _("I/O error\n");
		return false;
	}
	return true;
}

bool Generator::on_new_dict_info(const std::string& name,
				 const std::string& val)
{
	if (name == "lang_from")
		dict_ << "<xdxf lang_from=\"" << val << "\"";
	else if (name == "lang_to")
		dict_ <<  " lang_to=\"" << val << "\" format=\"visual\">\n";
	else
		dict_ << "<" << name << ">" << val << "</" << name << ">\n";

	if (!dict_) {
		StdErr << _("I/O error\n");
		return false;
	}
	return true;
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
		if (!copy_file(icon, dirname + G_DIR_SEPARATOR_S"dict" + ext))
			return EXIT_FAILURE;		
	}

	std::string dictname = dirname + G_DIR_SEPARATOR_S"dict.xdxf";
	dict_.reset(fopen(dictname.c_str(), "wb"));

	if (!dict_) {
		StdErr.printf(_("Can not open %s for writting: %s\n"),
			      dictname.c_str(), strerror(errno));
		return false;
	}
	g_message(_("Saving result to: %s\n"), dictname.c_str());
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


#if 0
int main(int argc, char *argv[])
{
	return xdxf::Generator().run(argc, argv);
}
#endif
