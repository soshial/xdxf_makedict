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
	//To fix compiler warning
	if (g_file_test(file.c_str(), G_FILE_TEST_EXISTS))
		return true;
	return false;
}
typedef std::vector<std::string> StringList;
extern StringList split(const std::string& str, char sep);
extern void strip(std::string& str);
extern const char *b64_encode(guint32 val);
extern guint32 b64_decode(const char *val);

//\! todo move this to separate header file, like copmiler.h
#if defined(__GNUC__) && defined(__GNUC_MINOR__)
#  define GNUC_PREREQ(maj,min) \
	((__GNUC__ << 16) + __GNUC_MINOR__ >= ((maj) << 16) + (min))
#else
#  define GNUC_PREREQ(maj,min) 0
#endif

/* Does this compiler support unused result checking? */
#if GNUC_PREREQ(3,4)
#  define ATTRIBUTE_WARN_UNUSED_RESULT __attribute__ ((__warn_unused_result__))
#else
#  define ATTRIBUTE_WARN_UNUSED_RESULT /**/
#endif

extern bool copy_file(const std::string& from, const std::string& to)
	ATTRIBUTE_WARN_UNUSED_RESULT;

#endif//!_UTILS_HPP_
