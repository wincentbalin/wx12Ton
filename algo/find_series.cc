// algo/find_series.cc

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

#include "find_series.h"

const CalcResult crpCalcTable[]={
 {19958099,9985920},//0
 { 5412489,1107917},//TE_FOURTH
 {16177639,7093480},//TE_CHORD
 { 4281661, 655689},//TE_FOURTH|TE_CHORD
 {19181297,9331114},//TE_EQ_INTERV
 { 5194150,1000579},//TE_EQ_INTERV|TE_FOURTH
 {15524673,6589114},//TE_EQ_INTERV|TE_CHORD
 { 4113533, 583559},//TE_EQ_INTERV|TE_FOURTH|TE_CHORD==TE_DEFAULT
 { 6929035,  40766},//TE_ALL_INTERV
 {       0,      0},//TE_ALL_INTERV|TE_FOURTH
 { 5931236,  26439},//TE_ALL_INTERV|TE_CHORD
 {       0,      0},//TE_ALL_INTERV|TE_FOURTH|TE_CHORD
 { 6929035,  40766},//TE_ALL_INTERV|TE_EQ_INTERV
 {       0,      0},//TE_ALL_INTERV|TE_EQ_INTERV|TE_FOURTH
 { 5931236,  26439},//TE_ALL_INTERV|TE_EQ_INTERV|TE_CHORD
 {       0,      0} //TE_ALL_INTERV|TE_EQ_INTERV|TE_FOURTH|TE_CHORD
};

const Series SortedCS::sFirstValidSeries  ("ABCDEFGHIJKL"); // 73686780563
const Series SortedCS::sFirstInvalidSeries("AGFLKJEDHBCI"); //402419147504

CompactSeries::operator Series()const{
 Series s;
 s.set_no_oct();
 for(int i=0;i<12;++i)s.at(i).set_val(get_val_at(i));
 return s;
}

template <int I>
inline Note::NoteDiff at_helper(const uint8_t* _cp){
 if(I%2)return (_cp[(I-1)/2]&0xf0)>>4;
 else   return (_cp[(I-2)/2]&0x0f);
}

template <>
inline Note::NoteDiff at_helper<0>(const uint8_t*){return Note::TONE_MIN;}

template <>
inline Note::NoteDiff at_helper<11>(const uint8_t* _cp){
# define AH(x) (uint16_t(1)<<at_helper<x>(_cp))
 switch(AH(0)|AH(1)|AH(2)|AH(3)|AH(4)|AH(5)|AH(6)|AH(7)|AH(8)|AH(9)|AH(10)){
  case 0xffd:return Note::TONE_MIN+ 1;
  case 0xffb:return Note::TONE_MIN+ 2;
  case 0xff7:return Note::TONE_MIN+ 3;
  case 0xfef:return Note::TONE_MIN+ 4;
  case 0xfdf:return Note::TONE_MIN+ 5;
  case 0xfbf:return Note::TONE_MIN+ 6;
  case 0xf7f:return Note::TONE_MIN+ 7;
  case 0xeff:return Note::TONE_MIN+ 8;
  case 0xdff:return Note::TONE_MIN+ 9;
  case 0xbff:return Note::TONE_MIN+10;
  case 0x7ff:return Note::TONE_MIN+11;
 }
# undef AH
 return Note::TONE_MIN;
}

Note::NoteDiff CompactSeries::get_val_at(int _i)const{
# define CASE(x) case x:return at_helper<x>(m_cp);
 switch(_i){
 CASE(1)CASE(2)CASE(3)CASE(4)CASE(5)CASE(6)CASE(7)CASE(8)CASE(9)CASE(10)CASE(11)
 }
# undef CASE
 return at_helper<0>(m_cp);
}

void SortedCS::m_step(){
 if(!m_s.step_find_valid_forw(m_iTE).first){
  if(m_s.IsNormalized())m_vcs.push_back(m_s);
  m_s.next_permutation();
 }
#if 0
 unsigned i=m_s.FindError(m_iTE).first;
 if(!i){
  if(m_s.IsNormalized())m_vcs.push_back(m_s);
  m_s.next_permutation();
 }else m_s.next_permutation(i);
#endif
}
