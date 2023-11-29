// wx-interface/forms.cpp

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

#include "forms.h"

#include "helper.h"
#include "pretty_sizer.h"
#include <wx/button.h>
#include <wx/radiobut.h>
#include <wx/stattext.h>
#include <wx/intl.h>

using namespace pretty_sizer;

wxString Forms::GetPText(Note::NoteDiff _i,bool _bR,bool _bI){
 static const wxString sp[4]={_("B%+i"),_("R%+i"),_("I%+i"),_("(I%+i)R")};
 wxString s;s.Printf(sp[(_bR?1:0)+(_bI?2:0)],_i);
 return s;
}

Series Forms::GetPSeries(Note::NoteDiff _i,bool _bR,bool _bI)const{
 if(!_bR && !_bI)return m_s.GetBasicForm (_i);
 if( _bR && !_bI)return m_s.GetRetrograde(_i);
 if(!_bR &&  _bI)return m_s.GetInversion (_i);
 if( _bR &&  _bI)return m_s.GetInversion (_i).GetRetrograde();
 return m_s; //never reached
}

void Forms::SetPSeries(const Series& _s,Note::NoteDiff _i,bool _bR,bool _bI){
 if(!_bR && !_bI)m_s=_s.GetBasicForm (-_i);
 if( _bR && !_bI)m_s=_s.GetRetrograde(-_i);
 if(!_bR &&  _bI)m_s=_s.GetInversion (-_i);
 if( _bR &&  _bI)m_s=_s.GetRetrograde(-_i).GetInversion();
}

inline wxSizer* NewLRBox(wxWindow* _wp,const wxString& _sL,const wxString& _sR){
 Sizer s(new wxBoxSizer(wxHORIZONTAL));
 s << new wxStaticText(_wp,-1,_sL) /Align(wxALIGN_LEFT)
   << wxSize(0,0) /Expand(1,wxEXPAND)
   << new wxStaticText(_wp,-1,_sR) /Align(wxALIGN_RIGHT);
 return s;
}

ShowForms::ShowForms(wxWindow* _wp,wxWindowID _id,const Series& _s):
 wxScrolledWindow(_wp,_id),Forms(_s){
 Sizer s(new wxFlexGridSizer(6));
 s.get<wxFlexGridSizer>()->AddGrowableCol(1);
 s.get<wxFlexGridSizer>()->AddGrowableCol(4);

 s << wxSize(0,0)
   << NewLRBox(this,_("basic form (B)"),_("retrograde (R)"))/Expand(0,wxEXPAND)
   << wxSize(0,0) << wxSize(0,0)
   << NewLRBox(this,_("inversion (I)"),_("retrograde inverted"))
      /Expand(0,wxEXPAND)
   << wxSize(0,0);

 for(int i=0;i<12*4;++i){
  s.get<wxFlexGridSizer>()->AddGrowableRow(i/4+1);

  s << new wxRadioButton(this,ID_RADIO_0+i,GetPText(i),wxDefaultPosition,
                                                wxDefaultSize,i?0:wxRB_GROUP)
       /Expand(0,wxEXPAND);
  m_spForms[i]=GetPSeries(i);
  if(!(i%2))
   s << new StaticSeriesCtl(this,ID_SERIES_0+i/2,m_spForms[i])
        /Expand(0,wxSHAPED)
        /Align(wxALIGN_CENTER_VERTICAL|wxALIGN_CENTER_HORIZONTAL);
 }

 SetSizer(s);
 Fit();
 int w,h;GetVirtualSize(&w,&h);
 SetScrollbars(10,10,w/10,h/10);
}

int ShowForms::m_get_selected()const{
 for(int i=0;i<12*4;++i)
  if(dynamic_cast<wxRadioButton*>(
   const_cast<ShowForms*>(this)->FindWindow(ID_RADIO_0+i))->GetValue())
   return i;
 return 0;
}

void ShowForms::SetSelect(int _i){
 for(int i=0;i<12*4;++i)
  dynamic_cast<wxRadioButton*>(FindWindow(ID_RADIO_0+i))->SetValue(i==_i);
}

void ShowForms::set_series(const Series& _s){
 SetPSeries(_s,m_get_selected());
 for(int i=0;i<12*4;++i){
  m_spForms[i]=GetPSeries(i);
  if(!(i%2))dynamic_cast<StaticSeriesCtl*>(FindWindow(ID_SERIES_0+i/2))
             ->set_series(m_spForms[i]);
 }
}

void ShowForms::SetSelect(const Series& _s){
 bool bSet=false; //if more then one matches (may occur, if symmetric), catch it
 for(int i=0;i<12*4;++i)
  if(!bSet && m_spForms[i]==_s){
   bSet=true;
   dynamic_cast<wxRadioButton*>(FindWindow(ID_RADIO_0+i))->SetValue(true);
  }else
   dynamic_cast<wxRadioButton*>(FindWindow(ID_RADIO_0+i))->SetValue(false);
}

