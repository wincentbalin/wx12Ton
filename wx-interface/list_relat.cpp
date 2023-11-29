// wx-interface/list_relat.cpp

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

#include "list_relat.h"

#include "tonal_widget.h"
#include "pretty_sizer.h"
#include <wx/string.h>
#include <wx/intl.h>
#include <wx/stattext.h>
#include <wx/statline.h>
#include <wx/spinctrl.h>
#include <wx/radiobut.h>
#include <wx/button.h>

using namespace pretty_sizer;

DisplaySeries::DisplaySeries(wxWindow* _wp,wxWindowID _id,
 std::auto_ptr<SortedCS> _scsp,int _i):wxPanel(_wp,_id),m_iPos(0),m_iLen(0),
 m_scsp(_scsp){
 wxGridSizer* bsp=new wxGridSizer(1);
 m_setlength(_i,bsp);
 SetSizerAndFit(bsp);
 m_scroll();
}

void DisplaySeries::m_setlength(size_t _i,wxSizer* _sp){
 SetScrollbar(wxVERTICAL,GetScrollPos(wxVERTICAL),_i,m_scsp->size());
 for(;_i<m_iLen;--m_iLen){
  _sp->Remove(m_iLen-1);
  delete FindWindow(ID_RADIOBOX_0+m_iLen-1);
  delete FindWindow(ID_SERIES_0+m_iLen-1);
 }
 wxString s;s.Printf(_T("%i"),m_scsp->size());
 Sizer ps(_sp);
 for(;m_iLen<_i;++m_iLen){
  *ps*new wxBoxSizer(wxHORIZONTAL)*Expand(0,wxEXPAND)
   << new wxRadioButton(this,ID_RADIOBOX_0+m_iLen,s,wxDefaultPosition,
                                  wxDefaultSize,m_iLen?0:wxRB_GROUP)
      /Expand(0,wxGROW)
   << new StaticSeriesCtl(this,ID_SERIES_0+m_iLen,(*m_scsp)[m_iLen])
      /Expand(1,wxSHAPED)
      /Align(wxALIGN_CENTER_VERTICAL|wxALIGN_CENTER_HORIZONTAL);
  !ps;
  Connect(ID_RADIOBOX_0+m_iLen,wxEVT_COMMAND_RADIOBUTTON_SELECTED,
                         (wxObjectEventFunction)&DisplaySeries::CB_BoxClicked);
 }
 ps.get<wxSizer>()->Layout();
}

void DisplaySeries::CB_Scroll(wxScrollWinEvent& _se){
 long i=GetScrollPos(wxVERTICAL);
 const long iMax=m_scsp->size()-m_iLen;

 if(_se.GetEventType()==wxEVT_SCROLLWIN_TOP         )i=0;
 if(_se.GetEventType()==wxEVT_SCROLLWIN_BOTTOM      )i=iMax;
 if(_se.GetEventType()==wxEVT_SCROLLWIN_LINEUP      )--i;
 if(_se.GetEventType()==wxEVT_SCROLLWIN_LINEDOWN    )++i;
 if(_se.GetEventType()==wxEVT_SCROLLWIN_PAGEUP      )i-=m_iLen;
 if(_se.GetEventType()==wxEVT_SCROLLWIN_PAGEDOWN    )i+=m_iLen;
 if(_se.GetEventType()==wxEVT_SCROLLWIN_THUMBRELEASE
  ||_se.GetEventType()==wxEVT_SCROLLWIN_THUMBTRACK  )i=_se.GetPosition();

 if(i<0)i=0;if(i>iMax)i=iMax;
 SetScrollPos(wxVERTICAL,i);
 Refresh();
 m_scroll();
}

void DisplaySeries::m_scroll(){
 size_t iP=GetScrollPos(wxVERTICAL);
 for(size_t i=0;i<m_iLen;++i){
  dynamic_cast<StaticSeriesCtl*>(FindWindow(ID_SERIES_0+i))
   ->set_series((*m_scsp)[i+iP]);
  wxString s;s.Printf(_T("%i"),i+iP+1);
  wxRadioButton* rbp=dynamic_cast<wxRadioButton*>(FindWindow(ID_RADIOBOX_0+i));
  rbp->SetLabel(s);
  rbp->SetValue(false);
 }
 if(m_iPos<iP)m_iPos=iP;
 if(m_iPos>=iP+m_iLen)m_iPos=iP+m_iLen-1;
 dynamic_cast<wxRadioButton*>(FindWindow(ID_RADIOBOX_0+m_iPos-iP))
  ->SetValue(true);
}

BEGIN_EVENT_TABLE(DisplaySeries,wxPanel)
 EVT_SCROLLWIN(DisplaySeries::CB_Scroll)
END_EVENT_TABLE()

DisplaySeriesInfo::DisplaySeriesInfo(wxWindow* _wp,wxWindowID _id,
 std::auto_ptr<SortedCS> _scsp,const SeriesValidator& _sv):wxPanel(_wp,_id),
 m_dsp(new DisplaySeries(this,-1,_scsp,8)){
 SetValidator(_sv);

 TonalErrorsCtl* tecp=new TonalErrorsCtl(this,-1,
                                              m_dsp->GetSCS().GetTonalError());
 tecp->Enable(false);
 wxButton* btnp=new wxButton(this,BTN_Audio,_("&play"));
 if(MidiIsPlaying())btnp->Disable();

 Sizer s(new wxBoxSizer(wxHORIZONTAL));
 s << m_dsp /Expand(1,wxEXPAND);

 *s*new wxBoxSizer(wxVERTICAL)*(Expand(0,wxEXPAND)|Border(wxALL))
  << new wxStaticText(this,-1,
      _("The list on the left contains all normalized series,\n"
        "excluding series, which contain tonal phrases, where"))
     /Expand(0,wxEXPAND)
  << tecp /Border(wxTOP)/Expand(0,wxEXPAND);
  *s*new wxBoxSizer(wxHORIZONTAL)*(Expand(0,wxEXPAND)|Border(wxTOP))
   << new wxStaticText(this,-1,_("number of series to show in window:"))
      /Align(wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL)
   << wxSize(0,0)/Expand(1,wxEXPAND);
   wxString sTmp;sTmp.Printf(_T("%i"),m_dsp->GetLength());
   s << new wxSpinCtrl(this,SPN_Length,sTmp,wxDefaultPosition,
         wxDefaultSize,wxSP_ARROW_KEYS,2,MAX_SERIES_LL,m_dsp->GetLength())
        /Align(wxALIGN_RIGHT);
  !s;
  
  s << wxSize(0,0)/Expand(1,wxEXPAND);

  s << new wxStaticLine(this,-1) /Expand(0,wxEXPAND)/Border(wxTOP);
  *s*new wxBoxSizer(wxHORIZONTAL)*
      (Border(wxTOP)|Align(wxALIGN_CENTER_HORIZONTAL))
   << new wxButton(this,wxID_APPLY,_("&apply")) /Expand(0,wxEXPAND)
   << wxSize(0,0)/Expand(1,wxEXPAND)/Border(wxLEFT|wxRIGHT)
   << btnp /Expand(0,wxEXPAND)
   << wxSize(0,0)/Expand(1,wxEXPAND)/Border(wxLEFT|wxRIGHT)
   << new wxButton(this,wxID_CANCEL,_("&close window")) /Expand(0,wxEXPAND);
  !s;
 !s;
 
 SetSizerAndFit(s);
}

BEGIN_EVENT_TABLE(DisplaySeriesInfo,wxPanel)
 EVT_BUTTON(wxID_APPLY  ,DisplaySeriesInfo::CB_Apply  )
 EVT_BUTTON(BTN_Audio   ,DisplaySeriesInfo::CB_Audio  )
 EVT_BUTTON(wxID_CANCEL ,DisplaySeriesInfo::CB_Close  )
 EVT_SPINCTRL(SPN_Length,DisplaySeriesInfo::CB_ChangeL)
END_EVENT_TABLE()
