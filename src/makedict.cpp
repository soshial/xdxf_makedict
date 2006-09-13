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
#include <getopt.h>
#include <glib.h>
#include <glib/gi18n.h>
#include <map>
#include <string>

#include "utils.hpp"
#include "parser.hpp"
#include "process.hpp"
#include "generator.hpp"
#include "connector.hpp"

//#define DEBUG


class MakeDict {
public:
	int run(int argc, char *argv[]);
private:
	StringMap input_codecs;
	StringMap output_codecs;
	StringList parser_options_;
	std::string workdir;
	std::string input_format, output_format;
	std::string appname_;

	int convert(const char *arg);
	const char *find_input_codec(const std::string& url);
	const char *find_output_codec(const std::string& format);
	ParserBase *create_parser(const std::string& url);
	bool fill_codecs_table(const std::string& prgname, const std::string& dirname="");
	void list_codecs();
	void unknown_input_format(const std::string& format = "");
	void unknown_output_format(const std::string& format = "");
	std::string parser_options();
};

static long width_of_first(const StringMap &sm);

std::string MakeDict::parser_options()
{
	std::string res;
	for (StringList::const_iterator it = parser_options_.begin();
	     it != parser_options_.end(); ++it)
		res += std::string(" ") + "--parser-option \"" + *it +"\"";
	return res;
}

void MakeDict::list_codecs()
{
	long w = width_of_first(input_codecs);
	StdOut << _("Input formats:\n");
	for (StringMap::const_iterator it = input_codecs.begin();
	     it != input_codecs.end(); ++it)
		StdOut.printf(_("%s  input codec: %s\n"), it->first.c_str(),
			      it->second.c_str());

	StringList myformats = ParsersRepo::get_instance().supported_formats();
	for (StringList::const_iterator it = myformats.begin();
	     it != myformats.end(); ++it)
		StdOut.printf(_("%s  supported by me\n"), it->c_str());

	w = width_of_first(output_codecs);
	StdOut << _("Output formats:\n");
	for (StringMap::const_iterator it = output_codecs.begin();
	     it != output_codecs.end(); ++it)
		StdOut.printf(_("%s  output codec: %s\n"), it->first.c_str(),
			      it->second.c_str());

	myformats = GeneratorsRepo::get_instance().supported_formats();
	for (StringList::const_iterator it = myformats.begin();
	     it != myformats.end(); ++it)
		StdOut.printf(_("%s  supported by me\n"), it->c_str());
}


int MakeDict::run(int argc, char *argv[])
{
	appname_ = argv[0];
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
			if (i == input_codecs.end() &&
			    !ParsersRepo::get_instance().supported_format(optarg)) {
				unknown_input_format(optarg);
				return EXIT_FAILURE;
			}
			input_format = optarg;
		}
		break;
		case 'o':
		{
			StringMap::iterator i = output_codecs.find(optarg);
			if (i == output_codecs.end() &&
			    !GeneratorsRepo::get_instance().supported_format(optarg)) {
				unknown_output_format(optarg);
				return EXIT_FAILURE;
			}
			output_format = optarg;
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
			parser_options_.push_back(optarg);
			break;
		case '?':
		default:
			StdErr.printf(_("Unknwon option.\nTry '%s --help' for more information.\n"),
				      argv[0]);
			return EXIT_FAILURE;
		}

	if (l) {
		list_codecs();
		return EXIT_SUCCESS;
	}

	if (v) {
		StdOut.printf(_("Utiltity for creating dictionaries, %s"), VERSION);
		return EXIT_SUCCESS;
	}

	if (h || optind >= argc) {
		StdOut.printf(_("Usage: %s [OPTIONS] file1 file2...\n"
				"-v, --version                print version information and exit\n"
				"-h, --help                   display this help and exit\n"
				"-i, --input-format           format of input file\n"
				"-o, --output-format          format of output file\n"
				"--work-dir                   root directory for all created dictionaries\n"
				"-l, --list-supported-formats list all supported formats\n"
				"--parser-option \"option_name=option_value\"\n"), argv[0]);
		return EXIT_SUCCESS;
	}



	for (int i = optind; i < argc; ++i)
		if (convert(argv[i]) != EXIT_SUCCESS)
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

bool MakeDict::fill_codecs_table(const std::string& prgname, const std::string& dirname_)
{
	glib::CharStr tmp_dirname(g_path_get_dirname(prgname.c_str()));
	std::string dirname(dirname_.empty() ? get_impl(tmp_dirname) : dirname_);

	glib::Error err;
	typedef ResourceWrapper<GDir, GDir, g_dir_close> MyGDir;

	MyGDir dir(g_dir_open(dirname.c_str(), 0, get_addr(err)));
	if (!dir) {
		StdErr.printf(_("Can not read %s: %s\n"), dirname.c_str(), err->message);
		return false;
	}

	glib::CharStr th(g_path_get_basename(prgname.c_str()));
	const gchar *filename;
	while ((filename = g_dir_read_name(get_impl(dir))) != NULL) {
		std::string realname = dirname + G_DIR_SEPARATOR + filename;
		glib::CharStr cur(g_path_get_basename(realname.c_str()));
		if (strcmp(get_impl(cur), get_impl(th)) == 0)
			continue;

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

	return true;
}

static long width_of_first(const StringMap &sm)
{
	long res=0;
	for (StringMap::const_iterator it=sm.begin(); it!=sm.end(); ++it)
		if (res<long(it->first.length()))
			res=it->first.length();
	return res;
}

void MakeDict::unknown_output_format(const std::string& format)
{
	if (!format.empty())
		StdErr.printf(_("Unknown output format: %s\n"), format.c_str());

	StdErr << _("Possible output formats are:\n");

	for (StringMap::const_iterator i=output_codecs.begin(); i!=output_codecs.end(); ++i)
		StdErr << i->first << "\n";

	StringList myformats = GeneratorsRepo::get_instance().supported_formats();
	for (StringList::const_iterator it = myformats.begin();
	     it != myformats.end(); ++it)
		StdErr << *it << "\n";
}


void MakeDict::unknown_input_format(const std::string& format)
{
	if (!format.empty())
		StdErr.printf(_("Unknown input format: %s\n"), format.c_str());

	StdErr << _("Possible input formats are:\n");
	for (StringMap::const_iterator i = input_codecs.begin();
	     i != input_codecs.end(); ++i)
		StdErr << i->first << "\n";

	StringList myformats = ParsersRepo::get_instance().supported_formats();
	for (StringList::const_iterator it = myformats.begin();
	     it != myformats.end(); ++it)
		StdErr << *it << "\n";
}

const char *MakeDict::find_input_codec(const std::string &url)
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
				StdErr.printf(_("`system' function failed: %s\n"),
					      strerror(errno));
				exit(EXIT_FAILURE);
			}
			if (WEXITSTATUS(status) == EXIT_SUCCESS)
				return it->second.c_str();
		}
	}

	return NULL;
}

const char *MakeDict::find_output_codec(const std::string& format)
{
	StringMap::const_iterator it = output_codecs.find(format);
	if (it != output_codecs.end())
		return it->second.c_str();
	return NULL;
}

ParserBase *MakeDict::create_parser(const std::string& url)
{
	ParserBase *parser = NULL;

	if (!input_format.empty())
		parser =
			ParsersRepo::get_instance().create_codec(input_format);
	else
		parser =
			ParsersRepo::get_instance().create_suitable_parser(url);


	if (!parser) {
		unknown_input_format(input_format);
		exit(EXIT_FAILURE);
	}

	return parser;
}

int MakeDict::convert(const char *arg)
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
	const char *ocodec = find_output_codec(output_format);
	if (icodec && ocodec) {
		std::string input_cmd = std::string("'") + icodec + "' '" +
			arg + "'" + parser_options();

		std::string output_cmd = std::string("'") + ocodec +
			"' --work-dir '"+cur_workdir+"'";

		Process input_codec, output_codec;
		if (!input_codec.run_async(input_cmd, Process::OPEN_PIPE_FOR_READ) ||
		    !output_codec.run_async(output_cmd, Process::OPEN_PIPE_FOR_WRITE))
			return EXIT_FAILURE;
		if (!File::copy(input_codec.output(), output_codec.input()))
			return EXIT_FAILURE;
		int res;
		if (!output_codec.wait(res))
			return EXIT_FAILURE;

		if (res == EXIT_FAILURE) {
			StdErr.printf(_("Output codec failed\n"));
			return EXIT_FAILURE;
		}

		if (!input_codec.wait(res))
			return EXIT_FAILURE;

		if (res == EXIT_FAILURE) {
			StdErr.printf(_("Input codec failed\n"));
			return EXIT_FAILURE;
		}
	} else if (ocodec) {
		std::auto_ptr<ParserBase> parser(create_parser(arg));
		g_assert(parser.get());
		std::string output_cmd = std::string("'") + ocodec +
			"' --work-dir '"+cur_workdir+"'";

		Process output_codec;
		if (!output_codec.run_async(output_cmd, Process::OPEN_PIPE_FOR_WRITE))
			return EXIT_FAILURE;
		PipeParserDictOps pipe_ops(output_codec.input());
		parser->reset_ops(&pipe_ops);

		if (parser->run(parser_options_, arg) != EXIT_SUCCESS)
			return EXIT_FAILURE;
		int res;
		if (!output_codec.wait(res))
			return EXIT_FAILURE;

		if (res == EXIT_FAILURE) {
			StdErr.printf(_("Output codec failed\n"));
			return EXIT_FAILURE;
		}

	} else if (icodec) {
		std::string input_cmd = std::string("'") + icodec + "' '" +
			arg + "'" + parser_options();

		Process input_codec;
		if (!input_codec.run_async(input_cmd, Process::OPEN_PIPE_FOR_READ))
			return EXIT_FAILURE;
		std::auto_ptr<GeneratorBase> generator(
			GeneratorsRepo::get_instance().create_codec(output_format));
		g_assert(generator.get());
		GeneratorDictPipeOps gen_dict_ops(input_codec.output(), *generator);
		generator->reset_ops(&gen_dict_ops);

		if (generator->run(appname_, cur_workdir) != EXIT_SUCCESS)
			return EXIT_FAILURE;
		int res;

		if (!input_codec.wait(res))
			return EXIT_FAILURE;

		if (res == EXIT_FAILURE) {
			StdErr.printf(_("Input codec failed\n"));
			return EXIT_FAILURE;
		}
	} else {
		std::auto_ptr<ParserBase> parser(create_parser(arg));
		g_assert(parser.get());
		std::auto_ptr<GeneratorBase> generator(
			GeneratorsRepo::get_instance().create_codec(output_format));
		g_assert(generator.get());
		Connector connector(*generator, cur_workdir);
		parser->reset_ops(&connector);
		generator->reset_ops(&connector);
		if (parser->run(parser_options_, arg) != EXIT_SUCCESS)
			return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

int main(int argc, char *argv[])
{
	return MakeDict().run(argc, argv);	
}
