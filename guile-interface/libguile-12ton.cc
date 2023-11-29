// guile-interface/libguile-12ton.cc

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

#include "enc_scm.h"
#include "guile_helper.h"
#include "libguile-12ton.h"
#include "algo/series.h"
#include "algo/midi_file.h"

const static struct EmptySeries_t:public Series{
 EmptySeries_t(){for(int i=0;i<12;++i)at(i)=Note(0,true);}
} an_empty_series;

class libc_Series_IO:private Series_IO{
 public:
  libc_Series_IO(const enc_scm& _scm):m_scm(_scm){}

  Series port_read(){
   std::pair<bool,Series> p=read();
   if(!p.first)return an_empty_series;
   return p.second;
  }
  void port_write(const Series& _s){write(_s);}
 private:
  virtual ssize_t read_block(char* _cp,ssize_t _iS)throw(){
   for(ssize_t i=0;i<_iS;++i){
    int c=m_scm.port_read_char();
    if(c==enc_scm::P_EOF)return i;
    _cp[i]=char(c);
   }
   return _iS;
  }
  virtual bool write_char(char _c)throw(){
   m_scm.port_write_char(_c);return true;}

  enc_scm m_scm;
};

def_guile_init(INIT_12TON)

// note related functions
#define _n (_1.get<Note>())
f2(note_new       ,1){
 return Note(_1.get_nd(),_2.is_unbound()?false:_2.get<bool>());}
f2(note_val       ,1){
 if(_2.is_unbound())return static_cast<int>(_n.get_val());
 Note N=_n;N.set_val(_2.get_tone());return N;
}
f2(note_oct       ,1){
 if(_2.is_unbound())return static_cast<int>(_n.get_oct());
 Note N=_n;N.set_oct(_2.get_oct());return N;
}
f1(note_has_oct,0){return !_n.no_oct();}
f1(note_set_no_oct,0){Note N=_n;N.set_no_oct();return N;}
f1(interval_modulo,0){return static_cast<int>(Note::modulo(_1.get_nd()));}
f2(note_interval  ,0){return static_cast<int>(_n-_2.get<Note>());}
f2(note_transpose ,0){return _n+_2.get_nd();}
f2(note_is_less   ,0){return _n< _2.get<Note>();}
f2(note_is_equal  ,0){return _n==_2.get<Note>();}
f2(note_is_eq     ,0){return _n.full_eq(_2.get<Note>());}
#undef _n

// series related functions
#define _s (_1.get<Series>())
f1(series_new             ,1){
 if(_1.is_unbound())return static_cast<const Series&>(an_empty_series);
//FIXME: needs error checking!
 return Series(_1.get<std::string>());
}
f1(series_to_string       ,0){return _s.get_string();}
f3(series_at              ,1){
 if(_3.is_unbound())return _s.at(_2.get_pos());
 Series S=_s;S.at(_2.get_pos())=_3.get<Note>();return S;
}
f2(series_basic_form      ,0){return Series(_s.GetBasicForm (_2.get_nd()));}
f2(series_retrograde      ,0){return Series(_s.GetRetrograde(_2.get_nd()));}
f2(series_inversion       ,0){return Series(_s.GetInversion (_2.get_nd()));}
f1(series_normalized      ,0){return Series(_s.GetNormalized());}
f2(series_next_permutation,1){
 Series S=_s;S.next_permutation((_2.is_unbound())?10:(_2.get_pos()));return S;}
f2(series_prev_permutation,1){
 Series S=_s;S.prev_permutation((_2.is_unbound())?10:(_2.get_pos()));return S;}
f2(series_find_valid_forw ,0){
 int i=_2.get_te();
 Series S=_s;
 while(S.step_find_valid_forw(i).first);
 return S;
}
f2(series_find_valid_backw,0){
 int i=_2.get_te();
 Series S=_s;
 while(S.step_find_valid_backw(i).first);
 return S;
}
f2(series_find_error      ,0){return _s.FindError(_2.get_te());}
f1(series_find_doubled    ,0){return static_cast<int>(_s.FindDoubled());}
f1(error_pos_transform    ,0){
 return static_cast<int>(Series::transform_error_pos(
  _1.get<Series::TonalErrorPos>()));
}
f1(series_read            ,0){return libc_Series_IO(_1).port_read();}
f2(series_write           ,0){
 libc_Series_IO(_2).port_write(_s);return enc_scm();}
f1(series_is_normalized   ,0){return _s.IsNormalized();}
f2(series_is_less         ,0){return _s< _2.get<Series>();}
f2(series_is_equal        ,0){return _s==_2.get<Series>();}
f2(series_is_eq           ,0){return _s.full_eq(_2.get<Series>());}
#undef _s

change_guile_name(note_has_oct ,"note-oct?"  )
change_guile_name(note_is_less ,"note-less?" )
change_guile_name(note_is_equal,"note-equal?")
change_guile_name(note_is_eq   ,"note-eq?"   )

change_guile_name(series_is_normalized,"series-normalized?")
change_guile_name(series_is_less      ,"series-less?"      )
change_guile_name(series_is_equal     ,"series-equal?"     )
change_guile_name(series_is_eq        ,"series-eq?"        )

