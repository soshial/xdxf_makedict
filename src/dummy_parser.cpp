/*
 * This file part of makedict - convertor from any dictionary format to any
 * http://sdcv.sourceforge.net
 * Copyright (C) 2005 Evgeniy <dushistov@mail.ru>
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

#include "parser.hpp"

namespace dummy {
	class Parser : public ParserBase {
	public:
		Parser() {
			set_parser_info("format", "dummy");
			set_parser_info("version", "dummy_parser, version 1.0");
			parser_options_["lang_from"] = "";
			parser_options_["lang_to"] = "";
		}
	protected:
		int parse(const std::string& url);
	} parser;
}
using namespace dummy;

int Parser::parse(const std::string& url)
{
#if 0
	if (url!="-") {
		std::cout<<"File in not supported format: "<<url<<std::endl;
		return EXIT_FAILURE;
	}
#endif
	set_dict_info("lang_from", parser_options_["lang_from"]);
	set_dict_info("lang_to", parser_options_["lang_to"]);
	begin();
	abbrs_begin();
	abbrs_end();

	std::string key, data;
	while (std::getline(std::cin, key) && std::getline(std::cin, data)) {
		std::vector<std::string> key_list(1, key);
		article(key_list, data);
	}

	return EXIT_SUCCESS;
}

#if 0
int main(int argc, char *argv[])
{
	 return Parser().run(argc, argv);
}
#endif
