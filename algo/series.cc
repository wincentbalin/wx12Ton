// algo/series.cc

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

#include "series.h"

#include <algorithm> //min_element,swap,find_end
#include <iterator>
//uint16_t
#include <stdint.h>

std::string Series::get_string()const{
 std::string s;
 for(int i=0;i<12;++i){
  if(!at(i).no_oct())s+=at(i).get_oct()-Note::OCT_MIN+'a';
  s+=at(i).get_val()+'A';
 }
 return s;
}

Series Series::GetInversion(const Note::NoteDiff& _nd)const{
 Series s;
 s.at(0)=at(0)+_nd;
 for(int i=1;i<12;++i){
  s.at(i)=s.at(i-1)-(at(i)-at(i-1));
  if(at(i).no_oct()||at(i-1).no_oct())s.at(i).set_no_oct();
 }
 return s;
}

Series Series::GetNormalized()const{
 Series sp[4];

 sp[0]=GetBasicForm(Note(0)-at(0));
 sp[1]=GetRetrograde(Note(0)-at(11));
 sp[2]=sp[0].GetInversion();
 sp[3]=sp[1].GetInversion();
 
 return *std::min_element(sp,sp+4);
}

unsigned Series::transform_error_pos(const TonalErrorPos& _p){
 switch(_p.second){
  case TE_ALL_INTERV:
  case TE_FOURTH    :return _p.first-1;
  case TE_CHORD     :return _p.first-2;
  case TE_EQ_INTERV :return _p.first-3;
  case TE_OK:break;
 }
 return _p.first;
}

Series::TonalErrorPos Series::FindError(int _i)const{
 if(!_i)return TonalErrorPos(0,Series::TE_OK);

 std::pair<Note::NoteDiff,bool> pI[3];
 uint16_t uInt=0;
 for(int i=-2;i<11;++i){
  pI[0]=pI[1];pI[1]=pI[2];
  if(i<9){
   pI[2].first=Note::modulo(at(i+2).get_val()-at(i+3).get_val());
   pI[2].second=(pI[2].first>6);
   if(pI[2].second)pI[2].first=12-pI[2].first;
   if(i<0)continue;
  }

  if(_i&Series::TE_ALL_INTERV){
#  if 0
   if(uInt&(uint16_t(1)<<(pI[0].first+((pI[0].first==6)?0:7))))
    return TonalErrorPos(i+1,Series::TE_ALL_INTERV);
   if(uInt&(uint16_t(1)<<pI[0].first))uInt|=(uint16_t(1)<<(pI[0].first+7));
   else                               uInt|=(uint16_t(1)<<pI[0].first);
#  else
   uint16_t uIntOld=uInt;
   if(uIntOld==(uInt|=(uint16_t(1)<<pI[0].first)))
    if(pI[0].first==6 || uIntOld==(uInt|=(uint16_t(1)<<(pI[0].first+7))))
     return TonalErrorPos(i+1,Series::TE_ALL_INTERV);
#  endif
  }else{
   if(_i&Series::TE_FOURTH && pI[0].first==5)
    return TonalErrorPos(i+1,Series::TE_FOURTH);
   if(_i&Series::TE_EQ_INTERV && i<9 && pI[0].first==pI[1].first &&
      pI[1].first == pI[2].first)
    return TonalErrorPos(i+3,Series::TE_EQ_INTERV);
  }
  if(_i&Series::TE_CHORD && i<10 && pI[0].second==pI[1].second &&
     ((pI[0].first==3 && pI[1].first==4) || (pI[0].first==4 && pI[1].first==3)))
   return TonalErrorPos(i+2,Series::TE_CHORD);
 }
 return TonalErrorPos(0,Series::TE_OK);
}

unsigned Series::FindDoubled()const{
 uint16_t v=0;
 for(int i=0;i<12;++i){
# if 0
  if(v&(uint16_t(1)<<(at(i).get_val())))return i;
  v|=uint16_t(1)<<(at(i).get_val());
# else
  uint16_t vold=v;
  if(vold==(v|=uint16_t(1)<<(at(i).get_val())))return i;
# endif
 }
 return 0;
}

bool Series::IsNormalized()const{
 if(at(0).get_val())return false;
 Series s=GetRetrograde(Note(0)-at(11));
 return !(s<(*this) || GetInversion()<(*this) || s.GetInversion()<(*this));
}

void Series::m_assign(const std::string& _s){
 std::string::const_iterator si=_s.begin();

 for(int i=0;i<12;++i){
  if((*si)>='a' && (*si)<='a'+Note::OCT_MAX-Note::OCT_MIN){
   at(i).set_oct(Note::Octave((*si)-'a')+Note::OCT_MIN);
   ++si;
  }else at(i).set_no_oct();
  at(i).set_val(Note::Tone((*si)-'A'));
  ++si;
 }
}

bool Series::m_next_permutation_at(int _i){
 int iMin=-1;
 for(int i=_i+1;i<12;++i)if((iMin<0 || at(i)<at(iMin)) && at(_i)<at(i))iMin=i;
 if(iMin<0)return false;
 std::swap(at(_i),at(iMin));
 std::sort(begin()+_i+1,end());
 return true;
}

bool Series::m_prev_permutation_at(int _i){
 int iMax=-1;
 for(int i=_i+1;i<12;++i)if((iMax<0 || at(iMax)<at(i)) && at(i)<at(_i))iMax=i;
 if(iMax<0)return false;
 std::swap(at(_i),at(iMax));
 std::sort(std::reverse_iterator<Note*>(end()),
           std::reverse_iterator<Note*>(begin()+_i+1));
 return true;
}
