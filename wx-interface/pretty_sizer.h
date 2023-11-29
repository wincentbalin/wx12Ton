// wx-interface/pretty_sizer.h:
//  an interface to wxSizer, the wx people were not capable to write ;)

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

#ifndef PRETTY_SIZER
#define PRETTY_SIZER

#include <wx/sizer.h>
#include <stack>
//std::pair
#include <utility>

//Note: All the template and STL stuff is only for convenience, the interface
//      can also be implemented without it.

namespace pretty_sizer{

class Options{
 public:
  Options(int _i1=0,int _i2=0,int _i3=0,wxObject* _wop=NULL):
   i1(_i1),i2(_i2),i3(_i3),wop(_wop){}

  template <typename T>
  void Apply(wxSizer* _sp,const T& _t)const{
   _sp->Add(_t,(i1<0)?0:i1,i2,(i3<0)?0:i3,wop);
  }

  Options operator |(const Options& _o)const{
   return Options((_o.i1<0)?i1:_o.i1,
                  i2|_o.i2,
                  (_o.i3<0)?i3:_o.i3,
                  (!_o.wop)?wop:_o.wop
                 );}

 private:
  int i1,i2,i3;
  wxObject* wop;
};

template <>
inline void Options::Apply(wxSizer* _sp,const wxSize& _ws)const{
 _sp->Add(_ws.GetWidth(),_ws.GetHeight(),
            (i1<0)?0:i1,i2,(i3<0)?0:i3,wop);
}

struct Align :Options{explicit Align (        int _i2             )
 :Options( -1,_i2, -1){}};
struct Expand:Options{explicit Expand(int _i1,int _i2=0           ) 
 :Options(_i1,_i2, -1){}};
struct Border:Options{explicit Border(        int _i2  ,int _i3=10)
 :Options( -1,_i2,_i3){}};

template <typename T>
inline std::pair<T,Options> operator /(const T& _t,const Options& _o){
 return std::make_pair(_t,_o);
}
template <typename T>
inline std::pair<T,Options> operator /(const std::pair<T,Options>& _p,
 const Options& _o){
 return (_p.first/(_o|_p.second));
}

class Sizer{
 public:
  Sizer(wxSizer* _sp){**this*_sp;}
  template <typename T>
  T* get()const{return dynamic_cast<T*>(m_l.top().first);}
  operator wxSizer*()const{return get<wxSizer>();}

  Sizer& operator *(){
   m_l.push(std::pair<wxSizer*,Options>(NULL,Options()));return *this;}
  Sizer& operator *(wxSizer* _sp){m_l.top().first=_sp;return *this;}
  const Sizer& operator *(const Options& _o){m_l.top().second=_o;return *this;}

  template <typename T>
  const Sizer& operator <<(const T& _t)const{return (*this << _t/Options());}
  template <typename T>
  const Sizer& operator << (const std::pair<T,Options>& _p)const{
   _p.second.Apply(m_l.top().first,_p.first);return *this;}

  void operator !(){
   std::pair<wxSizer*,Options> p=m_l.top();
   m_l.pop();
   (*this) << p;
  }

 private:
  std::stack<std::pair<wxSizer*,Options> > m_l;
};

}// namespace pretty_sizer

#endif
