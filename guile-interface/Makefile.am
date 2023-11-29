#Copyright (C) 2004 Matthias Kleinmann <Matthias dot Kleinmann at gmx dot de>

guiledir = @GUILE_SITE@
lib_LTLIBRARIES = libguile-12ton.la
noinst_HEADERS = guile_helper.h enc_scm.h libguile-12ton.h
guile_SCRIPTS = x12ton.scm
libguile_12ton_la_SOURCES = enc_scm.cc libguile-12ton.cc
libguile_12ton_la_LIBADD = ../algo/libalgo.la @GUILE_LDFLAGS@
libguile_12ton_la_CPPFLAGS = @GUILE_CFLAGS@
libguile_12ton_la_LDFLAGS = -version-info 1:0:0

INCLUDES = -I$(top_srcdir)

EXTRA_DIST = x12ton.scm
