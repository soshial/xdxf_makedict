SUBDIRS = sample-dicts dtd

noinst_PROGRAMS = validate_stardict normalize

validate_stardict_SOURCES = validate_stardict.cpp

normalize_SOURCES = normalize.cpp \
 $(top_builddir)/src/normalize_tags.cpp \
 $(top_builddir)/src/normalize_tags.hpp

LDADD = @MAKEDICT_LIBS@
INCLUDES = @MAKEDICT_CFLAGS@  -I$(top_builddir) -I$(top_builddir)/src

SH_TESTS = \
	t_dummy t_stardict t_normalize \
	t_generator t_dslparser t_xdxfgenerator \
	t_parser_error.sh t_parser_options.sh
EXTRA_DIST=$(SH_TESTS)
TESTS=$(SH_TESTS)
