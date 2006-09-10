#ifndef CHARSET_CONV_HPP
#define CHARSET_CONV_HPP

#include <iconv.h>
#include <iostream>
#include <string>

//wrapper for iconv usage
class CharsetConv {
public:
  CharsetConv() { cd=iconv_t(-1); }
  void workwith(const char *from, const char *to) 
  { 
    if (cd!=iconv_t(-1))
      iconv_close(cd);
    if ((cd=iconv_open(to, from))==iconv_t(-1))
			std::cerr<<strerror(errno)<<std::endl;
  }
  CharsetConv(const char *from, const char *to) { cd=iconv_t(-1); workwith(from, to); }
  ~CharsetConv() { if (cd!=iconv_t(-1)) iconv_close(cd); }
  bool convert(const std::string& str, std::string& res) const;
private:
  iconv_t cd;
};

#endif//!CHARSET_CONV_HPP
