lib_LTLIBRARIES = libignitech.la
libignitech_la_SOURCES = ignitech.cpp serial.cpp
include_HEADERS = ignitech.h
SUBDIRS = . example
examples:
	$(MAKE) $(AM_MAKEFLAGS) -C example example

git-clean: distclean
	rm -fr autom4te.cache
	rm -f Makefile.in aclocal.m4 compile config.guess config.h.in config.h.in~ config.sub configure depcomp example/.dirstamp example/Makefile.in install-sh ltmain.sh missing
