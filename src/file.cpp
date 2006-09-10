/*
 * This file part of makedict - convertor from any dictionary format to any
 * http://xdxf.sourceforge.net
 * Copyright (C) 2006 Evgeniy <dushistov@mail.ru>
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

#include <cstdarg>
#include <glib/gi18n.h>

#include "file.hpp"

File StdOut(stdout, false);
File StdErr(stderr, false);

File& File::operator<<(const char *str)
{
	if (stream_)
		fprintf(stream_, str);
	return *this;
}

File& File::operator<<(const std::string& str)
{
	if (stream_)
		fprintf(stream_, str.c_str());
	return *this;
}

void File::flush()
{
	if (stream_)
		fflush(stream_);
}

File& File::printf(const char *fmt, ...)
{	
	if (stream_) {
		va_list ap;
		va_start(ap, fmt);
		vfprintf(stream_, fmt, ap);
		va_end(ap);
	}
	return *this;
}

bool File::copy(File& in, File& out)
{
	char buf[1024];
	size_t readb;

	do {
		readb = fread(buf, 1, sizeof(buf), in.stream_);
		if (ferror(in.stream_)) {
			StdErr.printf(_("I/O error: can not read from stream\n"));
			return false;
		}
		if (fwrite(buf, 1, readb, out.stream_) != readb) {
			StdErr.printf(_("I/O error: can not write to stream\n"));
			return false;
		}
	} while (readb == sizeof(buf));

	return true;
}
