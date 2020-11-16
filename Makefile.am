lib_LTLIBRARIES = src/libignitech.la
src_libignitech_la_SOURCES = src/ignitech.cpp src/serial.cpp
include_HEADERS = src/ignitech.h
src_libignitech_la_LDFLAGS = -version-info 2:0:0
examples:
	$(MAKE) $(AM_MAKEFLAGS) -C example example
git-clean: distclean
	rm -fr autom4te.cache
	rm -f Makefile.in aclocal.m4 compile config.guess config.h.in config.h.in~ config.sub configure depcomp example/.dirstamp example/Makefile.in install-sh ltmain.sh missing

noinst_PROGRAMS =  			example/ignitech_example \
					example/tcip4_emulator
EXTRA_PROGRAMS =			example/ignitech_example_dyn
example_ignitech_example_SOURCES =	example/ignitech.cpp
example_ignitech_example_dyn_SOURCES =	example/ignitech.cpp
example_tcip4_emulator_SOURCES = 	example/tcip4.c \
					example/error_handling.c \
					example/cmdline.c
example_ignitech_example_LDADD =	src/libignitech.la
example_ignitech_example_dyn_LDADD =	-lignitech
example:
	$(MAKE) $(AM_MAKEFLAGS) -C .. all
	$(MAKE) $(AM_MAKEFLAGS) ignitech_example
	$(MAKE) $(AM_MAKEFLAGS) tcip4_emulator

example_dyn:
	$(MAKE) $(AM_MAKEFLAGS) ignitech_example_dyn

check_PROGRAMS =		check/rpm \
				check/sensormv \
				check/sensorval \
				check/advance \
				check/maxadvance \
				check/battmv \
				check/sensor_type
AM_DEFAULT_SOURCE_EXT = .cpp
LDADD =				src/libignitech.la
TESTS =				check/rpm.sh \
				check/sensormv.sh \
				check/sensorval.sh \
				check/advance.sh \
				check/maxadvance.sh \
				check/battmv.sh \
				check/sensor_type.sh
