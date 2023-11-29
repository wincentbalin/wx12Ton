// wx-interface/wx12Ton.h
//  the "main" file (the application class)

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

#ifndef WX12TON_H
#define WX12TON_H

#include "playfile.h"
#include "algo/series.h"
#include "algorithms.h"
#include <wx/app.h>
#include <wx/intl.h>
#include <wx/cmdline.h>

class wx12Ton:public wxApp{
 public:
  wx12Ton():m_s("hGhAhJhKhBhEhChFhIhHhLhD"){}

  virtual bool OnInit();
  virtual void OnInitCmdLine(wxCmdLineParser&);
  virtual bool OnCmdLineParsed(wxCmdLineParser&);

  wxString getLocaleName()const{
   if(m_locale.IsOk())return m_locale.GetName();else return _T("en_EN");}
  AlgorithmList get_algorithms()const;

  play_midi_file& get_midi_player(){return m_pmf;}
 private:
  wxLocale m_locale;
  Series m_s;
  play_midi_file m_pmf;
};

DECLARE_APP(wx12Ton)

class has_playable_series:public has_playable{
 public:
  has_playable_series():has_playable(wxGetApp().get_midi_player()){};
  bool PlaySeries(const Series&);
};

#endif
