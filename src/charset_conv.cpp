/*
 * This file is part of makedict - convertor from any
 * dictionary format to any http://xdxf.sourceforge.net
 *
 * Copyright (C) Evgeniy Dushistov, 2005-2006
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
#include <cstring>
#include <glib/gi18n.h>

#include "file.hpp"

#include "charset_conv.hpp"

//#define DEBUG

void CharsetConv::workwith(const char *from, const char *to)
{
	close();

	if ((cd_ = iconv_open(to, from)) == iconv_t(-1))
		StdErr.printf(_("Can not convert from %s to %s: %s\n"),
			      from, to, strerror(errno));
#ifdef DEBUG
	StdErr.printf("CharsetConv::workwith: %s to %s\n", from ,to);
#endif
}

bool CharsetConv::convert(const char *str, std::string::size_type len,
			  std::string& res) const
{
	res = "";
	if (cd_ == iconv_t(-1)) {
		StdErr << _("Can not convert from one encoding to another:"
			    " wrong iconv descriptor\n");
		return false;
	}

	size_t err;

	res.resize(len);
	const char *p = str;
	size_t inbytesleft = len;
	size_t outbytesleft = res.length();
//TODO: res may be not contiguous
	char *outp = &res[0];
again:
	err = iconv(cd_,
//this need because win32 version of iconv and from glibc is different
#ifdef WIN32
		    (const char **)&p,
#else
		    (char **)&p,
#endif
		    &inbytesleft, &outp, &outbytesleft);
	if (err == size_t(-1))
		if (errno == E2BIG) {
			size_t used = outp - &(res[0]);
			res.resize(res.length() * 2);
			outp = &res[0] + used;
			outbytesleft = res.length() - used;
			goto again;
		} else {
			StdErr.printf(_("Can not convert from one encoding to another: %s\n"),
				      strerror(errno));
			return false;
		}

	res.resize(outp - &res[0]);
	return true;
}
