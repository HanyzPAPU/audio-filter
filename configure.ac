AC_PREREQ([2.69])
AC_INIT([audio-filter], [0.1])

AM_INIT_AUTOMAKE([-Wall -Werror foreign])
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])
LT_INIT
AC_PROG_CXX

AC_CONFIG_SRCDIR([src/])
AC_CONFIG_MACRO_DIRS([m4])

AC_CHECK_LIB([fftw3], [fftw_cleanup])
AC_CHECK_LIB([sndfile], [sf_open])
dnl TODO: dunno if this works on other machines
AC_CHECK_LIB([stdc++fs], [std::experimental::filesystem::exists])

AC_CONFIG_FILES([Makefile])

AC_TYPE_SIZE_T()
AC_CHECK_HEADER_STDBOOL()
AC_OUTPUT