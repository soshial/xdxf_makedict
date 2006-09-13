#ifndef _FILE_HPP_
#define _FILE_HPP_

#include <cstdio>
#include <string>

class File {
public:
	File(FILE *stream = NULL, bool close_on_exit = true) : 
		stream_(stream), close_on_exit_(close_on_exit) {}
	void close() {
		if (stream_ && close_on_exit_) {
			fclose(stream_);
			stream_ = NULL;
		}
	}
	File& reset(FILE *stream) {
		close();
		stream_ = stream;
		return *this;
	}
	~File() { 
		close();
	}
	File& operator<<(const std::string&);
	File& operator<<(const char *str);
	void flush();
	File& printf(const char *fmt, ...);
	bool operator!() const { return !stream_ || feof(stream_) || ferror(stream_); }
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
	static bool copy(File& in, File& out);
	static File& getline(File& in, std::string& line);
	File& write(const char *buf, size_t len);
private:
	FILE *stream_;
	bool close_on_exit_;
};

class Strip {
public:
	Strip(const std::string& str) : str_(str) {}
	friend File& operator<<(File& out, const Strip& st);
private:
	const std::string& str_;
};

extern File StdIn;
extern File StdOut;
extern File StdErr;

#endif//!_FILE_HPP_
