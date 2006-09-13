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

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <glib/gi18n.h>

#include "parser.hpp"

class xdxf_parser : public ParserBase {
public:
	xdxf_parser() : ParserBase(false)
	{
		set_parser_info("format", "xdxf");
		set_parser_info("version", "xdxf_parser, version 0.1");
	}
protected:
	bool is_my_format(const std::string& url) { 
		return g_str_has_suffix(url.c_str(), "dict.xdxf"); 
	}
	int parse(const std::string& url);
	void basename(const std::string& url);
};

void xdxf_parser::basename(const std::string& url)
{
	std::string basename(url);
	std::string::size_type pos = basename.rfind(G_DIR_SEPARATOR);
	if (pos != std::string::npos)
		basename.erase(pos);
	pos = basename.rfind(G_DIR_SEPARATOR);
	if (pos != std::string::npos)
		basename.erase(0, pos+1);
	set_dict_info("basename", basename);
}

int xdxf_parser::parse(const std::string& url)
{
	meta_info();
	std::ifstream ifs(url.c_str());
	if (!ifs) {
		std::cerr<<_("Can not open: ")<<url<<std::endl;
		return EXIT_FAILURE;
	}
	std::string line;
	while (std::getline(ifs, line))
		std::cout << line << '\n';	

	return EXIT_SUCCESS;
}

int main(int argc, char *argv[])
{	 
	 return xdxf_parser().run(argc, argv);
}

