#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif

#include <cstdlib>
#include <iostream>
#include <utility>

#include "normalize_tags.hpp"

using std::string;
using std::cin;
using std::cout;
using std::cerr;
using std::endl;

typedef std::pair<const char *, const char *> InOut;

static string test_normalize(const char *str, TagInfoList &taginfo_list)
{
	string resstr;
	NormalizeTags norm_tags(taginfo_list);
	const char *p=str;
	while (*p) {
		if (*p=='<') {
			if (*(p+1)!='/') {
				if (!norm_tags.add_open_tag(resstr, p))
					goto end_of_handle;
			} else {
				if (!norm_tags.add_close_tag(resstr, p))
					goto end_of_handle;
			}
		} else {
		end_of_handle:
			resstr+=*p++;
		}
	}
	if (resstr[resstr.length()-1]=='\n')
		resstr.erase(resstr.begin()+resstr.length()-1);
	string datastr;
	norm_tags(resstr, datastr);
	return datastr;
}

int main(int argc, char *argv[])
{
#if 0
	if (argc!=2) {
		cerr << "Usage: " << argv[0] << " line" << endl;
		return EXIT_FAILURE;
	}
#endif
	const char *tags_list[] = {
		"<b>",
		"</b>",
		"<i>",
		"</i>",
		"<c>",
		"</c>",
		"<abr>",
		"</abr>",
		NULL,
	};

	TagInfoList taginfo_list;

	string open_tag, close_tag;
	int i = 1;
#if 1
	const char **p = tags_list;
	while (*p) {
		open_tag = *p++;
		close_tag = *p++;
		taginfo_list.push_back(TagInfo(open_tag, close_tag,
					       open_tag, close_tag, i));
		++i;
	}
#else
	while (getline(cin, open_tag) && getline(cin, close_tag)) {
		taginfo_list.push_back(TagInfo(open_tag, close_tag, open_tag, close_tag, i));
		++i;
	}
	cout << test_normalize(argv[1], taginfo_list) << endl;
#endif

	const InOut test_data[] = {
		InOut("get <b> <i>done</b></i>", "get <b> <i>done</i></b>"),
		InOut("AAA <i> BBB", "AAA <i> BBB</i>"),
		InOut("AAA <i> AAA <c></i> DDD</c>",
		      "AAA <i> AAA </i><c> DDD</c>"),
		InOut("<i><c>AAA</c></i>", "<i><c>AAA</c></i>"),
		InOut("<abr><i><c>AAA</c></i></abr>",
		      "<abr><i><c>AAA</c></i></abr>"),
		InOut(NULL, NULL),
	};
	const InOut *te = &test_data[0];
	while (te->first) {
		std::string res = test_normalize(te->first, taginfo_list);
		if (res != te->second) {
			std::cerr << "expect " << "|" << te->second << "|" <<
				"got: " << "|" << res << "|" << std::endl;
			return EXIT_FAILURE;
		}
		++te;
	}
	
	return EXIT_SUCCESS;
}
