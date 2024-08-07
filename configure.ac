#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_CONFIG_MACRO_DIRS([m4 /usr/share/aclocal])
AC_INIT([BuddyAlloc], [1.0], [https://github.com/evjeesm])
AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_SUBDIRS(vector)

# valgrind
AX_VALGRIND_DFLT([memcheck], [on])
AX_VALGRIND_DFLT([helgrind], [off])
AX_VALGRIND_DFLT([sgcheck], [off])
AX_VALGRIND_DFLT([drd], [off])
AX_VALGRIND_CHECK

AX_SYS_WEAK_ALIAS

# unique source file -- primitive safety check
AC_CONFIG_SRCDIR([buddy_alloc.c])

AM_INIT_AUTOMAKE([subdir-objects -Wall -Werror foreign 1.11.2])

# Checks for programs.
AM_PROG_AR
LT_INIT
AC_PROG_CC
AC_PROG_CPP
AC_PROG_CXX

# Checks for libraries.
PKG_CHECK_MODULES([CHECK], [check >= 0.9.6])

# Checks for header files.
AC_CHECK_HEADERS([stdint.h stddef.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL
AC_TYPE_SIZE_T
AC_TYPE_SSIZE_T
AC_TYPE_UINT32_T
AC_TYPE_UINT8_T
AC_FUNC_MALLOC

# Checks for library functions.
AC_CHECK_FUNCS([memset, memcpy])

# Output files 
AC_CONFIG_HEADERS([config.h])

AC_CONFIG_FILES([Makefile examples/Makefile])
AC_OUTPUT
