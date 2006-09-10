/*
 * This file part of makedict - convertor from any 
 * dictionary format to any http://xdxf.sourceforge.net
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

#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif

#include <cstdlib>
#include <string>
#include <iostream>
using namespace std;

#include "generator.hpp"

class dummy_generator : public GeneratorBase {
public:
	dummy_generator()
	{
		set_format("dummy");
		set_version("dummy_generator, version 1.0");
	}
protected:
	int generate();
	void on_have_data(const StringList& keys,
			  const std::string& data);
	bool on_prepare_generator(const std::string&,
				  const std::string&) { 
		return true; 
	}
		
};

int dummy_generator::generate()
{	
	return EXIT_SUCCESS;
}

void dummy_generator::on_have_data(const StringList& keys,
				   const std::string& data)
{
	for (StringList::const_iterator p = keys.begin(); p != keys.end(); ++p)
		cout << "key: " << *p << endl;
	cout << "data: " << data << endl;
}

int main(int argc, char *argv[])
{
	return dummy_generator().run(argc, argv);
}
