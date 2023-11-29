#Copyright (C) 2004 Matthias Kleinmann <Matthias dot Kleinmann at gmx dot de>

SUBDIRS = po

bin_PROGRAMS = wx12Ton
noinst_HEADERS = frame.h playfile.h wxfile_stl.h series_io.h helper.h \
                 series_widget.h enter_series.h tonal_widget.h \
                 forms.h flush_relat.h list_relat.h timed_yield.h icon.h \
                 wx12Ton.h pretty_sizer.h ids.h algorithms.h
wx12Ton_SOURCES = series_io.cpp playfile.cpp wx12Ton.cpp \
                  frame.cpp series_widget.cpp enter_series.cpp \
                  tonal_widget.cpp forms.cpp flush_relat.cpp icon.cpp \
                  list_relat.cpp algorithms.cc
wx12Ton_LDADD = ../algo/libalgo.la
wx12Ton_LDFLAGS = @WX_LIBS@
EXTRA_DIST = menu.inc wx12Ton.rc wx12Ton.ico edit.ico serieslist.ico 

INCLUDES = -I$(top_srcdir)
wx12Ton_CPPFLAGS= @WX_CPPFLAGS@

if WITH_RESOURCES
wx12Ton_LDADD += wx12Ton_resources.o

wx12Ton_resources.o: wx12Ton.rc
	@RC_COMPILE@ -i $< -o $@ @RC_FLAGS@ 
endif

if WITH_GUILE_INTERFACE
wx12Ton_CPPFLAGS += @GUILE_CFLAGS@ -DUSE_GUILE
wx12Ton_LDADD += ../guile-interface/libguile-12ton.la
wx12Ton_LDFLAGS += @GUILE_LDFLAGS@
endif

if MAINTAINER_MODE
BUILT_SOURCES = icon.cpp

%.xpm: %.ico
	convert $< xpm:- | sed -e "1,10s/^.*char.*\*[^/].*\$$/const char* $(patsubst %.xpm,%,$(basename $@))_xpm[]={/" > $@

icon.cpp: wx12Ton.xpm edit.xpm serieslist.xpm
	echo "#include \"icon.h\"" > $@
	echo "#ifdef USE_XPM_ICONS" >> $@
	cat $^ >> $@
	echo "#endif" >> $@
endif
