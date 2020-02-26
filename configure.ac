#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([IGNITECH], [0.1], [BUG-REPORT-ADDRESS])
AC_CONFIG_SRCDIR([ignitech.cpp])
AC_CONFIG_HEADERS([config.h])
AM_INIT_AUTOMAKE
LT_INIT

# Checks for programs.
AC_PROG_AWK
AC_PROG_CPP
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET
AC_PROG_RANLIB
AC_PROG_CXX
AC_PROG_CC

# Checks for libraries.

# Checks for header files.
AC_CHECK_HEADERS([stdint.h stdlib.h stdio.h unistd.h assert.h])

# Checks for typedefs, structures, and compiler characteristics.
#AX_CXX_COMPILE_STDCXX_11
AC_CHECK_HEADER_STDBOOL
AC_TYPE_UINT16_T
AC_TYPE_UINT8_T
AC_C_BIGENDIAN

# Checks for library functions.

AC_CONFIG_FILES([Makefile example/Makefile])
AC_OUTPUT
