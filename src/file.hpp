#ifndef _FILE_HPP_
#define _FILE_HPP_

#include <cstdio>
#include <string>

class File {
public:
	File(FILE *stream, bool close_on_exit = true) : 
		stream_(stream), close_on_exit_(close_on_exit) {}
	void close() {
		if (stream_ && close_on_exit_) {
			fclose(stream_);
			stream_ = NULL;
		}
	}
	~File() { 
		close();
	}
	File& operator<<(const std::string&);
	File& operator<<(const char *str);
	void flush();
	File& printf(const char *fmt, ...);
	bool operator!() const { return !stream_ || ferror(stream_); }
private:
	struct Tester {
		Tester() {}
        private:
		void operator delete(void*);
        };
public:
	// enable 'if (sp)'
        operator Tester*() const
        {
            if (!*this) return 0;
            static Tester t;
            return &t;
        }
private:
	FILE *stream_;
	bool close_on_exit_;
};

extern File StdOut;
extern File StdErr;

#endif//!_FILE_HPP_
