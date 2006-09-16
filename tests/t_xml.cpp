#include <cstdlib>
#include <glib.h>

#include "xml.hpp"

int main()
{
	std::string str;
	Xml::add_and_encode(str, '<');
	g_assert(str == "&lt;");
	Xml::add_and_encode(str, '\'');
	g_assert(str == "&lt;&apos;");
	return EXIT_SUCCESS;
}
