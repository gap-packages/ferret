#
# ferret: 
#
# This file is part of the build system of a GAP kernel extension.
# Requires GNU autoconf, GNU automake and GNU libtool.
#

dnl ##
dnl ## Setup autoconf
dnl ##
AC_PREREQ([2.68])
AC_INIT([ferret], m4_esyscmd([tr -d '\n' < VERSION]))
AC_CONFIG_SRCDIR([src/cppmapper.cc])
AC_CONFIG_HEADER([src/pkgconfig.h:cnf/pkgconfig.h.in])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_AUX_DIR([cnf])
m4_include([m4/ac_find_gap.m4])

dnl ##
dnl ## Get canonical host info
dnl ##
AC_CANONICAL_HOST

dnl ##
dnl ## Setup automake
dnl ##
AM_INIT_AUTOMAKE([1.11 -Wall foreign subdir-objects])
AM_SILENT_RULES([yes])
AM_PROG_AR

dnl ##
dnl ## Set the language
dnl ##
AC_PROG_CXX
AC_LANG([C++])

dnl ##
dnl ## Setup libtool (for building the GAP kernel extension)
dnl ##
LT_PREREQ([2.4.2])
LT_INIT([disable-static dlopen win32-dll])

dnl ##
dnl ## Locate the GAP root dir
dnl ##
AC_FIND_GAP

dnl ##
dnl ## Detect Windows resp. Cygwin
dnl ##
case $host_os in
  *cygwin* ) CYGWIN=yes;;
         * ) CYGWIN=no;;
esac
AM_CONDITIONAL([SYS_IS_CYGWIN], [test "$CYGWIN" = "yes"])
if test "$CYGWIN" = "yes"; then
  AC_DEFINE(SYS_IS_CYGWIN32, 1, are we on CYGWIN?)
else
  AC_DEFINE(SYS_IS_CYGWIN32, 0, are we on CYGWIN?)
fi

AC_ARG_ENABLE(checking,
[  --enable-checking=<checking level (defaults to 0)>],
[   case "$enableval" in
        yes) FERRET_CHECK=1
        ;;
        no) FERRET_CHECK=0
        ;;
        *) FERRET_CHECK=$enableval
        ;;
    esac], [FERRET_CHECK=0])
AC_SUBST(FERRET_CHECK)

AC_ARG_ENABLE(timing,
[  --enable-timing=<enable timing (defaults to off)>],
	[FERRET_TIMING=-DENABLE_TIMING], [])
AC_SUBST(FERRET_TIMING)

AC_ARG_ENABLE(printing,
[  --enable-printing=<debug printing level (defaults to 0)>],
[  case "${enableval}" in
        yes) FERRET_PRINT=1
        ;;
        no) FERRET_PRINT=0
        ;;
        *) FERRET_PRINT=$enableval
        ;;
    esac], [FERRET_PRINT=0])
AC_SUBST(FERRET_PRINT)


dnl ##
dnl ## Rebuild configure if VERSION file changes
dnl ##
AC_SUBST([CONFIG_STATUS_DEPENDENCIES], ['$(top_srcdir)/VERSION'])

dnl ##
dnl ## Output everything
dnl ##
AC_CONFIG_FILES([Makefile ferret.vars])
AC_OUTPUT
