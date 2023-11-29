// wx-interface/series_io.h:
//  various i/o routines for the classes defined in algo/series.h

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

#ifndef SERIES_IO_H
#define SERIES_IO_H

#include "algo/series.h"
#include "algo/midi_file.h"
#include <wx/dc.h>
#include <wx/file.h>
#include <wx/print.h>

wxRect DrawNote_Rect(unsigned,const wxRect&);
wxRect DrawSeries_Rect(const wxRect& _r);
void DrawSeries(wxDC&,const Series&,const wxRect&,bool=true);
void DrawNote  (wxDC&,const Note&  ,const wxRect&,bool=true);

class wxSeries_IO:public Series_IO{
 public:
  wxSeries_IO(wxFile& _f):m_f(_f){}
 private:
  wxFile& m_f;

  virtual bool write_char(char _c)throw(){return (m_f.Write(&_c,1)==1);}
  virtual ssize_t read_block(char* _cp,ssize_t _iS)throw(){
   return m_f.Read(_cp,_iS);}
};

class PrintSeries:public wxPrintout{
 public:
  PrintSeries(const Series&);

  virtual void GetPageInfo(int* _iMinP,int* _iMaxP,int* _iPF,int* _iPT){
   *_iMinP=*_iPF=1;*_iMaxP=*_iPT=2;}
  virtual bool HasPage(int _i){return (_i==1 || _i==2);}

  virtual bool OnPrintPage(int);
 private:
  Series m_s;
};

#endif
