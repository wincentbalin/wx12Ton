// wx-interface/helper.h
//  contains the menu structure, using macro calls

// Copyright (C) 2004 Matthias Kleinmann <Matthias dot Kleinmann at gmx dot de>
// All rights reserved.
//
// This file is part of wx12Ton, http://wx12ton.sourceforge.net/
//
// wx12Ton is free software; you can redistribute it and/or modify it under the
// terms of the GNU General Public License as published by the Free Software
// Foundation; either version 2, or (at your option) any later version.
// 
// wx12Ton is distributed in the hope that it will be useful, but WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
// details.
// 
// You should have received a copy of the GNU General Public License along with
// wx12Ton; see the file COPYING or COPYING.txt. If not, write to the Free
// Software Foundation, 59 Temple Place - Suite 330, Boston, MA 02111-1307,
// USA.

BEGIN_MENU(_("&Form"))
 SUBMENU(Enter12T,
  _("&Edit"),_("Edit selected form."))
 SUBMENU(SelectNorm,
  _("&Goto nomalized"),_("Select normalized form."))
 SUBMENU(Set,
  _("Se&t"),_("Set selected form as new basic form."))
 SUBMENU(Play,
  _("&Play"),_("Play selected form on MIDI device."))
 SEPERATOR
 SUBMENU(Open,
  _("&Open..."),_("Load a series from a !12Ton MIDI-file"))
 SUBMENU(Save,
  _("&Save as..."),_("Save selected form as !12Ton MIDI-file."))
END_MENU

BEGIN_MENU(_("&Other"))
 SUBMENU(Print,
  _("&Print..."),_("Print all forms."))
 SUBMENU(FlushRelatives,
  _("&Find series..."),_("Show the complete set of normalized series."))
 SEPERATOR
#ifdef USE_GUILE
 SUBMENU(LoadAlgo,
  _("&Load Algorithms"),_("Load a different set of algorithms."))
 SEPERATOR
#endif
 SUBMENU(About,
  _("&About..."),_("about !12Ton"))
 SUBMENU(Exit,
  _("&Exit"),_("Exit !12Ton."))
END_MENU

