dnl Process this file with autoconf to produce a configure script.
AC_PREREQ(2.52)

AC_INIT(makedict, 0.3beta1, dushistov@mail.ru)
AC_CONFIG_SRCDIR(src/makedict.cpp)

dnl Don't include maintainer make-rules by default
AM_MAINTAINER_MODE

AM_INIT_AUTOMAKE([dist-bzip2])

AM_CONFIG_HEADER(config.h)
AC_PROG_AWK
AC_PROG_CC
AC_PROG_CPP
AC_PROG_CXX
AC_LANG([C++])
AC_PROG_CXXCPP
AC_PROG_MAKE_SET
AC_PROG_RANLIB
AM_PATH_PYTHON

AC_ARG_ENABLE(doxygen-docs, AS_HELP_STRING([--enable-doxygen-docs],[build DOXYGEN documentation (requires Doxygen)]),enable_doxygen_docs=$enableval,enable_doxygen_docs=auto)

dnl ================================================================
dnl Gettext stuff.
dnl ================================================================
#AM_GNU_GETTEXT([external])
#AM_GNU_GETTEXT_VERSION(0.14.1)
#GETTEXT_PACKAGE=makedict
#AC_SUBST(GETTEXT_PACKAGE)
#AC_DEFINE_UNQUOTED(GETTEXT_PACKAGE,"$GETTEXT_PACKAGE", [Gettext package])
#AM_ICONV

### Doxygen Documentation

AC_PATH_PROG(DOXYGEN, doxygen, no)

AC_MSG_CHECKING([whether to build Doxygen documentation])

if test x$DOXYGEN = xno ; then
    have_doxygen=no
else
    have_doxygen=yes
fi

if test x$enable_doxygen_docs = xauto ; then
    if test x$have_doxygen = xno ; then
        enable_doxygen_docs=no
    else
        enable_doxygen_docs=yes
    fi
fi
if test x$enable_doxygen_docs = xyes; then
    if test x$have_doxygen = xno; then
        AC_MSG_ERROR([Building Doxygen docs explicitly required, but Doxygen not found])
    fi
fi

AM_CONDITIONAL(MAKEDICT_DOXYGEN_DOCS_ENABLED, test x$enable_doxygen_docs = xyes)
AC_MSG_RESULT(yes)


AC_FUNC_MMAP

AC_CHECK_LIB(z,zlibVersion,LIBS="$LIBS -lz",[AC_MSG_ERROR([zlib not found])])

PKG_CHECK_MODULES(MAKEDICT, glib-2.0 >= 2.6.0)
AM_WITH_EXPAT
MAKEDICT_CFLAGS="-Wall $EXPAT_CFLAGS $MAKEDICT_CFLAGS"
MAKEDICT_LIBS="-Wall $EXPAT_LIBS $MAKEDICT_LIBS"

AC_SUBST(MAKEDICT_LIBS)
AC_SUBST(MAKEDICT_CFLAGS)

AC_OUTPUT([
Doxyfile
Makefile 
m4/Makefile 
src/Makefile 
docs/Makefile
tests/Makefile 
tests/dtd/Makefile 
tests/sample-dicts/Makefile
mvsc/Makefile
])

echo "

Type \"make\" to compile makedict.

"
