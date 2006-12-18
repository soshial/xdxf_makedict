#ifndef MAPFILE_H
#define MAPFILE_H

#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif

#ifdef HAVE_MMAP
#elif defined(WIN32)
# include <windows.h>
#else
# include "resource.hpp"
#endif

/**
 * Wrapper around mmap and its analogs.
 */
class MapFile {
public:
	char *cur;

	MapFile();
	~MapFile();
	bool open(const char *file_name, bool logerr = false,
		  long file_size = -1);
	char *begin(void) { return data; }
	char *end(void) { return end_of_file; }
	char *find_str(char *beg, const char *str, char *end=NULL); 
	operator bool () { return cur < end_of_file; }
	bool eof() { return ! *this; }
private:
	char *data;
	char *end_of_file;
	long size;
#ifdef HAVE_MMAP
	int mmap_fd;
#elif defined(WIN32)
	HANDLE hFile;
	HANDLE hFileMap;
#else
	glib::CharStr databuf_;
#endif

	void close(void);
};

#endif/*MAPFILE_H*/
