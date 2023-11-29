// wx-interface/frame.h:
//  the main window

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

#ifndef FRAME_H
#define FRAME_H

#include "forms.h"
#include "wx12Ton.h"
#include "ids.h"
#include <wx/frame.h>
#include <wx/event.h>
#include <string>

#define BEGIN_MENU(x)
#define SEPERATOR
#define END_MENU

class FormsFrame:public wxFrame,public has_playable_series{
 public:
  FormsFrame(wxWindow*,wxWindowID,const Series&);

  virtual void ReportStartPlay();
  virtual void ReportStopPlay() ;

# define SUBMENU(z,x,y) void CB_Menu_##z(wxCommandEvent&);
# include "menu.inc"
# undef SUBMENU

  enum {MENU_ITEMX_FIRST=ID_FormsFrame
# define SUBMENU(z,x,y) ,MENU_ITEM_##z
# include "menu.inc"
# undef SUBMENU
   };

 private:
  ShowForms* m_srp;
  std::string m_sFunctions;
};

#undef BEGIN_MENU
#undef END_MENU
#undef SEPERATOR

#endif
