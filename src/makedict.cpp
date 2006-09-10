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

#ifdef HAVE_LOCALE_H
#  include <clocale>
#endif
#include <cerrno>
#include <cstdlib>
#include <fstream>
#include <getopt.h>
#include <glib.h>
#include <glib/gi18n.h>
#include <iostream>
#include <map>
#include <string>
#include <iomanip>
#include <sys/types.h>
#include <sys/wait.h>

#include "utils.hpp"
#include "parser.hpp"

//#define DEBUG


class makedict {
public:
	int run(int argc, char *argv[]);
private:
	StringMap input_codecs;
	StringMap output_codecs;
	std::string parser_options;
	std::string workdir;
	std::string input_format, output_format;

	int convert(const char *arg);
	const char *find_input_codec(const std::string& url);
	ParserBase *find_parser(const std::string& url);
	bool fill_codecs_table(const std::string& prgname, const std::string& dirname="");
	void list_codecs();
	void unknown_input_format(const std::string& format = "");
};

static void unknown_output_format(const StringMap& output_codecs, const std::string& format="");
static long width_of_first(const StringMap &sm);

void makedict::list_codecs()
{
	long w = width_of_first(input_codecs);
	std::cout << _("Input formats:") << std::endl;
	for (StringMap::const_iterator it = input_codecs.begin();
	     it != input_codecs.end(); ++it)
		std::cout << std::setiosflags(std::ios::left) << std::setw(w)
			  << it->first << "  " << _("input codec: ")
			  << it->second << std::endl;

	StringList myformats = ParsersRepo::get_instance().supported_formats();
	for (StringList::const_iterator it = myformats.begin();
	     it != myformats.end(); ++it)
		std::cout << std::setiosflags(std::ios::left) << std::setw(w)
			  << *it << "  " << _("supported by me")
			  << std::endl;

	w = width_of_first(output_codecs);
	std::cout << _("Output formats:") << std::endl;
	for (StringMap::const_iterator it = output_codecs.begin();
	     it != output_codecs.end(); ++it)
		std::cout << std::setiosflags(std::ios::left) << std::setw(w)
			  << it->first << "  " << _("output codec: ")
			  << it->second << std::endl;
}


int makedict::run(int argc, char *argv[])
{
#ifdef HAVE_LOCALE_H
	setlocale(LC_ALL, "");
#endif

	const gchar *dir = g_getenv("MAKEDICT_PLUGIN_DIR");
	if (!fill_codecs_table(argv[0], dir ? dir : CODECSDIR) &&
	    !fill_codecs_table(argv[0], ""))
		return EXIT_FAILURE;

#ifdef DEBUG
	for (StringMap::iterator p=input_codecs.begin(); p!=input_codecs.end(); ++p)
		std::cout<<p->first<<" "<<p->second<<std::endl;

	for (StringMap::iterator p=output_codecs.begin(); p!=output_codecs.end(); ++p)
		std::cout<<p->first<<" "<<p->second<<std::endl;
#endif
	option longopts[] ={
		{"version", no_argument, NULL, 'v' },
		{"help", no_argument, NULL, 'h' },
		{"input-format", required_argument, NULL, 'i'},
		{"ouput-format", required_argument, NULL, 'o'},
		{"work-dir", required_argument, NULL, 0},
		{"list-supported-formats", no_argument, NULL, 'l'},
		{"parser-option", required_argument, NULL, 1},
		{ NULL, 0, NULL, 0 }
	};

	bool h=false, v=false, l=false;
	int option_index, optc;
	std::string is_your_format;

	while ((optc = getopt_long(argc, argv, "hvi:o:lp:", longopts,
				   &option_index))!=-1)
		switch (optc) {
		case 'i':
		{
			StringMap::const_iterator i = input_codecs.find(optarg);
			input_format = optarg;
			if (i == input_codecs.end() && !find_parser("")) {
				unknown_input_format(optarg);
				return EXIT_FAILURE;
			}
		}
		break;
		case 'o':
		{
			StringMap::iterator i=output_codecs.find(optarg);
			if (i==output_codecs.end()) {
				unknown_output_format(output_codecs, optarg);
				return EXIT_FAILURE;
			}
			output_format=optarg;
		}
		break;
		case 'v':
			v = true;
			break;
		case 'h':
			h = true;
			break;
		case 'l':
			l = true;
			break;
		case 0:
			workdir=optarg;
			break;
		case 1:
			parser_options+=" "+std::string("--parser-option \"")+optarg+"\"";
			break;
		case '?':
		default:
			std::cerr <<
				g_strdup_printf(_("Unknwon option.\nTry '%s --help' for more information.\n"),
						argv[0]);
			return EXIT_FAILURE;
		}

	if (l) {
		list_codecs();
		return EXIT_SUCCESS;
	}

	if (h || optind >= argc) {
		std::cout<<_("Usage: makedict [OPTIONS] file1 file2...\n"
			     "-v, --version                print version information and exit\n"
			     "-h, --help                   display this help and exit\n"
			     "-i, --input-format           format of input file\n"
			     "-o, --output-format          format of output file\n"
			     "--work-dir                   root directory for all created dictionaries\n"
			     "-l, --list-supported-formats list all supported formats\n"
			     "--parser-option \"option_name=option_value\"\n");
		return EXIT_SUCCESS;
	}

	if (v) {
		std::cout<<_("Utiltity for creating dictionaries, v")<<VERSION<<std::endl;
		return EXIT_SUCCESS;
	}

	for (int i=optind; i<argc; ++i)
		if (convert(argv[i])!=EXIT_SUCCESS)
			return EXIT_FAILURE;

	return EXIT_SUCCESS;
}

static bool start_cmd(const std::string& cmd, gchar *&std_output)
{
	gchar *std_error;
	gint exit_status;
	if (g_spawn_command_line_sync(cmd.c_str(), &std_output, &std_error, &exit_status, NULL) &&
	    WEXITSTATUS(exit_status)==EXIT_SUCCESS) {
		size_t len=strlen(std_output);
		while (len>0 && g_ascii_isspace(std_output[len-1]))
			--len;
		std_output[len]='\0';
		return true;
	}
	return false;
}

bool makedict::fill_codecs_table(const std::string& prgname, const std::string& dirname_)
{
	gchar *tmp_dirname=g_path_get_dirname(prgname.c_str());
	std::string dirname(dirname_.empty() ? tmp_dirname : dirname_);
	g_free(tmp_dirname);

	GError *err=NULL;
	GDir *dir = g_dir_open(dirname.c_str(), 0, &err);
	if (dir==NULL) {
		std::cerr<<err->message<<std::endl;
		g_error_free(err);
		return false;
	}

	gchar *th = g_path_get_basename(prgname.c_str());
	const gchar *filename;
	while ((filename = g_dir_read_name(dir)) != NULL) {
		std::string realname(dirname+G_DIR_SEPARATOR+filename);
		gchar *cur = g_path_get_basename(realname.c_str());
		if (strcmp(cur, th)==0) {
			g_free(cur);
			continue;
		}
		g_free(cur);
		if (g_file_test(realname.c_str(), G_FILE_TEST_IS_EXECUTABLE)) {
			gchar *std_output;

			if (start_cmd("'" + realname + "' --input-format",
				      std_output)) {
				input_codecs[std_output] = realname;
				continue;
			}

			if (start_cmd("'" + realname + "' --output-format",
				      std_output)) {
				output_codecs[std_output] = realname;
				continue;
			}

		}
	}
	g_free(th);
	g_dir_close(dir);

	return true;
}

static void unknown_output_format(const StringMap& output_codecs, const std::string& format)
{
	if (!format.empty())
		std::cerr<<_("Unknown output format: ")<<format<<std::endl;

	std::cerr<<_("Possible output formats are:")<<std::endl;

	for (StringMap::const_iterator i=output_codecs.begin(); i!=output_codecs.end(); ++i)
		std::cerr<<i->first<<std::endl;
}

static long width_of_first(const StringMap &sm)
{
	long res=0;
	for (StringMap::const_iterator it=sm.begin(); it!=sm.end(); ++it)
		if (res<long(it->first.length()))
			res=it->first.length();
	return res;
}

void makedict::unknown_input_format(const std::string& format)
{
	if (!format.empty())
		std::cerr<<_("Unknown input format: ") << format << std::endl;

	std::cerr << _("Possible input formats are:") << std::endl;
	for (StringMap::const_iterator i = input_codecs.begin();
	     i != input_codecs.end(); ++i)
		std::cerr << i->first << std::endl;

	StringList myformats = ParsersRepo::get_instance().supported_formats();
	for (StringList::const_iterator it = myformats.begin();
	     it != myformats.end(); ++it)
		std::cerr << *it << std::endl;
}

const char *makedict::find_input_codec(const std::string &url)
{
	if (!input_format.empty()) {
		StringMap::const_iterator it = input_codecs.find(input_format);
		if (it != input_codecs.end())
			return it->second.c_str();
	} else {
		for (StringMap::iterator it = input_codecs.begin();
		     it != input_codecs.end(); ++it) {
			int status =
				system((it->second + " --is-your-format '" + url + "'").c_str());
			if (status == -1) {
				std::cout << _("system function failed: ")
					  << strerror(errno) << std::endl;
				exit(EXIT_FAILURE);
			}
			if (WEXITSTATUS(status) == EXIT_SUCCESS)
				return it->second.c_str();
		}
	}

	return NULL;
}

ParserBase *makedict::find_parser(const std::string& url)
{
	ParserBase *parser = NULL;

	if (!input_format.empty())
		parser =
			ParsersRepo::get_instance().find_parser(input_format);		
	else
		parser =
			ParsersRepo::get_instance().find_suitable_parser(url);
	

	if (!parser) {
		unknown_input_format(input_format);
		exit(EXIT_FAILURE);
	}

	return parser;
}

int makedict::convert(const char *arg)
{
	if (!arg)
		return EXIT_FAILURE;

	std::string basename(arg);
	std::string::size_type pos = basename.rfind(G_DIR_SEPARATOR);
	std::string dirname;
	if (pos != std::string::npos) {
		dirname.assign(basename, 0, pos);
	} else
		dirname=".";

	std::string cur_workdir;

	if (workdir.empty())
		cur_workdir=dirname;
	else
		cur_workdir=workdir;
	const char *icodec = find_input_codec(arg);
	if (icodec) {
		std::string input_cmd = std::string("'") + icodec + "' '" +
			arg + "'" + parser_options;
		std::string output_cmd = "'" + output_codecs[output_format] +
			"' --work-dir '"+cur_workdir+"'";

		std::string cmd = input_cmd + " | " + output_cmd;

		int status = system(cmd.c_str());

		if (status==-1) {
			std::cerr << cmd << _("failed: ")
				  << strerror(errno) << std::endl;
			return EXIT_FAILURE;
		}

		if (WEXITSTATUS(status) == EXIT_FAILURE)
			return EXIT_FAILURE;
	} else {
		ParserBase *parser = find_parser(arg);
		std::vector<char *> output_args;
		gchar *output_codec = g_strdup(output_codecs[output_format].c_str());
		output_args.push_back(output_codec);
		output_args.push_back("--work-dir");
		gchar *cur_dir = g_strdup(cur_workdir.c_str());
		output_args.push_back(cur_dir);
		output_args.push_back(NULL);
		GError *err = NULL;
		GPid codec_pid;
		gint codec_input;
		if (!g_spawn_async_with_pipes(NULL, &output_args[0], NULL,
					      GSpawnFlags(G_SPAWN_DO_NOT_REAP_CHILD),
					      NULL, NULL, &codec_pid, &codec_input,
					      NULL, NULL, &err)) {
			StdErr.printf(_("Execution of %s failed: %s\n"),
				      output_codec, err->message);
			g_error_free(err);
			return EXIT_FAILURE;
		}
		File codec_pipe(fdopen(codec_input, "w"));
		if (!codec_pipe) {
			StdErr.printf(_("Can not open pipe to %s\n"), output_codec);
			return EXIT_FAILURE;
		}
		parser->reset_ops(new PipeParserDictOps(codec_pipe));

		if (parser->run(arg) != EXIT_SUCCESS)
			return EXIT_FAILURE;
		codec_pipe.close();
		int status;
		if (waitpid(codec_pid, &status, 0) == -1) {
			StdErr.printf(_("Can get status of codec: %s\n"), strerror(errno));
			return EXIT_FAILURE;
		}
		if (WEXITSTATUS(status) == EXIT_FAILURE) {
			StdErr.printf(_("Output codec failed\n"));
			return EXIT_FAILURE;
		}
	}

	return EXIT_SUCCESS;
}

int main(int argc, char *argv[])
{
	return makedict().run(argc, argv);
}
