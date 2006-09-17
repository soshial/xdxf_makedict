#include <cstdlib>
#include <glib.h>

#include "xml.hpp"

int main()
{
	std::string str;
	xml::add_and_encode(str, '<');
	g_assert(str == "&lt;");
	xml::add_and_encode(str, '\'');
	g_assert(str == "&lt;&apos;");
	return EXIT_SUCCESS;
}
