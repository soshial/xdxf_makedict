#ifndef MAPFILE_H
#define MAPFILE_H

#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif

#ifdef HAVE_MMAP
#  include <sys/types.h>
#  include <sys/stat.h>
#  include <fcntl.h>
#  include <sys/mman.h>
#  include <unistd.h>
#elif defined(WIN32)
# include <windows.h>
#else
#  include <fstream>
#  include <vector>
#endif

class MapFile {
public:
	char *cur;

	MapFile();
	~MapFile();
	bool open(const char *file_name, long file_size=-1);
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
	std::vector<char> data_buffer;
#endif

	void close(void);
};

inline MapFile::MapFile() : data(NULL), end_of_file(NULL), size(0)
{
#ifdef HAVE_MMAP
	mmap_fd=-1;
#endif
}

inline void MapFile::close(void)
{
	if (!data)
		return;
#ifdef HAVE_MMAP
	munmap(data, size);
	::close(mmap_fd);
	mmap_fd=-1;
#elif defined(WIN32)
	UnmapViewOfFile(data);
	CloseHandle(hFileMap);
	CloseHandle(hFile);
#endif
	data=NULL;
}

inline bool MapFile::open(const char *file_name, long file_size)
{
	close();
	size=file_size;
#ifdef HAVE_MMAP
	if ((mmap_fd = ::open(file_name, O_RDONLY)) < 0)
		return false;
	
	if (-1==size) {
		struct stat stat_info;
		if (fstat(mmap_fd, &stat_info)==-1)
			return false;
		size=stat_info.st_size;
	}
	data = (char *)mmap(NULL, size, PROT_READ, MAP_SHARED, mmap_fd, 0);
	if ((void *)data == (void *)(-1))
		return false;

#elif defined(WIN32)
	hFile = CreateFile(file_name, GENERIC_READ, 0, NULL, OPEN_ALWAYS,
			   FILE_ATTRIBUTE_NORMAL, 0);
	if (-1==size)
		size=GetFileSize(hFile, NULL);
	hFileMap = CreateFileMapping(hFile, NULL, PAGE_READONLY, 0,
				     size, NULL);
	data = (char *)MapViewOfFile(hFileMap, FILE_MAP_READ, 0, 0, size);
#  else
	std::ifstream f(file_name);
	if (!f)
		return false;
	if (-1==size) {
		if (!f.seekg(0, std::ios::end))
			return false;
		size=f.tellg();
		if (!f.seekg(0))
			return false;
	}
	data_buffer.resize(size);
	if (!f.read(&(data_buffer[0]), data_buffer.size()))
		return false;
	data=&(data_buffer[0]);
#endif
	cur=data;
	end_of_file=data+size;
	return true;
}

inline MapFile::~MapFile()
{
	close();
}

inline char *MapFile::find_str(char *beg, const char *str, char *end)
{
	const char *p;
	if (!end)
		end=MapFile::end();
	while (beg!=end) {
		p=str;
		while (beg!=end && *p && *p==*beg) ++p, ++beg;
		if (*p=='\0')
			return beg+(str-p);
		++beg;
	}
	return end;
}

#endif/*MAPFILE_H*/
