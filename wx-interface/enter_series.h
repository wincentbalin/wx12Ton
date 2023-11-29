// wx-interface/enter_series.h:
//  the enter series window

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

#ifndef ENTER_SERIES_H
#define ENTER_SERIES_H

#include "series_widget.h"
#include "wx12Ton.h"
#include "ids.h"
#include "algorithms.h"
#include <wx/panel.h>
#include <wx/string.h>
#include <wx/frame.h>
#include <wx/event.h>

class EnterSeriesPanel:public wxPanel,public SeriesGetSet,
 public has_playable_series{
 public:
  EnterSeriesPanel(wxWindow*,wxWindowID,const SeriesValidator&,
                   const AlgorithmList&);
  
# define CB(t) void CB_##t(wxCommandEvent&)
# define CBV(t,x) void CB_##t(wxCommandEvent&){if(Validate()){x;}m_set_focus();}
  CB(Close ){GetParent()->Close();} //FIXME: bad to force parent to die
  CB(Contr ){m_control(true);m_set_focus();}
  CB(Oct   );
  CBV(Apply ,GetValidator()->TransferFromWindow())
  CBV(Audio ,PlaySeries(get_series()))
  CB(Algo);
# undef CB
# undef CBV

  virtual void ReportStartPlay(){FindWindow(BTN_Audio)->Enable(false);}
  virtual void ReportStopPlay (){FindWindow(BTN_Audio)->Enable(true );}
  void CB_TonalChange(int){m_control();m_set_focus();}
  virtual void set_series(const Series& _s){
   SeriesGetSet::set_series(_s);m_control();
   TransferDataToWindow();}

 private:
  void m_control(bool=false);

  void m_set_focus(){m_rctlp->SetFocus();}
  void m_set_message(const wxString&);
  DECLARE_EVENT_TABLE()

  enum {BTN_Audio=ID_EnterSeriesPanel,TEC_Errors,CBX_Oct,BTN_Contr,
        BTN_Algo,LB_Algo,SB_Count,ST_Message};
  SeriesCtl* m_rctlp;
  AlgorithmList m_algolist;
};

class EnterSeries:public wxFrame{
 public:
  EnterSeries(wxWindow*,wxWindowID,const SeriesValidator&,const AlgorithmList&);
};

#endif
