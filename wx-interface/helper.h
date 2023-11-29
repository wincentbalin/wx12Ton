// wx-interface/helper.h:
//  some convertes to pretty strings

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

#ifndef HELPER_H
#define HELPER_H

#include "algo/series.h"
#include <wx/string.h>
#include <wx/intl.h>

inline wxString tone2str(const Note::Tone& _t){
 const wxString sp[12]={
  _("c natural"),_("c sharp"),_("d natural"),_("e flat"),_("e natural"),
  _("f natural"),_("f sharp"),_("g natural"),_("g sharp"),_("a natural"),
  _("b flat"),_("b natural")};
 return sp[_t];
}

inline wxString tone_occurs_twice_str(const Note::Tone& _t){
 wxString s;
 s.Printf(_("note \"%s\" occurs twice"),tone2str(_t).c_str());
 return s;
}

inline wxString tonal_error_str(Series::TonalError _te){
 switch(_te){
  case Series::TE_FOURTH    :return _("perfect fourth/fifth");
  case Series::TE_CHORD     :return _("major/minor chord");
  case Series::TE_EQ_INTERV :return _("three identical intervals in a row");
  case Series::TE_ALL_INTERV:return _("not all intervals do appear");
  case Series::TE_OK        :return _("series tests OK");
 }
 return _T("");
}

#endif
