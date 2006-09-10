#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif

#include <cstdlib>
#include <iostream>

#include "normalize_tags.hpp"

using std::string;
using std::cin;
using std::cout;
using std::cerr;
using std::endl;

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
	if (argc!=2) {
		cerr<<"Usage: "<<argv[0]<<" line"<<endl;
		return EXIT_FAILURE;
	}

  TagInfoList taginfo_list;

	string open_tag, close_tag;
	int i=1;
	while (getline(cin, open_tag) && getline(cin, close_tag)) {
		taginfo_list.push_back(TagInfo(open_tag, close_tag, open_tag, close_tag, i));
		++i;
	}
  

	cout<<test_normalize(argv[1], taginfo_list)<<endl;

 
  return EXIT_SUCCESS;
}
