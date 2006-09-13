CODECS = \
	stardict_generator xdxf_generator dictd_generator \
	sdict_parser dictd_parser dsl_parser xdxf_parser

bin_PROGRAMS = makedict

noinst_PROGRAMS = $(CODECS)

SCRIPT_CODECS = mueller7_parser.py mdparser.py apresyan.py
noinst_PYTHON = $(SCRIPT_CODECS)

PARSER_LIB = parser.cpp parser.hpp utils.cpp utils.hpp file.cpp file.hpp
GENERATOR_LIB = generator.cpp generator.hpp utils.cpp utils.hpp file.cpp file.hpp

makedict_SOURCES = \
	makedict.cpp \
	process.cpp process.hpp \
	repository.hpp \
	dummy_parser.cpp \
	dummy_generator.cpp \
	utils.cpp utils.hpp \
	parser.cpp parser.hpp \
	generator.cpp generator.hpp \
	file.cpp file.hpp \
	connector.cpp connector.hpp

stardict_generator_SOURCES = stardict_generator.cpp $(GENERATOR_LIB)
xdxf_generator_SOURCES = xdxf_generator.cpp $(GENERATOR_LIB)
dictd_generator_SOURCES = dictd_generator.cpp $(GENERATOR_LIB)
xdxf_parser_SOURCES = xdxf_parser.cpp $(PARSER_LIB)

sdict_parser_SOURCES = \
	sdict_parser.cpp \
	normalize_tags.cpp normalize_tags.hpp \
	$(PARSER_LIB) \
	langs_2to3.cpp langs_2to3.hpp

dictd_parser_SOURCES = \
	dictd_parser.cpp \
	mapfile.hpp \
	charset_conv.hpp charset_conv.cpp \
	$(PARSER_LIB)

dsl_parser_SOURCES = \
	dsl_parser.cpp \
	charset_conv.hpp charset_conv.cpp \
	normalize_tags.cpp normalize_tags.hpp \
	mapfile.hpp \
	$(PARSER_LIB)

LDADD = @MAKEDICT_LIBS@ 
#makedict_LDADD = @MAKEDICT_LIBS@ 
#localedir = $(datadir)/locale

INCLUDES = @MAKEDICT_CFLAGS@  -I$(top_builddir) -DCODECSDIR=\""$(bindir)/makedict-codecs"\"
#DEFS=-DLOCALEDIR=\"$(localedir)\" @DEFS@

install-exec-local:
	$(INSTALL_PROGRAM) -d $(bindir)/makedict-codecs/
	for util in $(CODECS); do \
		$(INSTALL_PROGRAM) $$util $(bindir)/makedict-codecs/ ; \
	done
	for util in $(SCRIPT_CODECS); do \
		$(INSTALL_PROGRAM) $$util $(bindir)/makedict-codecs/ ; \
	done
