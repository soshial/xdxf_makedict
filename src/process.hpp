#ifndef __PROCESS_HPP__
#define __PROCESS_HPP__

#include <glib.h>
#include <string>

#include "file.hpp"

class Process {
public:
	enum {
		OPEN_PIPE_FOR_READ = 1 << 0,
		OPEN_PIPE_FOR_WRITE = 1 << 1,
		INHERIT_STDIN = 1 << 2
	};
	bool run_async(const std::string& cmd, int flags);
	bool wait(int &res);
	File& input() { return input_; }
	File& output() { return output_; }
	static bool run_cmd_line_sync(const std::string& cmd,
				      std::string& output);
private:
	File input_, output_;
	GPid pid_;
};

#endif//!__PROCESS_HPP__
