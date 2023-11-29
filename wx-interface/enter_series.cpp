// wx-interface/enter_series.cpp

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

#include "enter_series.h"

#include "helper.h"
#include "timed_yield.h"
#include "icon.h"
#include "tonal_widget.h"
#include "wx12Ton.h"
#include "pretty_sizer.h"

#include <wx/button.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/statline.h>
#include <wx/checkbox.h>
#include <wx/textctrl.h>
#include <wx/statbox.h>
#include <wx/listbox.h>
#include <wx/spinctrl.h>

using namespace pretty_sizer;

inline void ForceMinSize(wxWindow* _wp){
 int w,h;_wp->GetSize(&w,&h);_wp->SetSizeHints(w,h);
}

EnterSeriesPanel::EnterSeriesPanel(wxWindow* _wp,wxWindowID _id,
 const SeriesValidator& _sv,const AlgorithmList& _al):
 wxPanel(_wp,_id),SeriesGetSet(_sv.get_target_series()),m_algolist(_al){
 m_rctlp=new SeriesCtl(this,-1,SeriesValidator(*this));
 SetValidator(_sv);

 ForceMinSize(m_rctlp);
 TonalErrorsCtl* tecp=new TonalErrorsCtl(this,TEC_Errors,Series::TE_DEFAULT,
                                         _("tonal is (t)"));
 tecp->ConnectAll(this,&EnterSeriesPanel::CB_TonalChange);

 wxButton* btnpPlay=new wxButton(this,BTN_Audio,_("&play"));
 if(MidiIsPlaying())btnpPlay->Disable();

 wxListBox* lbpAlgo=new wxListBox(this,LB_Algo,wxDefaultPosition,wxDefaultSize,
                                  0,NULL,wxLB_SINGLE);

 Sizer s(new wxBoxSizer(wxVERTICAL));
 *s*new wxBoxSizer(wxHORIZONTAL)*(Expand(1,wxEXPAND)|Border(wxALL));
  *s*new wxBoxSizer(wxVERTICAL)*Expand(1,wxEXPAND)
   << new wxStaticText(this,-1,
       _("The series can be modfied with the arrow keys and PgUp/PgDown."))
   << m_rctlp/Expand(1,wxSHAPED)/Border(wxTOP)/Align(wxALIGN_CENTER_HORIZONTAL);
   *s*new wxStaticBoxSizer(new wxStaticBox(this,-1,_("tonal phrases")),
                            wxHORIZONTAL)*(Expand(0,wxEXPAND)|Border(wxTOP))
    << new wxTextCtrl(this,ST_Message,_T(""),wxDefaultPosition,wxDefaultSize,
                      wxTE_READONLY)
       /Expand(1)/Align(wxALIGN_CENTER_VERTICAL)
    << new wxButton(this,BTN_Contr,_("&goto error"))
       /Align(wxALIGN_CENTER_VERTICAL)/Border(wxLEFT);
   !s;
 
   *s*new wxStaticBoxSizer(new wxStaticBox(this,-1,_("algorithms")),
             wxHORIZONTAL)*(Border(wxTOP,10)|Expand(0,wxEXPAND));
    s
    << lbpAlgo/Expand(1,wxEXPAND)
    << wxSize(0,0)/Border(wxLEFT|wxRIGHT)
    << new wxButton(this,BTN_Algo,_("&run"))/Align(wxALIGN_CENTER_VERTICAL);
   !s;
  !s;
 
  s << new wxStaticLine(this,-1,wxDefaultPosition,wxDefaultSize,wxVERTICAL)
        /Expand(0,wxEXPAND)/Border(wxLEFT|wxRIGHT);
  *s*new wxBoxSizer(wxVERTICAL)*Expand(0,wxEXPAND)
   << new wxCheckBox(this,CBX_Oct,_("use octave information"))
   << tecp/Border(wxTOP);
   *s*new wxBoxSizer(wxHORIZONTAL)*Border(wxTOP)
    << new wxStaticText(this,-1,_("iterate (m)"))/Align(wxALIGN_CENTER_VERTICAL)
    << new wxSpinCtrl(this,SB_Count,_T("1"),wxDefaultPosition,wxDefaultSize,
                      wxSP_ARROW_KEYS,-10000000,10000000,1)
       /Align(wxALIGN_CENTER_HORIZONTAL)/Border(wxLEFT);
   !s;
  !s;
 !s;

 s << new wxStaticLine(this,-1)/Expand(0,wxEXPAND);
  
 *s*new wxBoxSizer(wxHORIZONTAL)*
                 (Align(wxALIGN_CENTER_HORIZONTAL)|Border(wxALL))
  << new wxButton(this,wxID_APPLY,_("&apply"))/Expand(0,wxEXPAND)
  << wxSize(0,0)/Expand(1)/Border(wxLEFT|wxRIGHT)
  << btnpPlay/Expand(0,wxEXPAND)
  << wxSize(0,0)/Expand(1)/Border(wxLEFT|wxRIGHT)
  << new wxButton(this,wxID_CANCEL,_("&close window"))/Expand(0,wxEXPAND);
 !s;

 std::string sL((const char*)wxGetApp().getLocaleName().mb_str(wxConvUTF8),2);
 for(size_t i=0;i<m_algolist.size();++i){
  if(!i)lbpAlgo->SetSelection(0);
  lbpAlgo->Append(wxString(m_algolist[i].find_name(sL).c_str(),wxConvLocal));
 }

 SetSizerAndFit(s);
 TransferDataToWindow();
 m_control();
 m_set_focus();
}

void EnterSeriesPanel::CB_Oct(wxCommandEvent& _ce){
 Series s=get_series();
 if(_ce.IsChecked())s.set_oct(0);else s.set_no_oct();
 set_series(s);
 m_set_focus();
}

void EnterSeriesPanel::CB_Algo(wxCommandEvent&){
 if(Validate()){
  int iM=dynamic_cast<wxSpinCtrl*>(FindWindow(SB_Count))->GetValue();
  int iTE=dynamic_cast<TonalErrorsCtl*>(FindWindow(TEC_Errors))->GetValue();
  wxListBox* lbp=dynamic_cast<wxListBox*>(FindWindow(LB_Algo));
  int iS=lbp->GetSelection();
  if(iS>=0 && iS<int(m_algolist.size()) && lbp->IsSelected(iS))
   set_series(m_algolist[iS](get_series(),iTE,iM));
 }
 m_set_focus();
}

void EnterSeriesPanel::m_control(bool _b){
 dynamic_cast<wxCheckBox*>(FindWindow(CBX_Oct))
   ->SetValue(!get_series().at(0).no_oct());
 unsigned i=get_series().FindDoubled();
 if(i){
  m_set_message(tone_occurs_twice_str(get_series().at(i).get_val()));
  if(_b)m_rctlp->set_pos(i);
  FindWindow(BTN_Contr)->Enable(true);
  return;
 }
 Series::TonalErrorPos ep=get_series().FindError(
  dynamic_cast<TonalErrorsCtl*>(FindWindow(TEC_Errors))->GetValue());
 m_set_message(tonal_error_str(ep.second));
 if(ep.second!=Series::TE_OK){
  if(_b)m_rctlp->set_pos(Series::transform_error_pos(ep));
  FindWindow(BTN_Contr)->Enable(true);
  return;
 }
 FindWindow(BTN_Contr)->Enable(false);
}

void EnterSeriesPanel::m_set_message(const wxString& _s){
 dynamic_cast<wxTextCtrl*>(FindWindow(ST_Message))->SetValue(_s);
}

BEGIN_EVENT_TABLE(EnterSeriesPanel,wxPanel)
 EVT_BUTTON(wxID_APPLY ,EnterSeriesPanel::CB_Apply )
 EVT_BUTTON(BTN_Audio  ,EnterSeriesPanel::CB_Audio )
 EVT_BUTTON(wxID_CANCEL,EnterSeriesPanel::CB_Close )
 EVT_CHECKBOX(CBX_Oct  ,EnterSeriesPanel::CB_Oct   )
 EVT_BUTTON(BTN_Contr  ,EnterSeriesPanel::CB_Contr )

 EVT_BUTTON(BTN_Algo   ,EnterSeriesPanel::CB_Algo  )
END_EVENT_TABLE()

EnterSeries::EnterSeries(wxWindow* _wp,wxWindowID _id,
 const SeriesValidator& _sv,const AlgorithmList& _al):
 wxFrame(_wp,_id,_("Edit Series"),wxDefaultPosition,wxDefaultSize,
         wxCAPTION|wxMINIMIZE_BOX|wxSYSTEM_MENU|wxRESIZE_BORDER){
 SetIcon(wxICON(edit));
 EnterSeriesPanel* espp=new EnterSeriesPanel(this,-1,_sv,_al);
 espp->GetSizer()->SetSizeHints(this);
 Fit();
 Show();
}
