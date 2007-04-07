/*
 * This file part of makedict - convertor from any dictionary format to any
 * http://xdxf.sourceforge.net
 * Copyright (C) 2005-2006 Evgeniy <dushistov@mail.ru>
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

//$Id$

#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif

#include <cstdlib>

#include "xml.hpp"
#include "file.hpp"

#include "parser.hpp"

namespace {
	class DummyParser : public ParserBase {
	public:
		DummyParser() {
			set_parser_info("format", DUMMY_FORMAT_NAME);
			set_parser_info("version", "dummy_parser, version 1.0");
			parser_options_["lang_from"] = "";
			parser_options_["lang_to"] = "";
		}
	protected:
		int parse(const std::string& url);
	};

	REGISTER_PARSER(DummyParser, dummy);

	int DummyParser::parse(const std::string& url)
	{
#if 0
		if (url!="-") {
			std::cout<<"File in not supported format: "<<url<<std::endl;
			return EXIT_FAILURE;
		}
#endif
		set_dict_info("lang_from", parser_options_["lang_from"]);
		set_dict_info("lang_to", parser_options_["lang_to"]);
		if (!begin() || !abbrs_begin() || !abbrs_end())
			return EXIT_FAILURE;

		std::string key, data, enc_data, enc_key;
		while (File::getline(StdIn, key) && File::getline(StdIn, data)) {
			enc_key = enc_data = "";
			xml::encode(key, enc_key);
			xml::encode(data, enc_data);

			if (!article(enc_key, enc_data, false))
				return EXIT_FAILURE;
		}

		return EXIT_SUCCESS;
	}
}//end of anonymous namespace

#ifdef DUMMY_PARSER_ALONE
int main(int argc, char *argv[])
{
	 return DummyParser().run(argc, argv);
}
#endif
