SUBDIRS = sample-dicts dtd
DEFS = -DDUMMY_FORMAT_NAME=\"single-dummy\" -DDUMMY_PARSER_ALONE \
	-DDUMMY_GENERATOR_ALONE  @DEFS@

noinst_PROGRAMS = validate_stardict normalize t_xml dummy_parser \
	dummy_generator

validate_stardict_SOURCES = validate_stardict.cpp
t_xml_SOURCES = \
	t_xml.cpp \
	$(top_builddir)/src/xml.cpp $(top_builddir)/src/xml.hpp \
	$(top_builddir)/src/file.cpp $(top_builddir)/src/file.hpp

normalize_SOURCES = t_normalize.cpp \
 $(top_builddir)/src/normalize_tags.cpp \
 $(top_builddir)/src/normalize_tags.hpp

dummy_parser_SOURCES = \
	$(top_builddir)/src/dummy_parser.cpp \
	$(top_builddir)/src/parser.cpp $(top_builddir)/src/parser.hpp \
	$(top_builddir)/src/utils.cpp $(top_builddir)/src/utils.hpp \
	$(top_builddir)/src/file.cpp $(top_builddir)/src/file.hpp \
	$(top_builddir)/src/xml.cpp $(top_builddir)/src/xml.hpp \
	$(top_builddir)/src/log.cpp $(top_builddir)/src/log.hpp

dummy_generator_SOURCES = \
	$(top_builddir)/src/dummy_generator.cpp \
	$(top_builddir)/src/generator.cpp $(top_builddir)/src/generator.hpp \
	$(top_builddir)/src/utils.cpp $(top_builddir)/src/utils.hpp \
	$(top_builddir)/src/file.cpp $(top_builddir)/src/file.hpp \
	$(top_builddir)/src/xml.cpp $(top_builddir)/src/xml.hpp \
	$(top_builddir)/src/log.cpp $(top_builddir)/src/log.hpp

LDADD = @MAKEDICT_LIBS@
INCLUDES = @MAKEDICT_CFLAGS@  -I$(top_builddir) -I$(top_builddir)/src

SH_TESTS = \
	t_dummy t_stardict t_normalize \
	t_generator t_dslparser t_xdxfgenerator \
	t_parser_error.sh t_parser_options.sh \
	t_sdict_parser t_generator_error.sh \
	t_stardict_parser.sh t_perfomance.sh

EXTRA_DIST=$(SH_TESTS) funcs.sh t_convert_all.sh
TESTS = $(SH_TESTS) t_xml
