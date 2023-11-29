// wx-interface/flush_relat.cpp

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

#include "flush_relat.h"

#include "list_relat.h"
#include "tonal_widget.h"
#include "icon.h"
#include "pretty_sizer.h"
#include <wx/string.h>
#include <wx/intl.h>
#include <wx/statline.h>
#include <wx/stattext.h>

using namespace pretty_sizer;

FlushRelDlg::FlushRelDlg(wxWindow* _wp,wxWindowID _id,int _iErr):
 wxDialog(_wp,_id,_("Find series"),wxDefaultPosition){

 TonalErrorsCtl* tecp=new TonalErrorsCtl(this,TEC_Errors,_iErr);
 tecp->ConnectAll(this,&FlushRelDlg::CB_Set);

 Sizer s(new wxBoxSizer(wxVERTICAL));
 s << new wxStaticText(this,-1,
       _("List all normalized series, excluding\n"
         "Series, which contain tonal phrases,\n"
         "where")
       ) /Border(wxLEFT|wxRIGHT|wxTOP)/Expand(0,wxEXPAND)
   << tecp /Border(wxLEFT|wxRIGHT|wxTOP)/Expand(0,wxEXPAND);
 
 *s*new wxFlexGridSizer(2)*(Border(wxLEFT|wxRIGHT|wxTOP)|Expand(0,wxEXPAND));
  s.get<wxFlexGridSizer>()->AddGrowableCol(1);
  s << new wxStaticText(this,-1,_("Calculation steps: "))
    << new wxStaticText(this,ST_Steps,_T("")) /Expand(0,wxEXPAND)
    << new wxStaticText(this,-1,_("Valid Series: "))
    << new wxStaticText(this,ST_Series,_T("")) /Expand(0,wxEXPAND)
    << new wxStaticText(this,-1,_("Memory usage: "))
    << new wxStaticText(this,ST_Memory,_T("")) /Expand(0,wxEXPAND);
 !s;
 
 s << wxSize(0,0)/Expand(1,wxEXPAND)
   << new wxStaticLine(this,-1)/Border(wxLEFT|wxRIGHT|wxTOP)/Expand(0,wxEXPAND)
   << CreateButtonSizer(wxOK|wxCANCEL)
      /Border(wxALL)/Align(wxALIGN_CENTER_HORIZONTAL);

 SetSizerAndFit(s);
 CB_Set(Series::TE_DEFAULT);
}

void FlushRelDlg::CB_Set(int _i){
 wxString s;
 s.Printf(_("%.1f million steps"),float(crpCalcTable[_i].iSteps)/1000000.);
 FindWindow(ST_Steps)->SetLabel(s);
 s.Printf(_("%i series"),crpCalcTable[_i].iSeries);
 FindWindow(ST_Series)->SetLabel(s);
 s.Printf(_("%.1f MB"),float(crpCalcTable[_i].iSeries)*
                                     float(sizeof(CompactSeries))/1024./1024.);
 FindWindow(ST_Memory)->SetLabel(s);
 if(GetSizer())GetSizer()->Layout();
}

int FlushRelDlg::GetValue(){
 return dynamic_cast<TonalErrorsCtl*>(FindWindow(TEC_Errors))->GetValue();}

ShowRelatives::ShowRelatives(wxWindow* _wp,wxWindowID _id,
 std::auto_ptr<SortedCS> _scsp,const SeriesValidator& _sv):
 wxFrame(_wp,_id,_("List of all series"),wxDefaultPosition,wxDefaultSize,
         wxCAPTION|wxMINIMIZE_BOX|wxMAXIMIZE_BOX|wxSYSTEM_MENU|wxRESIZE_BORDER){

 SetIcon(wxICON(serieslist));
 new DisplaySeriesInfo(this,-1,_scsp,_sv);
 Fit();
 Show();
}

