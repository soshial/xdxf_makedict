#ifndef _UTILS_HPP_
#define _UTILS_HPP_

#include <algorithm>
#include <cstring>
#include <glib.h>
#include <map>
#include <string>
#include <vector>

struct less_str {
  bool operator()(const char *lh, const char *rh) const {
    return strcmp(lh, rh)<0;
  }
};
typedef std::map<const char*, const char *, less_str> Str2StrTable;

extern void replace(const Str2StrTable& replace_table,
		    const char *str, std::string& res);

extern bool make_directory(const std::string& dir);

static inline void tolower(std::string& str)
{
	std::transform(str.begin(), str.end(), str.begin(), g_ascii_tolower);
}

static inline bool is_file_exist(const std::string& file)
{
	return g_file_test(file.c_str(), G_FILE_TEST_EXISTS);
}

extern std::vector<std::string> split(const std::string& str, char sep);
extern void strip(std::string& str);
extern const char *b64_encode(guint32 val);
extern guint32 b64_decode(const char *val);
extern void copy_file(const std::string& from, const std::string& to);

#endif//!_UTILS_HPP_
