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
#include <glib/gi18n.h>
#include <stack>
#include <map>
#include <string>

#include "xml.hpp"
#include "file.hpp"

#include "parser.hpp"

namespace xdxf {
	class Parser : public ParserBase {
	public:
		Parser();
	protected:
		bool is_my_format(const std::string& url) {
			return g_str_has_suffix(url.c_str(), "dict.xdxf");
		}
		int parse(const std::string& url);
		void basename(const std::string& url);
	private:
		enum State {
			stXDXF, stDESCRIPTION, stFULL_NAME, stABBREVIATIONS,
			stABR_DEF, stKEY, stAR, stNU, stOPT, stV
		};
		std::stack<State> state_stack_;
		typedef std::map<std::string, State> StateMap;
		static StateMap state_map_;
		bool send_info_;
		std::string data_;
		std::string keystr_;
		StringList keylist_;

		static void XMLCALL xml_start(void *, const XML_Char *,
					      const XML_Char **);
		static void XMLCALL xml_end(void *, const XML_Char *);
		static void XMLCALL xml_char_data(void *, const XML_Char *, int);
	};
	REGISTER_PARSER(Parser, xdxf);
}

using namespace xdxf;

Parser::StateMap Parser::state_map_;

Parser::Parser() : ParserBase(false)
{
	send_info_ = false;

	state_map_["xdxf"] = stXDXF;
	state_map_["description"] = stDESCRIPTION;
	state_map_["full_name"] = stFULL_NAME;
	state_map_["abbreviations"] = stABBREVIATIONS;
	state_map_["abr_def"] = stABR_DEF;
	state_map_["k"] = stKEY;
	state_map_["ar"] = stAR;
	state_map_["nu"] = stNU;
	state_map_["opt"] = stOPT;
	state_map_["v"] = stV;

	set_parser_info("format", "xdxf");
	set_parser_info("version", "xdxf_parser, version 0.2");
}

void XMLCALL Parser::xml_start(void *arg, const XML_Char *name,
			       const XML_Char **atts)
{
	Parser *parser = static_cast<Parser *>(arg);
	//TODO: std::map::find(const std::string&), const char * -> const std::string
	//may be exists better solution?
	StateMap::const_iterator it = state_map_.find(name);
	if (it == state_map_.end()) {
		if (parser->state_stack_.empty())
			return;

		parser->data_ += '<';
		parser->data_ += name;
		for (int i = 0; atts[i]; i += 2) {
			parser->data_ += ' ';
			parser->data_ += atts[i];
			parser->data_ += "=\"";
			parser->data_ += atts[i+1];
			parser->data_ += "\"";
		}
		parser->data_ += '>';
		if (parser->state_stack_.top() == stKEY) {
			parser->keystr_ += '<';
			parser->keystr_ += name;
			for (int i = 0; atts[i]; i += 2) {
				parser->keystr_ += ' ';
				parser->keystr_ += atts[i];
				parser->keystr_ += "=\"";
				parser->keystr_ += atts[i+1];
				parser->keystr_ += "\"";
			}
			parser->keystr_ += '>';
		}

	} else {
		switch (it->second) {
		case stKEY:
			if (parser->state_stack_.top() == stAR) {
				parser->data_ += '<';
				parser->data_ += name;
				parser->data_ += '>';
			}
			parser->keystr_.clear();
			break;
		case stNU:
			parser->data_ += "<nu />";
			parser->keystr_ += "<nu />";
			break;
		case stV:
			parser->data_.clear();
			break;
		case stOPT:
			parser->data_ += "<opt>";
			parser->keystr_ += "<opt>";
			break;
		case stABR_DEF:
		case stAR:
			if (!parser->send_info_) {
				if (!parser->begin())
					exit(EXIT_FAILURE);
				parser->send_info_ = true;
			}
			parser->data_.clear();
			parser->keylist_.clear();
			parser->keystr_.clear();
			break;
		case stABBREVIATIONS:
			if (!parser->send_info_) {
				if (!parser->begin())
					exit(EXIT_FAILURE);
				parser->send_info_ = true;
			}
			if (!parser->abbrs_begin())
				exit(EXIT_FAILURE);
			break;
		case stXDXF:
			for (int i = 0; atts[i]; i += 2)
				if (strcmp(atts[i], "lang_from") == 0) {
					if (atts[i + 1])
						parser->set_dict_info("lang_from", atts[i + 1]);
				} else if (strcmp(atts[i], "lang_to") == 0) {
					if (atts[i + 1])
						parser->set_dict_info("lang_to", atts[i + 1]);
				}
			break;
		default:
			/*nothing*/break;
		}
		parser->state_stack_.push(it->second);
	}
}

void XMLCALL Parser::xml_end(void *arg, const XML_Char *name)
{
	Parser *parser = static_cast<Parser *>(arg);

	if (parser->state_stack_.empty())
		return;

	//TODO: std::map::find(const std::string&), const char * -> const std::string
	//may be exists better solution?
	StateMap::const_iterator it = state_map_.find(name);

	if (it == state_map_.end() ||
	    parser->state_stack_.top() != it->second) {
		parser->data_ += "</";
		parser->data_ += name;
		parser->data_ += '>';
	} else {
		switch (parser->state_stack_.top()) {
		case stFULL_NAME:
		case stDESCRIPTION:
			parser->set_dict_info(it->first, parser->data_);
			parser->data_.clear();
			break;
		case stOPT:
			parser->data_ += "</opt>";
			parser->keystr_ += "</opt>";
			break;
		case stKEY:
			parser->data_ += std::string("</") + name + ">";
			parser->keylist_.push_back(parser->keystr_);
			parser->keystr_.clear();
			break;
		case stAR:
			if (!parser->article(parser->keylist_, parser->data_, true))
				exit(EXIT_FAILURE);
			parser->keystr_.clear();
			parser->keylist_.clear();
			parser->data_.clear();
			break;
		case stABR_DEF:
			if (!parser->abbr(parser->keylist_, parser->data_))
				exit(EXIT_FAILURE);
			parser->keystr_.clear();
			parser->keylist_.clear();
			parser->data_.clear();
			break;
		case stABBREVIATIONS:
			if (!parser->abbrs_end())
				exit(EXIT_FAILURE);
			break;
#if 1
		case stXDXF:

//TODO: really need? - yes, but why we need this?
			if (!parser->dict_ops_->end())
				exit(EXIT_FAILURE);
			break;
#endif
		case stNU:
		default:
			/*nothing*/break;
		}
		parser->state_stack_.pop();
	}
}

void XMLCALL Parser::xml_char_data(void *arg, const XML_Char *s, int len)
{

	Parser *parser = static_cast<Parser *>(arg);

	if (parser->state_stack_.empty() ||
	    parser->state_stack_.top() == stXDXF ||
	    parser->state_stack_.top() == stABBREVIATIONS)
		return;

#ifdef DEBUG
	StdErr<<"xml_char_data, data="<<parser->data_<<"\n";
#endif

	std::string data;
	xml::encode(std::string(s, len), data);
	parser->data_ += data;
	switch (parser->state_stack_.top()) {
	case stKEY:
	case stOPT:
		parser->keystr_.append(data);
		break;
	default:
		/*nothing*/break;
	}
}

void Parser::basename(const std::string& url)
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

int Parser::parse(const std::string& url)
{
	File ifs(fopen(url.c_str(), "rb"));
	if (!ifs) {
		StdErr.printf(_("Can not open: %s\n"), url.c_str());
		return EXIT_FAILURE;
	}

	xml::Parser xdxf_parser(xml_start, xml_end, xml_char_data, this);

	std::string line;
	while (File::getline(ifs, line)) {
		line += '\n';
		if (!xdxf_parser.parse_line(line))
			return false;
	}

	if (!xdxf_parser.finish(line))
		return false;

	return EXIT_SUCCESS;
}

#if 0
int main(int argc, char *argv[])
{
	 return Parser().run(argc, argv);
}
#endif
