bin_PROGRAMS = makedict

#noinst_PROGRAMS = $(CODECS)

SCRIPT_CODECS = mueller7_parser.py mdparser.py apresyan.py
noinst_PYTHON = $(SCRIPT_CODECS)

PARSER_LIB = parser.cpp parser.hpp utils.cpp utils.hpp file.cpp file.hpp
GENERATOR_LIB = generator.cpp generator.hpp utils.cpp utils.hpp \
	file.cpp file.hpp xml.cpp xml.hpp

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
	connector.cpp connector.hpp \
	dsl_parser.cpp \
	charset_conv.cpp charset_conv.hpp \
	mapfile.hpp \
	normalize_tags.cpp normalize_tags.hpp \
	sdict_parser.cpp \
	langs_2to3.cpp langs_2to3.hpp \
	xdxf_generator.cpp \
	xml.cpp xml.hpp \
	xdxf_parser.cpp \
	stardict_generator.cpp \
	dictd_generator.cpp \
	dictd_parser.cpp

LDADD = @MAKEDICT_LIBS@ 
#makedict_LDADD = @MAKEDICT_LIBS@ 
#localedir = $(datadir)/locale

INCLUDES = @MAKEDICT_CFLAGS@  -I$(top_builddir) 
#DEFS=-DLOCALEDIR=\"$(localedir)\" @DEFS@
DEFS = -DDUMMY_FORMAT_NAME=\"dummy\" -DCODECSDIR=\""$(bindir)/makedict-codecs"\" @DEFS@ 

install-exec-local:
	$(INSTALL_PROGRAM) -d $(bindir)/makedict-codecs/
	for util in $(CODECS); do \
		$(INSTALL_PROGRAM) $$util $(bindir)/makedict-codecs/ ; \
	done
	for util in $(SCRIPT_CODECS); do \
		$(INSTALL_PROGRAM) $$util $(bindir)/makedict-codecs/ ; \
	done
