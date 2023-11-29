// wx-interface/tonal_widget.h:
//  widget to select the tonal errors

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
// wx12Ton; see the file COPYING or COPYING.txt. If not, write to the Free      // Software Foundation, 59 Temple Place - Suite 330, Boston, MA 02111-1307,
// USA.

#ifndef TONAL_WIDGET_H
#define TONAL_WIDGET_H

#include "algo/series.h"
#include "ids.h"
#include <wx/panel.h>
#include <wx/event.h>
#include <wx/string.h>
#include <wx/intl.h>
//std::pair
#include <utility>

class TonalErrorsCtl:public wxPanel{
 public:
  TonalErrorsCtl(wxWindow*,wxWindowID,int,const wxString& =_("tonal is"));
  virtual int GetValue(){TransferDataFromWindow();return m_i;}

  template <class T>
  void ConnectAll(T* _t,void(T::* _fp)(int)){
   m_connect_cb(CallB(_t,static_cast<CallB::second_type>(_fp)));}

 private:
  typedef std::pair<wxObject*,void(wxObject::*)(int)> CallB;
  enum{CB_FOURTH=ID_TonalErrorsCtl,CB_CHORD,CB_EQ_INTERV,CB_ALL_INTERV};

  CallB m_cb;
  int m_i;

  void m_connect_cb(CallB _cb){m_cb=_cb;}
  void CB_Clicked(wxCommandEvent&);
  DECLARE_EVENT_TABLE()
};

#endif
