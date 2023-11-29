// wx-interface/forms.h:
//  handle the set of all forms

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

#ifndef PERSP_H
#define PERSP_H

#include "algo/series.h"  
#include "series_widget.h"
#include "ids.h"
#include <wx/scrolwin.h>

class Forms{
 public:
  virtual ~Forms(){}
  explicit Forms(const Series& _s):m_s(_s){}
  static wxString GetPText(int _i){return GetPText(_i/4,_i%2,(_i/2)%2);}
  static wxString GetPText(Note::NoteDiff,bool,bool);
  Series GetPSeries(int _i)const{return GetPSeries(_i/4,_i%2,(_i/2)%2);}
  Series GetPSeries(Note::NoteDiff,bool,bool)const;

  void SetPSeries(const Series& _s,int _i){SetPSeries(_s,_i/4,_i%2,(_i/2)%2);}
  void SetPSeries(const Series&,Note::NoteDiff,bool,bool);
 private:
  Series m_s;
};

class ShowForms:public wxScrolledWindow,public Forms,public SeriesGetSet_base{
 public:
  ShowForms(wxWindow*,wxWindowID,const Series&);

  virtual void set_series(const Series&);
  virtual const Series& get_series()const{return m_spForms[m_get_selected()];}
  void SetSelect(const Series& _s);
  void SetSelect(int _i);

 private:
  enum {ID_RADIO_0=ID_Forms,ID_SERIES_0=ID_Forms+48};
  int m_get_selected()const;

  Series m_spForms[24*4];
};

#endif
