#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([ucsspell],[1.7.2])
AC_CONFIG_SRCDIR([config.h.in])
AC_CONFIG_HEADERS([config.h])
AC_CANONICAL_HOST
AC_USE_SYSTEM_EXTENSIONS
AC_CONFIG_MACRO_DIRS([m4])
AM_INIT_AUTOMAKE


# the following 6 lines are used for pkg-check's .pc.in file
UCSSPELL_VERSION_MAJOR=`echo $VERSION | cut -d"." -f1`
UCSSPELL_VERSION_MINOR=`echo $VERSION | cut -d"." -f2`
AS_IF([test "x$enable_static" = xyes], [PC_CFLAGS=-DUCSSPELL_STATIC])
AC_SUBST(UCSSPELL_VERSION_MAJOR)
AC_SUBST(UCSSPELL_VERSION_MINOR)
AC_SUBST(PC_CFLAGS)

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
PKG_PROG_PKG_CONFIG

LT_INIT
AC_PROG_AWK

AX_CXX_COMPILE_STDCXX_11([noext], [optional])

# Checks for libraries.
AM_ICONV
AC_REQUIRE_AUX_FILE([tap-driver.sh])

# Checks for header files.
AC_CHECK_HEADERS([locale.h langinfo.h])

# Checks for typedefs, structures, and compiler characteristics.
gl_VISIBILITY

# Checks for library functions.

dnl internationalization macros
AM_GNU_GETTEXT_VERSION(0.20)
AM_GNU_GETTEXT([external])

AC_ARG_WITH(warnings,[  --with-warnings         compile with warning messages],[
	AC_DEFINE(HUNSPELL_WARNING_ON,1,"Define if you need warning messages")
	CPPFLAGS="$CPPFLAGS -DHUNSPELL_WARNING_ON"
])



CURSESLIB=""
AC_ARG_WITH(
	[ui],
	[AS_HELP_STRING([--with-ui],[support Curses user interface])],
	[],
	[with_ui=no]
)
AS_IF([test "x$with_ui" != xno], [
	AC_CHECK_LIB([ncursesw],[tparm],[
		CURSESLIB=-lncursesw
	],[AC_CHECK_LIB([curses],[tparm],[
		CURSESLIB=-lcurses
	],[AC_CHECK_LIB([ncurses],[tparm],[
		CURSESLIB=-lncurses
	])])])
   if test "$CURSESLIB" != "" ; then
	echo Compiling with curses user interface.
	AC_CHECK_HEADERS([curses.h])
	if test "$CURSESLIB" = "-lncursesw" ; then
		AC_CHECK_HEADERS([ncursesw/curses.h])
	else
		echo "No Unicode support on interactive console. (Install Ncursesw library.)"
	fi
	AC_DEFINE(HUNSPELL_WARNING_ON,1,"Define if you need warning messages")
   fi
])
AC_SUBST(CURSESLIB)

AC_ARG_WITH(
	[readline],
	[AS_HELP_STRING([--with-readline],[support fancy command input editing])],
	[],
	[with_readline=no]
)
READLINELIB=""
AS_IF([test "x$with_readline" != xno],
	[AC_CHECK_LIB([tinfo],[tgetent], [
		TERMLIB=-ltinfo
        ],[AC_CHECK_LIB([curses],[tparm],[
		TERMLIB=-lncurses
	],[AC_CHECK_LIB([termcap],[tgetent],[
		TERMLIB=-ltermcap
	])])])
	LDSAVE=$LDFLAGS
	LDFLAGS="$LDFLAGS $TERMLIB"
	AC_CHECK_LIB([readline],[readline],[
		AC_CHECK_HEADER([readline/readline.h],[
			READLINELIB="-lreadline $TERMLIB"
			echo Using the readline library.
			AC_DEFINE([HAVE_READLINE],[1],
			["Define if you have fancy command input editing with Readline"])
		])
	])
])
AC_SUBST(READLINELIB)

# --- Generate optional pdf documentation ---
AC_ARG_WITH([pdf-generation],
    [AS_HELP_STRING([--with-pdf-generation],
        [Enable PDF documentation generation using LaTeX])],
    [with_pdf_generation=$withval],
    [with_pdf_generation=no])

if test "x$with_pdf_generation" = "xyes"; then
    AC_MSG_CHECKING([for LaTeX (pdflatex)])
    AC_PATH_PROG([PDFLATEX], [pdflatex])
    if test -z "$PDFLATEX"; then
        AC_MSG_ERROR([pdflatex not found! Please install a LaTeX distribution to generate PDF documentation.])
    else
        AC_MSG_RESULT([$PDFLATEX found])
    fi
fi

# --- Python Code Generation Support ---
# Option: --with-python-codegen
# Enables regeneration of C++ Unicode data tables using Python scripts.
AC_ARG_WITH([python-codegen],
  [AS_HELP_STRING([--with-python-codegen], [Regenerate C++ Unicode tables using Python scripts])],
  [with_python_codegen=$withval],
  [with_python_codegen=no])

if test "x$with_python_codegen" = "xyes"; then
    AM_PATH_PYTHON([3.6], [], [
        AC_MSG_ERROR([python3 not found! Python 3.6 or higher is required for code generation.])
    ])
fi
AM_CONDITIONAL([ENABLE_PYTHON_CODEGEN], [test "x$with_python_codegen" = "xyes"])
AC_SUBST([WITH_PYTHON_CODEGEN], [$with_python_codegen])

# --- Online Unicode Data Download Support ---
# Option: --with-online-uc-data
# Downloads the latest UnicodeData.txt from unicode.org during the build process.
AC_ARG_WITH([online-uc-data],
  [AS_HELP_STRING([--with-online-uc-data], [Download latest UnicodeData.txt and iso15924.txt from unicode.org])],
  [with_online_uc_data=$withval],
  [with_online_uc_data=no])

if test "x$with_online_uc_data" = "xyes"; then
    AC_CHECK_PROGS([CURL], [curl wget], [])
    if test -z "$CURL"; then
        AC_MSG_ERROR([Neither curl nor wget was found! Required to download Unicode data.])
    fi
fi
AM_CONDITIONAL([ENABLE_ONLINE_UC_DATA], [test "x$with_online_uc_data" = "xyes"])
AC_SUBST([WITH_ONLINE_UC_DATA], [$with_online_uc_data])

AC_CONFIG_FILES([
	Makefile
	ucsspell.pc
	po/Makefile.in
	man/Makefile
	man/hu/Makefile
	src/Makefile
	src/hunspell/Makefile
	src/hunspell/hunvisapi.h
	src/parsers/Makefile
	src/tools/Makefile
	tests/Makefile
	tests/suggestiontest/Makefile
])
AC_OUTPUT
