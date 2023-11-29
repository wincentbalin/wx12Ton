// wx-interface/series_widget.h:
//  widgets to show and edit series

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

#ifndef SERIES_WIDGET_H
#define SERIES_WIDGET_H

#include "algo/series.h"
#include <wx/validate.h>
#include <wx/panel.h>
#include <wx/event.h>

class SeriesGetSet_base{
 public:
  virtual ~SeriesGetSet_base(){}
  virtual const Series& get_series()const=0;
  virtual void set_series(const Series& _s)=0;
};

class SeriesGetSet:public SeriesGetSet_base{
 public:
  explicit SeriesGetSet(const Series& _s){m_assign(_s);}
  virtual const Series& get_series()const{return m_s;}
  virtual void set_series(const Series& _s){m_assign(_s);}
 protected:
  operator Series&(){return m_s;} //use with care (you have to do limit_oct)
 private:
  void m_assign(const Series& _s){m_s=_s;m_s.limit_oct();}
  Series m_s;
};

class SeriesValidator:public wxValidator{
 public:
  explicit SeriesValidator(SeriesGetSet_base& _rgs):m_sgs(_rgs){}
  SeriesValidator(const SeriesValidator& _sv):
   wxValidator(),m_sgs(_sv.m_sgs){Copy(_sv);}
  virtual SeriesValidator* Clone()const{return new SeriesValidator(*this);}
 
  virtual bool TransferFromWindow(){
   set_target_series(get_source_series());return true;}
  virtual bool TransferToWindow(){
   set_source_series(get_target_series());return true;}
  virtual bool Validate(wxWindow*);

  const Series& get_target_series()const{return m_sgs.get_series();}
  void set_target_series(const Series& _s){return m_sgs.set_series(_s);}
  const Series& get_source_series()const{
   return dynamic_cast<SeriesGetSet_base&>(*m_validatorWindow).get_series();}
  void set_source_series(const Series& _s){
   return dynamic_cast<SeriesGetSet_base&>(*m_validatorWindow).set_series(_s);}

 private:
  SeriesGetSet_base& m_sgs;
};

class StaticSeriesCtl:public wxPanel,public SeriesGetSet{
 public:
  StaticSeriesCtl(wxWindow*,wxWindowID,const Series&,int=409,int=40);
  StaticSeriesCtl(wxWindow*,wxWindowID,const SeriesValidator&,int=409,int=40,
                  long=0);
  virtual void set_series(const Series& _s){
   SeriesGetSet::set_series(_s);Refresh();}

 protected:
  virtual void CB_Size(wxSizeEvent&);
  virtual void CB_Draw(wxPaintEvent&){p_draw(-1);}

  void p_draw(int);
 private:
  DECLARE_EVENT_TABLE()
};

class SeriesCtl:public StaticSeriesCtl{
 public:
  SeriesCtl(wxWindow* _wp,wxWindowID _id,const SeriesValidator& _rv,
   int _w=409,int _h=40):StaticSeriesCtl(_wp,_id,_rv,_w,_h,wxWANTS_CHARS),
                         m_iPos(0){}

  void set_pos(int _i){m_iPos=_i;Refresh();}

 private:
  int m_iPos;

  virtual void CB_Draw(wxPaintEvent&){p_draw(m_iPos);}
  virtual void CB_KeyDown(wxKeyEvent&);
  virtual void CB_LMouse(wxMouseEvent&);
  DECLARE_EVENT_TABLE()
};

#endif
