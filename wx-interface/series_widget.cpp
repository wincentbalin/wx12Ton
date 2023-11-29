// wx-interface/series_widget.cpp

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

#include "series_widget.h"

#include "helper.h"
#include "series_io.h"
#include <wx/intl.h>
#include <wx/msgdlg.h>
#include <wx/dcclient.h>

bool SeriesValidator::Validate(wxWindow* _wp){
 unsigned i=get_source_series().FindDoubled();
 if(!i)return true;

 wxMessageBox(tone_occurs_twice_str(get_source_series().at(i).get_val()),
  _("invalid series"),wxOK|wxICON_ERROR,_wp);
 return false;
}

StaticSeriesCtl::StaticSeriesCtl(wxWindow* _wp,wxWindowID _id,
 const Series& _s,int _w,int _h):
 wxPanel(_wp,_id,wxDefaultPosition,wxSize(_w,_h)),
 SeriesGetSet(_s){}

StaticSeriesCtl::StaticSeriesCtl(wxWindow* _wp,wxWindowID _id,
 const SeriesValidator& _rv,int _w,int _h,long _iStyle):
 wxPanel(_wp,_id,wxDefaultPosition,wxSize(_w,_h),_iStyle|wxTAB_TRAVERSAL),
 SeriesGetSet(_rv.get_target_series()){
 SetValidator(_rv);
}

void StaticSeriesCtl::CB_Size(wxSizeEvent&){
 wxRect rWind(0,0,0,0);GetSize(&rWind.width,&rWind.height);
 wxRect rSeries=DrawSeries_Rect(rWind);
 if(rWind.width!=rSeries.width || rWind.height!=rSeries.height)
  SetSize(rSeries.width,rSeries.height);
 Refresh();
}

void StaticSeriesCtl::p_draw(int _i){
 wxRect rWind(0,0,0,0);GetSize(&rWind.width,&rWind.height);--rWind.width;
 wxRect rSeries=DrawSeries_Rect(rWind);

 wxPaintDC dc(this);
 dc.SetPen(*wxTRANSPARENT_PEN);
 dc.SetBrush(*wxWHITE_BRUSH);
 dc.DrawRectangle(0,0,rSeries.width+1,rSeries.height);
 if(_i>=0){
  wxRect rNote=DrawNote_Rect(_i,rWind);
  dc.SetBrush(*wxBLUE_BRUSH);
  dc.DrawRectangle(rNote.x+1,rNote.y,rNote.width-1,rNote.height);
 }

 DrawSeries(dc,get_series(),rSeries);
}

BEGIN_EVENT_TABLE(StaticSeriesCtl,wxPanel)
  EVT_PAINT    (StaticSeriesCtl::CB_Draw)
  EVT_SIZE     (StaticSeriesCtl::CB_Size)
END_EVENT_TABLE()

void SeriesCtl::CB_KeyDown(wxKeyEvent& _ke){
 Note& n=static_cast<Series&>(*this).at(m_iPos);
 switch(_ke.GetKeyCode()){
  case WXK_LEFT  :m_iPos=(m_iPos+11)%12;break;
  case WXK_RIGHT :m_iPos=(m_iPos+ 1)%12;break;
  case WXK_UP    :
   if(n.get_oct()<Note::OCT_MAX || n.get_val()<Note::TONE_MAX || n.no_oct())
    n.set((n-Note(0))+1,n.no_oct());break;
  case WXK_DOWN  :
   if(n.get_oct()>Note::OCT_MIN || n.get_val()>Note::TONE_MIN || n.no_oct())
    n.set((n-Note(0))-1,n.no_oct());break;
  case WXK_PRIOR :
   if(n.get_oct()<Note::OCT_MAX && !n.no_oct())n.set_oct(n.get_oct()+1);
   break;
  case WXK_NEXT :
   if(n.get_oct()>Note::OCT_MIN && !n.no_oct())n.set_oct(n.get_oct()-1);
   break;
  default:_ke.Skip();return;
 }
 Refresh();
 GetValidator()->TransferFromWindow();
}

void SeriesCtl::CB_LMouse(wxMouseEvent& _me){
 wxPoint p=_me.GetPosition();
 wxRect rWind(0,0,0,0);GetSize(&rWind.width,&rWind.height);
 wxRect rSeries=DrawSeries_Rect(rWind);
 if(!rSeries.Inside(p)){_me.Skip();return;}
 m_iPos=(p.x-rSeries.x)/(rSeries.width/12);
 Refresh();
}

BEGIN_EVENT_TABLE(SeriesCtl,StaticSeriesCtl)
  EVT_KEY_DOWN (SeriesCtl::CB_KeyDown)
  EVT_LEFT_DOWN(SeriesCtl::CB_LMouse)
END_EVENT_TABLE()
