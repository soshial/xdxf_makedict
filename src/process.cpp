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

#ifdef _WIN32
#  include <windows.h>
#else
extern "C" {
#  include <sys/types.h>
#  include <sys/wait.h>
}
#endif
#include <cerrno>
#include <glib.h>
#include <glib/gi18n.h>

#include "utils.hpp"
#include "resource.hpp"

#include "process.hpp"

typedef ResourceWrapper<gchar *, gchar *, g_strfreev> GStrArray;

bool Process::run_async(const std::string& cmd, int flags)
{
	GStrArray argv;
	glib::Error err;
	gint argc;
	if (!g_shell_parse_argv(cmd.c_str(), &argc, get_addr(argv),
				get_addr(err))) {
		StdErr.printf(_("Can not parser %s: %s"),
			      cmd.c_str(), err->message);
		return false;
	}
	gint infd, outfd, *in = NULL, *out = NULL;
	if (flags & OPEN_PIPE_FOR_WRITE)
		in = &infd;

	if (flags & OPEN_PIPE_FOR_READ)
		out = &outfd;
	int spawn_flags = G_SPAWN_DO_NOT_REAP_CHILD;
	if (flags & INHERIT_STDIN)
		spawn_flags |= G_SPAWN_CHILD_INHERITS_STDIN;

	if (!g_spawn_async_with_pipes(NULL, get_impl(argv), NULL,
				      GSpawnFlags(spawn_flags),
				      NULL, NULL, &pid_, in, out,
				      NULL, get_addr(err))) {
		StdErr.printf(_("Can not execute %s: %s\n"),
			      cmd.c_str(), err->message);
		return false;
	}
	if (flags & OPEN_PIPE_FOR_WRITE) {
		input_.reset(fdopen(infd, "w"));
		if (!input_) {
			StdErr.printf(_("Can not open pipe to %s\n"),
				      cmd.c_str());
			return false;	
		}
	}

	if (flags & OPEN_PIPE_FOR_READ) {
		output_.reset(fdopen(outfd, "r"));
		if (!output_) {
			StdErr.printf(_("Can not open pipe to %s\n"),
				      cmd.c_str());
			return false;	
		}
	}

	return true;
}

bool Process::wait(int &res)
{
#ifdef _WIN32
	input_.close();
	DWORD status;
	if (WaitForSingleObject((HANDLE)pid_, INFINITE) !=
		WAIT_OBJECT_0 ||
		!GetExitCodeProcess((HANDLE)pid_, &status)) {
			g_warning(_("Can not get status of spawned process\n"));
			return false;
	}
	res = status;	
	g_spawn_close_pid(pid_);	
#else
	input_.close();
	int status;
	if (waitpid(pid_, &status, 0) == -1) {
		StdErr.printf(_("Can get status of process: %s\n"),
			      strerror(errno));
		return false;
	}
	res = WEXITSTATUS(status);
	return true;	
#endif
}
