// wx-interface/list_relat.h:
//  show the list of all series

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

#ifndef LIST_RELAT_H
#define LIST_RELAT_H

#include "series_widget.h"
#include "wx12Ton.h"
#include "algo/find_series.h"
#include "ids.h"
#include <wx/panel.h>
#include <wx/event.h>
#include <wx/sizer.h>
#include <wx/spinbutt.h>
#include <memory>

enum {MAX_SERIES_LL=50};

class DisplaySeries:public wxPanel{
 public:
  DisplaySeries(wxWindow*,wxWindowID,std::auto_ptr<SortedCS>,int);
  const SortedCS& GetSCS()const{return *m_scsp;}

  void CB_Scroll(wxScrollWinEvent&);
  void CB_BoxClicked(wxCommandEvent& _ce){
   m_iPos=GetScrollPos(wxVERTICAL)+_ce.GetId()-ID_RADIOBOX_0;}

  Series GetSeries(){return (*m_scsp)[m_iPos];}

  size_t GetLength()const{return m_iLen;}
  void SetLength(size_t _i){m_setlength(_i,GetSizer());m_scroll();}
 private:
  enum{ID_RADIOBOX_0=ID_DisplaySeries,
       ID_SERIES_0=ID_DisplaySeries+MAX_SERIES_LL};
  void m_scroll();
  void m_setlength(size_t,wxSizer*);
  DECLARE_EVENT_TABLE()

  size_t m_iPos,m_iLen;
  std::auto_ptr<SortedCS> m_scsp; 
};

class DisplaySeriesInfo:public wxPanel,public SeriesGetSet_base,
                        public has_playable_series{
 public:
  DisplaySeriesInfo(wxWindow*,wxWindowID,std::auto_ptr<SortedCS>,
   const SeriesValidator&);

# define CB(t) void CB_##t(wxCommandEvent&)
  CB(Apply ){if(Validate())GetValidator()->TransferFromWindow();}
  CB(Audio ){PlaySeries(m_dsp->GetSeries());}
  CB(Close ){GetParent()->Close();} //FIXME: bad to force parent to die
  void CB_ChangeL(wxSpinEvent& _se){m_dsp->SetLength(_se.GetPosition());}
# undef CB

  virtual void ReportStartPlay(){FindWindow(BTN_Audio)->Enable(false);}
  virtual void ReportStopPlay(){FindWindow(BTN_Audio)->Enable(true);}

  virtual const Series& get_series()const{m_sBuffer=m_dsp->GetSeries();
   return m_sBuffer;}

 private:
  enum{BTN_Audio=ID_DisplaySeriesInfo,SPN_Length};
  DisplaySeries* m_dsp;
  mutable Series m_sBuffer; //FIXME: bad coding style

  virtual void set_series(const Series&){}
  DECLARE_EVENT_TABLE()
};

#endif
