#ifndef _XML_HPP_
#define _XML_HPP_

#include <string>

class Xml {
public:
	static void add_and_encode(std::string& str, char ch);
	static void encode(const std::string& str, std::string& res);
private:
	static const char raw_entrs[];
	static const char* xml_entrs[];
};

#endif//!_XML_HPP_
