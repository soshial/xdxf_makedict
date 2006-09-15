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

#include <glib/gi18n.h>
#include <getopt.h>
#include <unistd.h>
#include <iostream>

#include "utils.hpp"

#include "parser.hpp"

void PipeParserDictOps::send_meta_info()
{
	out_ << "<meta_info>\n";
	for (StringMap::const_iterator p = dict_info_.begin(); 
	     p != dict_info_.end(); ++p)
		out_ << "<" << p->first << ">"
			  << p->second 
			  << "</" << p->first << ">\n";

	out_ << "</meta_info>\n";
}

void PipeParserDictOps::send_info()
{
	out_ << 
		"<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
		"<!DOCTYPE xdxf SYSTEM \"http://xdxf.sourceforge.net/xdxf_lousy.dtd\">\n"
		  << "<xdxf lang_from=\"" << dict_info_["lang_from"]
		  << "\" lang_to=\"" << dict_info_["lang_to"]
		  << "\" format=\"visual\">\n"
		"<full_name>" << dict_info_["full_name"] << "</full_name>\n"
		"<description>" << dict_info_["description"]
		  << "</description>\n";
}

void PipeParserDictOps::abbrs_begin()
{
	out_ << "<abbreviations>\n";
}

void PipeParserDictOps::abbrs_end()
{
	out_ << "</abbreviations>\n";
}

void PipeParserDictOps::abbr(const StringList& keys,
			     const std::string& val)
{
	out_ << "<abr_def>";
	for (StringList::const_iterator p = keys.begin(); p != keys.end(); ++p)
		out_ << "<k>" << Strip(*p) << "</k>";
	out_ << "<v>" << val << "</v>"
		  << "</abr_def>\n";
}

void PipeParserDictOps::article(const StringList& keys, const std::string& val)
{
	out_ << "<ar>";
	for (StringList::const_iterator p = keys.begin(); p != keys.end(); ++p)
		out_ << "<k>" << Strip(*p) << "</k>\n";

	out_ << val << "</ar>\n";

	if (!out_)
		StdErr << _("Pipe write error\n");
}

void PipeParserDictOps::end()
{
	out_ << "</xdxf>\n";
	out_.flush();
}

const std::string& ParserBase::format() const
{ 
	static std::string empty;

	StringMap::const_iterator it = parser_info_.find("format");

	if (it == parser_options_.end())		
		return empty;
	
	return it->second;
}

ParserBase::ParserBase(bool generate_xdxf)
{	
	generate_xdxf_ = generate_xdxf;
	std_dict_ops_.reset(new PipeParserDictOps(StdOut));
	dict_ops_ = std_dict_ops_.get();
}

bool ParserBase::parse_option(const std::string& optarg)
{
	std::vector<std::string> l = split(optarg, '=');			
	if (l.size() != 2) {
		StdErr <<
			_("Invalid usage of parser-option: didn't find '=' in option\n");
		return false;
	}
	StringMap::iterator opt_ptr = parser_options_.find(l[0]);
	if (opt_ptr == parser_options_.end()) {
		StdErr << _("Invalid parser option, possible options:\n");
		for (StringMap::iterator it = parser_options_.begin();
		     it != parser_options_.end(); ++it)
			StdErr << it->first << "\n";
		return false;
	}
	opt_ptr->second = l[1];
	return true;
}

int ParserBase::run(int argc, char *argv[])
{
	static const char *shortopts = "hi";
	static const option lopts[] = {
		{"version", no_argument, NULL, 0 },
		{"help", no_argument, NULL, 'h' },
		{"input-format", no_argument, NULL, 'i'},
		{"is-your-format", required_argument, NULL, 1},
		{"parser-option", required_argument, NULL, 2},
		{ NULL, 0, NULL, 0 },
	};
	int option_index, optc;
	std::string help =
		_("Usage: program path/to/dictionary\n"
			"-h, --help            display this help and exit\n"
			"--version             output version information and exit\n"
			"-i, --input-format    output supported input format and exit\n"
			"--is-your-format file test if file in format which accept this codec\n"
			"--parser-option \"option_name=option_value\"\n");

	while ((optc = getopt_long(argc, argv, shortopts, &lopts[0],
				   &option_index))!=-1) {
		switch (optc) {
		case 'h':
			StdOut << help << "\n";
			return EXIT_SUCCESS;
		case 0:
			StdOut << parser_info_["version"] << "\n";
			return EXIT_SUCCESS;
		case 1:
			return is_my_format(optarg) ? EXIT_SUCCESS : EXIT_FAILURE;
		case 'i':
			StdOut << parser_info_["format"] << "\n";
			return EXIT_SUCCESS;
		case 2:
			if (!parse_option(optarg))
				return EXIT_FAILURE;		
			break;
		case '?':
		default:
			StdErr << help << "\n";
			return EXIT_FAILURE;
		}
	}
	if (optind == argc) {
		StdErr.printf(_("%s: no input files\n"), argv[0]);
		return EXIT_FAILURE;
	}
	return do_run(argv[optind]);
}

int ParserBase::run(const StringList& options, const std::string& url)
{
	for (StringList::const_iterator it = options.begin(); 
	     it != options.end(); ++it)
		if (!parse_option(*it))
			return EXIT_FAILURE;
	return do_run(url);
}

int ParserBase::do_run(const std::string& url)
{
	basename(url);
	int res = parse(url);
	if (generate_xdxf_)
		dict_ops_->end();
	return res;
}

void ParserBase::basename(const std::string& url)
{
	std::string basename(url);
	std::string::size_type pos = basename.rfind(G_DIR_SEPARATOR);
	if (pos != std::string::npos)
		basename.erase(0, pos+1);

	if ((pos = basename.rfind('.')) != std::string::npos)
		basename.erase(pos, basename.length() - pos);
	set_dict_info("basename", basename);
}

void ParserBase::set_parser_info(const std::string& key, const std::string& val)
{
	parser_info_[key] = val;
}

void ParserBase::meta_info()
{
	dict_ops_->send_meta_info();
}

void ParserBase::begin()
{
	meta_info();		
	dict_ops_->send_info();
}

void ParserBase::set_dict_info(const std::string& key, const std::string& val)
{
	dict_ops_->set_dict_info(key, val);
}

void ParserBase::abbrs_begin()
{
	dict_ops_->abbrs_begin();
}

void ParserBase::abbrs_end()
{
	dict_ops_->abbrs_end();
}

void ParserBase::abbr(const StringList& keys, const std::string& val)
{
	dict_ops_->abbr(keys, val);
}

void ParserBase::article(const StringList& keys, const std::string& val)
{
	dict_ops_->article(keys, val);
}

int ParserBase::parse(const std::string& url)
{
	return EXIT_SUCCESS;
}

void ParserBase::remove_not_valid(std::string &str)
{		
	std::string valid_data;	 
	char utf8buf[8];
	for (const char *p = str.c_str(); *p; p = g_utf8_next_char(p)) {
		gunichar ch = g_utf8_get_char(p);
		if (not_valid_chars.find(ch) == not_valid_chars.end()) {
			utf8buf[g_unichar_to_utf8(ch, utf8buf)] = '\0';
			valid_data += utf8buf;
		}/* else
			std::cerr<<_("Not valid character was removed, its code: ")<<ch<<std::endl;*/
	}
	str = valid_data;
}

ParserBase *ParsersRepo::create_suitable_parser(const std::string& url)
{
	CodecsMap::const_iterator it;
	for (it = codecs_.begin(); it != codecs_.end(); ++it) {
		ParserBase *res = (*it->second).create();
		if (res->is_my_format(url))
			return res;
		delete res;
	}

	return NULL;
}
