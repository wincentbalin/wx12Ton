#Copyright (C) 2004 Matthias Kleinmann <Matthias dot Kleinmann at gmx dot de>

demessagesdir = $(datadir)/locale/de/LC_MESSAGES
dist_demessages_DATA = de/wx12Ton.mo
EXTRA_DIST = u wx12Ton.pot de.po

if MAINTAINER_MODE
%/wx12Ton.mo: %.po 
	[ -d $(dir $@) ] || mkdir -p $(dir $@)
	msgfmt $^ -o $@
endif
