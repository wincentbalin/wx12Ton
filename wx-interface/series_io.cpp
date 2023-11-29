// wx-interface/series_io.cpp

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

#include "series_io.h"
#include "forms.h"
#include <wx/string.h>
#include <wx/intl.h>

// FIXME: fixes wxWidgets bug, that GetTextExtent does note return scaled values
inline void GetRealTextExtent(wxDC& _dc,const wxString& _s,
 wxCoord* _xp,wxCoord* _yp){
 double fsx=0.,fsy=0.;
 _dc.GetUserScale(&fsx,&fsy);
 _dc.GetTextExtent(_s,_xp,_yp);
 (*_xp)*=wxCoord(1./fsx);
 (*_yp)*=wxCoord(1./fsy);
}

wxRect DrawNote_Rect(unsigned _i,const wxRect& _r){
 return wxRect(_r.x+_i*(_r.width/12),_r.y,_r.width/12,_r.height);
}

wxRect DrawSeries_Rect(const wxRect& _r){
 wxRect r=DrawNote_Rect(0,_r);r.width=r.width*12;
 return r;
}

void DrawSeries(wxDC& _dc,const Series& _s,const wxRect& _r,bool _bColor){
 for(int i=0;i<12;++i)DrawNote(_dc,_s.at(i),DrawNote_Rect(i,_r),_bColor);
}

inline void draw_pure_note(wxDC& _dc,const wxRect& _r,bool _bHelpLine){
 wxBrush b=_dc.GetBrush();
 _dc.SetBrush(*wxBLACK_BRUSH);
 _dc.DrawEllipse(_r.x-_r.width/2,_r.y-_r.height/2,_r.width,_r.height);
 _dc.SetBrush(b);
 if(_bHelpLine)_dc.DrawLine(_r.x-_r.width,_r.y,_r.x+_r.width,_r.y);
}

inline void draw_pure_sign(wxDC& _dc,const wxRect& _r,int _iSig){
#define  ARG4(a,b,c,d) _r.x+_r.width*a,_r.y+_r.height*b,\
                       _r.x+_r.width*c,_r.y+_r.height*d
#define ARG4B(a,b,c,d) _r.x+_r.width*a,_r.y+_r.height*b,\
                            _r.width*c,     _r.height*d
 switch(_iSig){
  case -1: //flat
   _dc.DrawLine(ARG4(-1/2,-11/10,-1/2,5/11));
   _dc.DrawEllipticArc(ARG4B(-2/2,-4/10,1,8/10),-89,90);
   break;
  case +0: //natural
   _dc.DrawLine(ARG4(-1/2,-11/11,-1/2,  4/11));
   _dc.DrawLine(ARG4(   0, -5/11,   0, 11/11));
   _dc.DrawLine(ARG4(-1/2, -5/11,   0, -5/11));
   _dc.DrawLine(ARG4(-1/2,  4/11,   0,  4/11));
   break;
  case +1: //sharp
   _dc.DrawLine(ARG4( -1/2,    -1,-7/10,    1));
   _dc.DrawLine(ARG4(    0,    -1,-2/10,    1));
   _dc.DrawLine(ARG4( -9/11,-4/11, 4/11,-4/11));
   _dc.DrawLine(ARG4(-10/11, 4/11, 3/11, 4/11));
   break;
 }
#undef ARG4
#undef ARG8
}

inline void draw_pure_octave(wxDC& _dc,const wxPoint& _p,wxCoord _h,
 Note::Octave _o,bool _bColor){
 wxString s(_T("0"));
 if(_bColor){
  if(_o<0){
   _dc.SetTextForeground(*wxRED);
   s.Printf(_T("%i"),-_o); 
  }else if(_o>0){
   _dc.SetTextForeground(*wxGREEN);
   s.Printf(_T("%i"),_o); 
  }else _dc.SetTextForeground(*wxBLACK);
 }else{
  _dc.SetTextForeground(*wxBLACK);
  s.Printf(_T("%+i"),_o); 
 }
  
 wxFont f=*wxSMALL_FONT;
 f.SetPointSize(_h);
 _dc.SetFont(f);
 wxCoord w,h;GetRealTextExtent(_dc,s,&w,&h);
 _dc.DrawText(s,_p.x,_p.y-h);
}

void DrawNote(wxDC& _dc,const Note& _n,const wxRect& _r,bool _bColor){
 static const int iNoteSign[12]={+0,+1,+0,-1,+0,+0,+1,+0,+1,+0,-1,+0};
 static const int iNotePos[12] ={ 0, 0, 1, 2, 2, 3, 3, 4, 4, 5, 6, 6};

 int h=_r.height/7;

 wxPen pen=*wxBLACK_PEN;
 pen.SetStyle(wxSOLID);
 pen.SetWidth((h>7)?h/7:1);
 _dc.SetPen(pen);
 _dc.SetBrush(*wxTRANSPARENT_BRUSH);
 _dc.BeginDrawing();

 // draw left/right boundaries
 _dc.DrawLine(_r.x         ,_r.y+h,_r.x         ,_r.y+h*5);
 _dc.DrawLine(_r.x+_r.width,_r.y+h,_r.x+_r.width,_r.y+h*5);

 // draw lines
 if(h<4){pen.SetStyle(wxDOT);_dc.SetPen(pen);}
 for(int i=0;i<5;++i)_dc.DrawLine(_r.x,_r.y+h*(i+1),_r.x+_r.width,_r.y+h*(i+1));
 pen.SetStyle(wxSOLID);_dc.SetPen(pen);

 // draw note
 int x=_r.x+_r.width/2+_r.width/15;
 int y=_r.y+h*6-h*iNotePos[_n.get_val()]/2;
 int w=int(_r.height*1.3/7.);
 draw_pure_note(_dc,wxRect(x,y,w,h),iNotePos[_n.get_val()]==0);
 draw_pure_sign(_dc,wxRect(x-w,y,w*3/4,h),iNoteSign[_n.get_val()]);
 if(!_n.no_oct())
  draw_pure_octave(_dc,wxPoint(_r.x,_r.y+_r.height),h,_n.get_oct(),_bColor);
 
 _dc.EndDrawing();
}

PrintSeries::PrintSeries(const Series& _s):
 wxPrintout(_T("wx12Ton: ")+wxString(_s.get_string().c_str(),wxConvLocal)),
 m_s(_s){}

bool PrintSeries::OnPrintPage(int _iPage){
 Forms f(m_s);
 wxDC& dc=*GetDC();
 wxFont font=*wxNORMAL_FONT;

 wxRect r;
 int fw=0,fh=0;
 { //set r,fw,fh,font
  int w,h;GetPageSizePixels(&w,&h);
  int wpi,hpi;GetPPIPrinter(&wpi,&hpi);
  
  { //scale to at least 400ppi (needed, since postscript defaults to 72ppi)
   enum {MIN_PPI=400};
   float fsx=1.,fsy=1.;
   if(wpi<MIN_PPI){
    fsx/=float(MIN_PPI/wpi+1);
    w*=(MIN_PPI/wpi+1);wpi*=(MIN_PPI/wpi+1);
   }
   if(hpi<MIN_PPI){
    fsy/=float(MIN_PPI/hpi+1);
    h*=(MIN_PPI/hpi+1);hpi*=(MIN_PPI/hpi+1);
   }
   dc.SetUserScale(fsy,fsy);
  }

  h=(h-2*hpi)/12;    //1 inch border
  int wold=w;
  w=(w-2*wpi-w/12);  //1 inch + 1 note border (1 note is a good guess for
                     // for the upcoming text width calculation)
  //set ratio to about 1:1
  if(float(w/12)/float(wpi)<float(h)/float(hpi))
   h=int(float(w/12)/float(wpi)*float(hpi));
  if(float(w/12)/float(wpi)>float(h)/float(hpi))
   w=int(float(h/12)/float(hpi)*float(wpi));

  //text width calcuation
  font.SetPointSize(h/4);
  dc.SetFont(font);
  for(int i=0;i<12;++i){
   int fw2=0,fh2=0;
   GetRealTextExtent(dc,f.GetPText(i,false,true )+_T(" "),&fw2,&fh2);
   fw=((fw2>fw)?fw2:fw);fh=((fh2>fh)?fh2:fh);
   GetRealTextExtent(dc,f.GetPText(i,false,false)+_T(" "),&fw2,&fh2);
   fw=((fw2>fw)?fw2:fw);fh=((fh2>fh)?fh2:fh);
  }
  //replace guessed width by exact one
  w=wold-2*wpi-fw;
 
  //set series rect
  r=wxRect(wpi+fw,hpi,w,h);
 }

 for(int i=0;i<12;++i){
  dc.BeginDrawing();
  dc.SetFont(font);
  dc.DrawText(f.GetPText(i,false,_iPage==2)+_T(" "),r.x-fw,r.y+(r.height-fh)/2);
  dc.EndDrawing();
  DrawSeries(dc,f.GetPSeries(i,false,_iPage==2),r,false);
  r.y+=r.height;
 }
 return true;
}
