SUBDIRS = sample-dicts dtd

noinst_PROGRAMS = validate_stardict normalize t_xml

validate_stardict_SOURCES = validate_stardict.cpp
t_xml_SOURCES = \
	t_xml.cpp \
	$(top_builddir)/src/xml.cpp $(top_builddir)/src/xml.hpp

normalize_SOURCES = normalize.cpp \
 $(top_builddir)/src/normalize_tags.cpp \
 $(top_builddir)/src/normalize_tags.hpp

LDADD = @MAKEDICT_LIBS@
INCLUDES = @MAKEDICT_CFLAGS@  -I$(top_builddir) -I$(top_builddir)/src

SH_TESTS = \
	t_dummy t_stardict t_normalize \
	t_generator t_dslparser t_xdxfgenerator \
	t_parser_error.sh t_parser_options.sh \
	t_sdict_parser

EXTRA_DIST=$(SH_TESTS)
TESTS = $(SH_TESTS) t_xml
