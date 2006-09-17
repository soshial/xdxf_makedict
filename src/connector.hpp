#ifndef __CONNECTOR_HPP__
#define __CONNECTOR_HPP__

#include "parser.hpp"
#include "generator.hpp"

class Connector : public IParserDictOps, public IGeneratorDictOps {
public:
	Connector(GeneratorBase& generator, const std::string& workdir):
		IGeneratorDictOps(generator), workdir_(workdir) {}
	void set_dict_info(const std::string&, const std::string&);
	void send_meta_info();
	void send_info();

	void abbrs_begin();
	void abbrs_end();
	void abbr(const StringList&, const std::string&);
	void article(const StringList&, const std::string&, bool);
	void end();

	bool get_meta_info();
	bool get_info();
	const std::string& get_dict_info(const std::string&) const;
private:
	StringMap dict_info_;
	std::string workdir_;

	void fill_key(const std::string& keystr);
};

#endif//!__CONNECTOR_HPP__
