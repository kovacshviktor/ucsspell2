bin_PROGRAMS=analyze chmorph ucsspell munch unmunch hzip hunzip

AM_CPPFLAGS=-I${top_builddir}/src/hunspell -I${top_srcdir}/src/hunspell -I${top_srcdir}/src/parsers  -DDATADIR=\"$(datadir)\"

hzip_SOURCES=hzip.cxx
hunzip_SOURCES=hunzip.cxx
hunzip_LDADD = ../hunspell/libucsspell-1.7.la

munch_SOURCES=munch.cxx munch.h
unmunch_SOURCES=unmunch.cxx unmunch.h

example_SOURCES=example.cxx
example_LDADD = ../hunspell/libucsspell-1.7.la

ucsspell_SOURCES=hunspell.cxx
nodist_ucsspell_SOURCES= ../../config.h
ucsspell_LDADD = @LIBICONV@ ../parsers/libparsers.a \
	../hunspell/libucsspell-1.7.la @CURSESLIB@ @READLINELIB@

analyze_SOURCES=analyze.cxx
analyze_LDADD = ../hunspell/libucsspell-1.7.la

chmorph_SOURCES=chmorph.cxx
chmorph_LDADD = ../parsers/libparsers.a ../hunspell/libucsspell-1.7.la

noinst_PROGRAMS=example persdiccrash

persdiccrash_SOURCES=persdiccrash.cxx
persdiccrash_LDADD = ../hunspell/libucsspell-1.7.la

dist_bin_SCRIPTS=makealias affixcompress wordforms ispellaff2myspell wordlist2hunspell
