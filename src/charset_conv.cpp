/*
 * This file is part of makedict - convertor from any
 * dictionary format to any http://sdcv.sourceforge.net
 *
 * Copyright (C) Evgeniy Dushistov, 2005
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Library General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif

#include <cerrno>
#include <glib/gi18n.h>

#include "charset_conv.hpp"

using std::string;

bool CharsetConv::convert(const string& str, string& res) const
{
  if (cd!=iconv_t(-1)) {
    size_t err;
    res="";
    res.resize(str.length());
    const char *p=&str[0];
    size_t inbytesleft=str.length();
    size_t outbytesleft=res.length();
    char *outp=&res[0];
  again:
    err=iconv(cd,/*this need because win32 version of iconv and from glibc is different*/
#ifdef WIN32	
							(const char **)&p, 
#else
							(char **)&p, 
#endif
							&inbytesleft, &outp, &outbytesleft);
    if (err==size_t(-1))
      if (errno==E2BIG) {
				size_t used = outp - &(res[0]);
				res.resize(res.length()*2);
				outp = &(res[0]) + used;
				outbytesleft = res.length() - used;
				goto again;
      } else {
				std::cerr<<_("Can not convert from one encoding to another: ")<<strerror(errno)<<std::endl;
				return false;
      }
    size_t used=outp-&(res[0]);
    res.resize(used);
    return true;
  } else {
		std::cerr<<_("Can not convert")<<std::endl;
    return false;
  }
	
  return true;
}
