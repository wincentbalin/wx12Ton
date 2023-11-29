#Copyright (C) 2004 Matthias Kleinmann <Matthias dot Kleinmann at gmx dot de>

if WITH_WX_INTERFACE
 WX_SUBDIR = wx-interface
endif
if WITH_GUILE_INTERFACE
 GUILE_SUBDIR = guile-interface
endif
if WITH_SDL
 PLAYMUS_SUBDIR = playmus_12T
endif

SUBDIRS = algo $(GUILE_SUBDIR) $(PLAYMUS_SUBDIR) $(WX_SUBDIR) 
EXTRA_DIST = documentation README.de
