EXTRA_PROGRAMS = ignitech_example ignitech_example_dyn tcip4_emulator
ignitech_example_SOURCES = ignitech.cpp
ignitech_example_dyn_SOURCES = ignitech.cpp
tcip4_emulator_SOURCES = tcip4.cpp error_handling.cpp
ignitech_example_LDADD = ../libignitech.la
ignitech_example_dyn_LDADD = -lignitech
example:
	$(MAKE) $(AM_MAKEFLAGS) -C .. all
	$(MAKE) $(AM_MAKEFLAGS) ignitech_example
	$(MAKE) $(AM_MAKEFLAGS) ignitech_example_dyn
	$(MAKE) $(AM_MAKEFLAGS) tcip4_emulator
