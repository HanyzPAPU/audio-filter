#!/bin/bash

# simple autogen script that generates basic layout for autotools.

NAME="audiofilter"

OUT=Makefile.am

echo > $OUT

DISTDIRS=""

echo "AUTOMAKE_OPTIONS = subdir-objects" >>$OUT
echo "ACLOCAL_AMFLAGS = -I m4" >>$OUT

echo "bin_PROGRAMS = ${NAME}" >>$OUT
echo "${NAME}_SOURCES = `( find src/ -type f -name \*.c ; find src/ -type f -name \*.cpp ) |tr \"\n\" \" \" ` " >>$OUT
echo "noinst_HEADERS = `find src/ -type f -name \*.h |tr \"\n\" \" \" `" >>$OUT
echo "AM_CPPFLAGS = -I\$(top_srcdir)" >>$OUT
echo "AM_CFLAGS = -Wall" >>$OUT
echo "${NAME}_CPPFLAGS = \$(FFTW3_CFLAGS)" >>$OUT
echo "${NAME}_LDADD = -lfftw3 -lsndfile -lstdc++fs " >>$OUT

if [[ "$OSTYPE" == "darwin"* ]]; then
  glibtoolize --force && aclocal && autoconf && automake --add-missing
else
  libtoolize --force && aclocal && autoconf && automake --add-missing
fi