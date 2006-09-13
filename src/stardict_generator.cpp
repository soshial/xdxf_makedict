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

#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <glib/gi18n.h>
#include <glib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>

#include "utils.hpp"

#include "generator.hpp"

class stardict_generator : public GeneratorBase {
public:
	stardict_generator()
	{
		set_format("stardict");
		set_version(_("stardict_generator, version 0.2"));
		cur_off=0;
		keys_list.reserve(20000);
	}
	~stardict_generator()
	{
		for (std::vector<Key>::iterator p=keys_list.begin();
				 p!=keys_list.end(); ++p)
			delete []p->value;
	}
protected:
	int generate();
	void on_have_data(const StringList&, const std::string&);
	bool on_prepare_generator(const std::string&,
				  const std::string&);
private:
	std::ofstream idx_file_;
	guint32 cur_off;
	std::string full_name;

	class RemoveAfterExit {
	public:
		RemoveAfterExit() {}
		RemoveAfterExit(const std::string& fn) : filename_(fn) {}
		~RemoveAfterExit() {
			if (!filename_.empty())
				remove(filename_.c_str()); 
		}
		void operator() (const std::string& fn) { filename_ = fn; }
	private:
		std::string filename_;
	} remove_after_exit_;
	std::fstream tmp_dict_file_;
	std::ofstream dict_file_;
	std::string realbasename_;

	struct Key {
		char *value;
		guint32 data_off;
		guint32 data_size;
		Key(char *val=NULL, guint32 off=0, guint32 size=0) :
			value(val), data_off(off), data_size(size)
		{
		}
		Key(const std::string& val, guint32 off=0, guint32 size=0) :
			value(NULL), data_off(off), data_size(size)
		{
			value = new char [val.length()+1];
			strcpy(value, val.c_str());
		}
	};
	std::vector<Key> keys_list;
	static gint stardict_strcmp(const gchar *s1, const gchar *s2) {
		gint a = g_ascii_strcasecmp(s1, s2);
		if (a == 0)
			return strcmp(s1, s2);

		return a;
	}
	class stardict_less {
	public:
		bool operator()(const Key & lh, const Key & rh) {
			return stardict_strcmp(lh.value, rh.value)<0;
		}
	};
	bool create_ifo_file(const std::string& basename, guint32 wordcount, guint32 idx_file_size);
	std::string get_current_date(void);
};


bool stardict_generator::on_prepare_generator(const std::string& workdir,
					      const std::string& bname)
{
	std::string basename = "stardict-" + bname + "-2.4.2";
	std::string dirname = workdir + G_DIR_SEPARATOR + basename;

	if (!make_directory(dirname))
		return false;

	realbasename_ = dirname + G_DIR_SEPARATOR + bname;

	std::string file_name = realbasename_ + ".dict.tmp";
	tmp_dict_file_.open(file_name.c_str(),
			    std::ios::binary | std::ios::out |
			    std::ios::trunc | std::ios::in);
	if (!tmp_dict_file_) {
		std::cerr<<_("Can not open/create: ") << file_name << std::endl;
		return false;
	}
	remove_after_exit_(file_name);


	file_name = realbasename_ + ".idx";
	idx_file_.open(file_name.c_str(),
		       std::ios::binary | std::ios::out | std::ios::trunc);
	if (!idx_file_) {
		std::cerr<<_("Can not open/create: ")<<file_name<<std::endl;
		return false;
	}

	file_name = realbasename_ + ".dict";
	dict_file_.open(file_name.c_str(), std::ios::binary |
		   std::ios::out | std::ios::trunc);
	if (!dict_file_) {
		std::cerr<<_("Can not open/create: ")<<file_name<<std::endl;
		return false;
	}

	return true;
}

int stardict_generator::generate()
{
	int res = EXIT_FAILURE;

	std::cout<<_("Sorting...");
	sort(keys_list.begin(), keys_list.end(), stardict_less());
	std::cout<<_("done\n");

	guint32 wordcount=keys_list.size();
	guint32 data_size, data_offset=0;
	guint32 i=0;
	bool duplicates=false;
	std::string encoded_str;
	Key *pitem = &keys_list[i];
	while (i<keys_list.size()) {
		char *prev_item_value=NULL;
		std::vector<char> buf;
		guint32 tmpguint32=0;
		data_size=0;

		for (;;) {
			tmp_dict_file_.seekg(pitem->data_off);
			buf.resize(pitem->data_size+1);
			tmp_dict_file_.read(&buf[0], pitem->data_size);
			buf[pitem->data_size]='\0';

			if (!g_utf8_validate(&buf[0], gssize(-1), NULL)) {
				std::cerr<<_("Not valid UTF-8 std::string: ")<<encoded_str<<std::endl;
				return res;
			}

			if (!dict_file_.write(&buf[0], pitem->data_size)) {
				std::cerr<<_("Can not write in data file\n");
				return res;
			}

			data_size+=pitem->data_size;
			prev_item_value=pitem->value;
			++i;
			pitem = &keys_list[i];
			if (i<keys_list.size() &&
			    strcmp(prev_item_value, keys_list[i].value)==0) {
#if 0
				std::cerr<<_("Duplicate!: ")<<prev_item_value<<std::endl;
#else
				duplicates=true;
#endif
				wordcount--;
				dict_file_.write("\n", 1);
				++data_size;
			} else
				break;
		}

		idx_file_.write(prev_item_value, strlen(prev_item_value)+1);
		tmpguint32 = g_htonl(data_offset);
		idx_file_.write((char *)&tmpguint32, sizeof(guint32));
		tmpguint32 = g_htonl(data_size);
		idx_file_.write((char *)&tmpguint32, sizeof(guint32));
		data_offset+=data_size;
	}
	if (duplicates)
		std::cerr<<_("There are several duplicated keys\n");
	if (!create_ifo_file(realbasename_, wordcount, idx_file_.tellp())) {
		std::cerr<<_("Creating ifo file error.\n");
		return res;
	}
	std::cout<<_("Count of articles: ")<<wordcount<<std::endl;
	res=EXIT_SUCCESS;

	return res;
}

void stardict_generator::on_have_data(const StringList& keys,
				      const std::string& data)
{
	for (StringList::const_iterator p = keys.begin(); p != keys.end(); ++p)
		keys_list.push_back(Key(*p, cur_off, data.length()));
	tmp_dict_file_.write(&data[0], data.length());
	cur_off += data.length();
}

bool
stardict_generator::create_ifo_file(const std::string& basename,
				    guint32 wordcount,
				    guint32 idx_file_size)
{
	std::string filename=basename+".ifo";
	std::ofstream f(filename.c_str(), std::ios::binary|std::ios::out);

	if (!f) {
		std::cerr<<_("Can not create/open: ")<<filename.c_str()<<std::endl;
		return false;
	}

	f<<"StarDict's dict ifo file\n"
	 <<"version=2.4.2\n"
	 <<"wordcount="<<wordcount<<"\n"
	 <<"idxfilesize="<<idx_file_size<<"\n";
	if (!full_name.empty())
		f << "bookname=" << full_name << "\n";
	else
		f << "bookname=" << get_dict_info("full_name") << "\n";

	f<<"date="<<get_current_date()<<"\n"
	 <<"sametypesequence=x\n";


	Str2StrTable newline_tbl;
	newline_tbl["\n"]=" ";
	newline_tbl["\r"]=" ";
	std::string descr;
	replace(newline_tbl, get_dict_info("description").c_str(), descr);
	if (!descr.empty())
		f<<"description="<<descr<<"\n";

	return true;
}

std::string stardict_generator::get_current_date(void)
{
  /* Get the current time. */
  time_t curtime = time(NULL);

  /* Convert it to local time representation. */
  struct tm *loctime = localtime (&curtime);
  gchar buf[256];
  strftime(buf, sizeof(buf), "%Y.%m.%d", loctime);

  return buf;
}

int main(int argc, char *argv[])
{
	return stardict_generator().run(argc, argv);
}
