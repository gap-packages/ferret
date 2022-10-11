#
# Ferret: Partition Backtrack
#
# This file is part of the build system of a GAP kernel extension.
# Requires GNU autoconf, GNU automake and GNU libtool.
#

dnl ##
dnl ## Setup autoconf
dnl ##
AC_PREREQ([2.68])
AC_INIT([ferret], [package], [https://github.com/gap-packages/ferret/issues], [ferret], [https://gap-packages.github.io/ferret/])
AC_CONFIG_SRCDIR([src/cppmapper.cc])
AC_CONFIG_MACRO_DIR([m4])
m4_include([m4/find_gap.m4])

dnl ##
dnl ## Set the language
dnl ##
AC_PROG_CXX
AC_LANG([C++])

dnl ##
dnl ## Locate the GAP root dir
dnl ##
FIND_GAP
GAPEXEC="${GAP:-${GAPROOT}/bin/gap.sh}" # GAP >= 4.12.0 sets "GAP"
AC_SUBST(GAPEXEC)

dnl ##
dnl ## Set flags so libtool will use them
dnl ##
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
dnl ## Output everything
dnl ##
AC_CONFIG_FILES([Makefile ferret.vars])
AC_OUTPUT
