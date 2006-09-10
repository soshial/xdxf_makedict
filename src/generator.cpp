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

#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif

#include <cstring>
#include <glib/gi18n.h>
#include <unistd.h>
#include <getopt.h>
#include <glib.h>
#include <iostream>
#include <numeric>

#include "utils.hpp"

#include "generator.hpp"

//#define DEBUG

GeneratorDictPipeOps::TagTable GeneratorDictPipeOps::tag_table_;
Str2StrTable GeneratorDictPipeOps::xml_spec_seq_;

class xml_parser {
public:
	xml_parser(XML_StartElementHandler on_start,
						 XML_EndElementHandler on_end,
						 XML_CharacterDataHandler on_char,
						 void *data);
	~xml_parser() {	XML_ParserFree(xmlp); }
	void xml_error(const std::string& line) const {
		std::cerr << _("xml parser error: ")
			  << XML_ErrorString(XML_GetErrorCode(xmlp)) << std::endl
			  <<_("Can not parse such line: ") << line << std::endl;
	}
	bool parse_line(const std::string& line) {
		if (XML_Parse(xmlp, &line[0], line.size(), 0)!=XML_STATUS_OK) {
			xml_error(line);
			return false;
		}

		return true;
	}
	bool finish(const std::string& line) {
		if (XML_Parse(xmlp, &line[0], line.size(), 1)!=XML_STATUS_OK) {
			xml_error(line);
			return false;
		}

		return true;
	}
private:
	XML_Parser xmlp;
};

xml_parser::xml_parser(XML_StartElementHandler on_start,
		       XML_EndElementHandler on_end,
		       XML_CharacterDataHandler on_char,
		       void *data)
{
	xmlp = XML_ParserCreate("UTF-8");
	XML_SetElementHandler(xmlp, on_start, on_end);
	XML_SetCharacterDataHandler(xmlp, on_char);
	XML_SetUserData(xmlp, data);
}

GeneratorDictPipeOps::GeneratorDictPipeOps(GeneratorBase& gen) :
	IGeneratorDictOps(gen)
{
	meta_mode_ = mmNONE;

	if (!xml_spec_seq_.empty())
		return;

	xml_spec_seq_["<"]="&lt;";
	xml_spec_seq_[">"]="&gt;";
	xml_spec_seq_["&"]="&amp;";
	xml_spec_seq_["\""]="&quot;";

	tag_table_["full_name"] = FULL_NAME;
	tag_table_["description"] = DESCRIPTION;
	tag_table_["abr_def"] = ABR_DEF;
	tag_table_["ar"] = AR;
	tag_table_["k"] = K;
	tag_table_["v"] = V;
	tag_table_["opt"] = OPT;
	tag_table_["xdxf"] = XDXF;
	tag_table_["abbreviations"] = ABBREVIATIONS;
}

bool GeneratorDictPipeOps::get_meta_info()
{
	std::string line;

	xml_parser meta_parser(on_meta_start, on_meta_end, on_meta_data, this);

	while (std::getline(std::cin, line) &&
	       line.find("</meta_info>") == std::string::npos) {
		line += '\n';
		if (!meta_parser.parse_line(line))
			return false;
	}

	if (!meta_parser.finish(line))
		return false;

	return true;
}

bool GeneratorDictPipeOps::get_info()
{
	std::string line;

	xml_parser xdxf_parser(xml_start, xml_end, xml_char_data, this);

	while (std::getline(std::cin, line)) {
		line += '\n';
		if (!xdxf_parser.parse_line(line))
			return false;
	}
	if (!xdxf_parser.finish(line))
		return false;
	return true;
}

GeneratorBase::GeneratorBase()
{
	dict_ops_.reset(new GeneratorDictPipeOps(*this));
}

int GeneratorBase::run(int argc, char *argv[])
{
	static const char *shortopts = "hob:w:";
	static const option lopts[]= {
		{"version", no_argument, NULL, 0 },
		{"help", no_argument, NULL, 'h' },
		{"output-format", no_argument, NULL, 'o'},
		{"basename", required_argument, NULL, 'b'},
		{"work-dir", required_argument, NULL, 'w'},
		{NULL, 0, NULL, 0},
	};
	int option_index, optc;
	std::string help =
		_("Usage: program [-w dir]"
			"-h, --help          display this help and exit\n"
			"--version           output version information and exit\n"
			"-o, --output-format display supported output format and exit\n"
			"-w, --work-dir dir  use 'dir' as current directory");

	std::string workdir;

	while ((optc = getopt_long(argc, argv, shortopts, &lopts[0],
				   &option_index))!=-1) {
		switch (optc) {
		case 'h':
			std::cout<<help<<std::endl;
			return EXIT_SUCCESS;
		case 0:
			std::cout << version_ << std::endl;
			return EXIT_SUCCESS;
		case 'o':
			std::cout << format_ << std::endl;
			return EXIT_SUCCESS;
		case 'w':
			workdir=optarg;
			break;
		case '?':
		default:
			std::cerr<<help<<std::endl;
			return EXIT_FAILURE;
		}
	}

	if (optind != argc) {
		std::cout<<help<<std::endl;
		return EXIT_FAILURE;
	}
	if (workdir.empty()) {
		std::string appname(argv[0]);
		std::string::size_type pos=appname.rfind(G_DIR_SEPARATOR);

		if (pos!=std::string::size_type(-1)) {
			workdir.assign(appname, 0, pos);
		} else
			workdir=".";
	}
	if (!dict_ops_->get_meta_info())
		return EXIT_FAILURE;
	if (!on_prepare_generator(workdir, dict_ops_->get_dict_info("basename")))
		return EXIT_FAILURE;
	if (!dict_ops_->get_info())
		return EXIT_FAILURE;
	return generate();
}


void XMLCALL GeneratorDictPipeOps::on_meta_start(void *user_arg,
						 const XML_Char *name,
						 const XML_Char **atts)
{
	GeneratorDictPipeOps *mg =
		static_cast<GeneratorDictPipeOps *>(user_arg);
	if (strcmp(name, "icon") == 0)
		mg->meta_mode_ = mmICON;
	else if (strcmp(name, "basename") == 0)
		mg->meta_mode_ = mmBASENAME;
}

void XMLCALL GeneratorDictPipeOps::on_meta_end(void *user_arg,
					       const XML_Char *name)
{
	GeneratorDictPipeOps *mg =
		static_cast<GeneratorDictPipeOps *>(user_arg);
	if (strcmp(name, "icon")==0 || strcmp(name, "basename")==0)
		mg->meta_mode_ = mmNONE;
}

void XMLCALL GeneratorDictPipeOps::on_meta_data(void *user_arg,
						const XML_Char *s, int len)
{
	GeneratorDictPipeOps *mg =
		static_cast<GeneratorDictPipeOps *>(user_arg);

	switch (mg->meta_mode_) {
	case mmICON:
		mg->dict_info_["icon"] += std::string(s, len);
		break;
	case mmBASENAME:
		mg->dict_info_["basename"] += std::string(s, len);
		break;
	default:
		/*nothing*/;
	}
}


void XMLCALL GeneratorDictPipeOps::xml_start(void *user_arg,
					     const XML_Char *name,
					     const XML_Char **atts)
{
#ifdef DEBUG
	std::cerr << "xml_start: name=" << name << std::endl;
#endif

	GeneratorDictPipeOps *gen =
		static_cast<GeneratorDictPipeOps *>(user_arg);
	TagTable::const_iterator it = tag_table_.find(name);

	if (it == tag_table_.end() && gen->state_stack_.empty())
		return;

	if (it == tag_table_.end()) {
		gen->data_ += std::string("<") + name;
		for (int i = 0; atts[i]; i += 2)
			gen->data_ += std::string(" ") + atts[i] +"=\"" +
				atts[i+1] + "\"";
		gen->data_ += ">";
	} else {
		switch (it->second) {
		case K:
			if (gen->state_stack_.top() == AR)
				gen->data_ += std::string("<")+name+">";
			gen->key_.parts_.push_back(std::string());
			break;
		case V:
			gen->data_.clear();
			break;
		case OPT:
			gen->data_ += std::string("<") + name + ">";
			gen->key_.opts_.push_back(std::string());
			gen->key_.parts_.push_back(std::string());
			break;
		case ABR_DEF:
		case AR:
			gen->data_.clear();
			break;
		case ABBREVIATIONS:
			gen->generator_.on_abbr_begin();
			break;
		case XDXF:

			for (int i = 0; atts[i]; i += 2)
				if (strcmp(atts[i], "lang_from") == 0) {
					if (atts[i + 1])
						gen->generator_.set_dict_info("lang_from", atts[i + 1]);
				} else if (strcmp(atts[i], "lang_to") == 0) {
					if (atts[i + 1])
						gen->generator_.set_dict_info("lang_to", atts[i + 1]);
				}
			break;
		default:
			/*nothing*/;
		}
		gen->state_stack_.push(it->second);
#ifdef DEBUG
		std::cerr << "xml_start: tag added, data=" << gen->data_
			  << std::endl;
#endif
	}
}

void GeneratorBase::set_dict_info(const std::string& name,
				  const std::string& val)
{
	dict_ops_->set_dict_info(name, val);
	on_new_dict_info(name, val);
}

void XMLCALL GeneratorDictPipeOps::xml_end(void *userData, const XML_Char *name)
{
#ifdef DEBUG
	std::cerr<<"xml_end: name="<<name<<std::endl;
#endif

	GeneratorDictPipeOps *gen = static_cast<GeneratorDictPipeOps *>(userData);

	if (gen->state_stack_.empty())
		return;

	TagTable::const_iterator it = tag_table_.find(name);

	if (it == tag_table_.end() ||
	    gen->state_stack_.top() != it->second) {
		gen->data_ += std::string("</") + name + ">";
	} else {
		switch (gen->state_stack_.top()) {
		case FULL_NAME:
		case DESCRIPTION:
			gen->generator_.set_dict_info(it->first, gen->data_);
			gen->data_.clear();
			break;
		case OPT:
			gen->data_ += std::string("</") + name + ">";
			break;
		case K:
			gen->data_ += std::string("</") + name + ">";
			gen->generate_keys();
			gen->key_.clear();
			break;
		case AR:
		case ABR_DEF:
			gen->generator_.on_have_data(gen->keys_, gen->data_);
			gen->keys_.clear();
			gen->data_.clear();
			break;
		case ABBREVIATIONS:
			gen->generator_.on_abbr_end();
			break;
		default:
			/*nothing*/;
		}
		gen->state_stack_.pop();
	}
}

void XMLCALL GeneratorDictPipeOps::xml_char_data(void *userData,
						 const XML_Char *s, int len)
{
	GeneratorDictPipeOps *gen = static_cast<GeneratorDictPipeOps *>(userData);

	if (gen->state_stack_.empty() || gen->state_stack_.top() == XDXF ||
	    gen->state_stack_.top() == ABBREVIATIONS)
		return;

#ifdef DEBUG
	std::cerr<<"xml_char_data, data="<<gen->data_<<std::endl;
#endif

	std::string data;
	replace(xml_spec_seq_, std::string(s, len).c_str(), data);
	gen->data_ += data;
	switch (gen->state_stack_.top()) {
	case K:
		gen->key_.parts_.back() += std::string(s, len);
		break;
	case OPT:
		gen->key_.opts_.back() += std::string(s, len);
		break;
	default:
		/*nothing*/;
	}
}



void GeneratorDictPipeOps::sample(std::vector<std::string>::size_type n)
{
	if (n==0) {
		std::string res =
			accumulate(sample_data_.begin(), sample_data_.end(),
				   std::string());
		keys_.push_back(res);
		return;
	}
	sample_data_.push_back(key_.parts_[n + 1 - key_.opts_.size()]);
	sample(n-1);
	sample_data_.pop_back();

	sample_data_.push_back(key_.opts_[key_.opts_.size() - n]);
	sample_data_.push_back(key_.parts_[key_.opts_.size() - n + 1]);
	sample(n-1);
	sample_data_.pop_back();
	sample_data_.pop_back();
}

void GeneratorDictPipeOps::generate_keys()
{
	if (key_.parts_.empty()) {
		std::cerr<<_("Internal error, can not generate key list, there is no parts of key")
			 << std::endl;
		return;
	}
	sample_data_.clear();
	sample_data_.push_back(key_.parts_.front());
	sample(key_.opts_.size());

	std::for_each(keys_.begin(), keys_.end(), strip);
}
