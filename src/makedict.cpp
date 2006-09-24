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
#include <glib.h>
#include <glib/gi18n.h>

#include <map>
#include <string>

#include "connector.hpp"
#include "generator.hpp"
#include "log.hpp"
#include "process.hpp"
#include "parser.hpp"
#include "resource.hpp"
#include "utils.hpp"

//#define DEBUG


class MakeDict {
public:
	MakeDict() {}
	int run(int argc, char *argv[]);
private:
	StringMap input_codecs;
	StringMap output_codecs;
	StringList parser_options_;
	std::string workdir_;
	std::string input_format_, output_format_;
	Logger logger_;

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

static int width_of_first(const StringMap &, const StringList&);

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
	StringList myformats = ParsersRepo::get_instance().supported_formats();
	int w = width_of_first(input_codecs, myformats);
	StdOut << _("Input formats:\n");
	for (StringMap::const_iterator it = input_codecs.begin();
	     it != input_codecs.end(); ++it)
		StdOut.printf(_("%*s  input codec: %s\n"), w, it->first.c_str(),
			      it->second.c_str());

	for (StringList::const_iterator it = myformats.begin();
	     it != myformats.end(); ++it)
		StdOut.printf(_("%*s  supported by me\n"), w, it->c_str());

	myformats = GeneratorsRepo::get_instance().supported_formats();
	w = width_of_first(output_codecs, myformats);

	StdOut << _("Output formats:\n");
	for (StringMap::const_iterator it = output_codecs.begin();
	     it != output_codecs.end(); ++it)
		StdOut.printf(_("%*s  output codec: %s\n"), w, it->first.c_str(),
			      it->second.c_str());

	for (StringList::const_iterator it = myformats.begin();
	     it != myformats.end(); ++it)
		StdOut.printf(_("%*s  supported by me\n"), w, it->c_str());
}


int MakeDict::run(int argc, char *argv[])
{	
#ifdef HAVE_LOCALE_H
	setlocale(LC_ALL, "");
#endif

	const gchar *dir = g_getenv("MAKEDICT_PLUGIN_DIR");
	if (!fill_codecs_table(argv[0], dir ? dir : CODECSDIR) &&
	    !fill_codecs_table(argv[0], ""))
		return EXIT_FAILURE;

	for (StringMap::const_iterator p = input_codecs.begin();
	     p != input_codecs.end(); ++p)
		g_info(_("Input codec: %s\t%s\n"), p->first.c_str(),
		       p->second.c_str());

	for (StringMap::const_iterator p = output_codecs.begin();
	     p != output_codecs.end(); ++p)
		g_info(_("Output codec: %s\t%s\n"), p->first.c_str(),
		       p->second.c_str());

	gboolean list_fmts = FALSE, show_version = FALSE;
	glib::CharStr input_fmt, output_fmt, work_dir;
	glib::CharStrArr parser_opts;
	gint verbose;

	static GOptionEntry entries[] = {
		{ "version", 'v', 0, G_OPTION_ARG_NONE, &show_version,
		  _("print version information and exit"), NULL },
		{ "list-supported-formats", 'l', 0, G_OPTION_ARG_NONE, &list_fmts,
		  _("list all supported formats"), NULL },
		{ "input-format", 'i', 0, G_OPTION_ARG_STRING, get_addr(input_fmt),
		  _("format of input file"), NULL },
		{ "output-format", 'o', 0, G_OPTION_ARG_STRING, get_addr(output_fmt),
		  _("format of output file"), NULL },
		{ "work-dir", 'd', 0, G_OPTION_ARG_STRING, get_addr(work_dir),
		  _("root directory for all created dictionaries"), NULL },
		{ "parser-option", 0, 0, G_OPTION_ARG_STRING_ARRAY,
		  get_addr(parser_opts), _("\"option_name=option_value\""),
		  NULL },
		{ "verbose", 0, 0, G_OPTION_ARG_INT, &verbose,
		  _("set level of verbosity"), NULL },
		{ NULL },
	};

	glib::OptionContext opt_cnt(g_option_context_new(_("file1 file2...")));
	g_option_context_add_main_entries(get_impl(opt_cnt), entries, NULL);
	g_option_context_set_help_enabled(get_impl(opt_cnt), TRUE);
	glib::Error err;
	if (!g_option_context_parse(get_impl(opt_cnt), &argc, &argv, get_addr(err))) {
		g_warning(_("Options parsing failed: %s\n"), err->message);
		return EXIT_FAILURE;
	}

	logger_.set_verbosity(verbose);

	if (input_fmt) {
		StringMap::const_iterator i = input_codecs.find(get_impl(input_fmt));
		if (i == input_codecs.end() &&
		    !ParsersRepo::get_instance().supported_format(get_impl(input_fmt))) {
			unknown_input_format(get_impl(input_fmt));
			return EXIT_FAILURE;
		}
		input_format_ = get_impl(input_fmt);
	}
	if (output_fmt)	{
		StringMap::iterator i = output_codecs.find(get_impl(output_fmt));
		if (i == output_codecs.end() &&
		    !GeneratorsRepo::get_instance().supported_format(get_impl(output_fmt))) {
			unknown_output_format(get_impl(output_fmt));
			return EXIT_FAILURE;
		}
		output_format_ = get_impl(output_fmt);
	}
	if (show_version) {
		StdOut.printf(_("Utiltity for creating dictionaries, %s"), VERSION);
		return EXIT_SUCCESS;
	}
	if (list_fmts) {
		list_codecs();
		return EXIT_SUCCESS;
	}

	if (work_dir)
		workdir_ = get_impl(work_dir);

	if (parser_opts) {
		gchar **popts = get_impl(parser_opts);
		while (*popts) {
			std::string pars_opt = *popts;
			size_t beg = 0, len = pars_opt.length();
			if (pars_opt[0] == '"' || pars_opt[0] == '\'')
				++beg;
			if (len && (pars_opt[len - 1] == '"' ||
				    pars_opt[len - 1] == '\''))
				--len;
			g_info(_("Parser option: %s\n"),
			       pars_opt.substr(beg, len - beg).c_str());
			parser_options_.push_back(pars_opt.substr(beg, len - beg));
			++popts;
		}
	}


	if (1 == argc) {
		g_warning(_("You do not specify file names of dictionaries\n"
			    "Use %s --help to get more information\n"), 
			  g_get_prgname());
		return EXIT_SUCCESS;
	}



	for (int i = 1; i < argc; ++i)
		if (convert(argv[i]) != EXIT_SUCCESS)
			return EXIT_FAILURE;

	return EXIT_SUCCESS;
}

static bool start_cmd(const std::string& cmd, std::string& res)
{
	gchar *std_error = NULL;
	gchar *std_output = NULL;
	gint exit_status;
	if (g_spawn_command_line_sync(cmd.c_str(), &std_output, &std_error, &exit_status, NULL) &&
	    WEXITSTATUS(exit_status) == EXIT_SUCCESS) {
		size_t len=strlen(std_output);
		while (len>0 && g_ascii_isspace(std_output[len-1]))
			--len;
		std_output[len]='\0';
		res = std_output;
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
			std::string std_output;

			if (start_cmd("'" + realname + "' --input-format",
				      std_output) && !std_output.empty()) {
				input_codecs[std_output] = realname;
				continue;
			}

			if (start_cmd("'" + realname + "' --output-format",
				      std_output) && !std_output.empty()) {
				output_codecs[std_output] = realname;
				continue;
			}

		}
	}

	return true;
}

static int width_of_first(const StringMap &sm, const StringList& sl)
{
	int res = 0;
	for (StringMap::const_iterator it = sm.begin(); it != sm.end(); ++it)
		if (res < int(it->first.length()))
			res = it->first.length();

	for (StringList::const_iterator it = sl.begin(); it != sl.end(); ++it)
		if (res < int(it->length()))
			res = it->length();
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
	if (!input_format_.empty()) {
		StringMap::const_iterator it = input_codecs.find(input_format_);
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

	if (!input_format_.empty())
		parser =
			ParsersRepo::get_instance().create_codec(input_format_);
	else
		parser =
			ParsersRepo::get_instance().create_suitable_parser(url);


	if (!parser) {
		unknown_input_format(input_format_);
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

	if (workdir_.empty())
		cur_workdir = dirname;
	else
		cur_workdir = workdir_;
	const char *icodec = find_input_codec(arg);
	const char *ocodec = find_output_codec(output_format_);
	if (icodec && ocodec) {
		std::string input_cmd = std::string("'") + icodec + "' '" +
			arg + "'" + parser_options();

		std::string output_cmd = std::string("'") + ocodec +
			"' --work-dir '"+cur_workdir+"'";

		Process input_codec, output_codec;
		if (!input_codec.run_async(input_cmd,
					   Process::OPEN_PIPE_FOR_READ |
					   Process::INHERIT_STDIN) ||
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
#ifdef DEBUG
		StdOut << "Run: " << input_cmd << "\n";
#endif
		if (!input_codec.run_async(input_cmd,
					   Process::OPEN_PIPE_FOR_READ |
					   Process::INHERIT_STDIN))
			return EXIT_FAILURE;
		std::auto_ptr<GeneratorBase> generator(
			GeneratorsRepo::get_instance().create_codec(output_format_));
		g_assert(generator.get());
		GeneratorDictPipeOps gen_dict_ops(input_codec.output(), *generator);
		generator->reset_ops(&gen_dict_ops);

		if (generator->run(g_get_prgname(), cur_workdir) != EXIT_SUCCESS)
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
			GeneratorsRepo::get_instance().create_codec(output_format_));
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
