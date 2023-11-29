// guile-interface/enc_scm.cc

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
#include <list>

#define T(x) template<> void enc_scm::m_assign<x>(const x& _t)
T(scm_type){m_scm=_t;}
T(bool){m_scm=SCM_BOOL(_t);}
T(char){m_scm=SCM_MAKE_CHAR(_t);}
T(int){m_scm=scm_int2num(_t);}
T(std::string){m_scm=scm_mem2string(_t.c_str(),_t.length());}

T(Note){
 m_scm=enc_scm(int(
  _t.get_val()+(_t.no_oct()?0:(_t.get_oct()-OCT_NONE))*NOTE_DIV));
}
T(Series){
 m_make_vector(12);
 for(int i=0;i<12;++i)m_vector_set(i,enc_scm(_t.at(i)));
}
T(Series::TonalError){
 switch(_t){
#define X(x) case Series::TE_##x:m_assign<std::string>(#x);break;
 X(OK)X(FOURTH)X(CHORD)X(EQ_INTERV)X(ALL_INTERV)
#undef X
 }
}
T(Series::TonalErrorPos){
 m_make_pair(enc_scm(int(_t.first)),enc_scm(_t.second));}
#undef T


#define T(x) template<> x enc_scm::get<x>()const
#define S(x) #x " enc_scm::get<" #x ">()const"
T(bool){return SCM_NFALSEP(m_scm);}
T(char){m_assert(SCM_CHARP(m_scm),SCM_ARGn,S(char));return SCM_CHAR(m_scm);}
T(int){return scm_num2int(m_scm,SCM_ARGn,S(int));}
T(std::string){
 m_assert(SCM_STRINGP(m_scm),SCM_ARGn,S(std::string));
 return SCM_STRING_CHARS(m_scm);
}

T(Note){
 int iN=get<int>();
 int iO=iN/NOTE_DIV+OCT_NONE;iN%=NOTE_DIV;
 m_assert_is_tone(iN,SCM_ARG1,S(Note));
 if(iO!=OCT_NONE)m_assert_is_oct(iO,SCM_ARG1,S(Note));
 Note N;N.set_val(iN);
 if(iO==OCT_NONE)N.set_no_oct();else N.set_oct(iO);
 return N;
}
T(Series){
 m_assert(m_vector_p(),SCM_ARG1,S(Series));
 m_assert(m_vector_length()==12,SCM_ARG1,S(Series));
 Series S;
 for(int i=0;i<12;++i)S.at(i)=m_vector_ref(i).get<Note>();
 return S;
}
T(Series::TonalErrorPos){
 m_assert(m_pair_p(),SCM_ARGn,S(Series::TonalErrorPos));
 return Series::TonalErrorPos(m_pair_1st().get<int>(),
           Series::TonalError(m_pair_2nd().m_get_te_t(false)));
}
T(std::list<enc_scm>){
 m_assert(m_list_p(),SCM_ARGn,S(std::list<enc_scm>));
 int iL=m_list_length();
 std::list<enc_scm> l;
 for(int i=0;i<iL;++i)l.push_back(m_list_ref(i));
 return l;
}
#undef T

Note::NoteDiff enc_scm::get_nd()const{
 int iN=get<int>();
//FIXME m_assert(?,SCM_ARGn,"Note::Octave enc_scm::get_oct()const");
 return iN;
}

Note::Tone enc_scm::get_tone()const{
 int iT=get<int>();
 m_assert_is_tone(iT,SCM_ARGn,"Note::Tone enc_scm::get_tone()const");
 return iT;
}

Note::Octave enc_scm::get_oct()const{
 int iO=get<int>();
 m_assert_is_oct(iO,SCM_ARGn,"Note::Octave enc_scm::get_oct()const");
 return iO;
}

int enc_scm::get_pos()const{
 int iP=get<int>();
 m_assert(iP>=0 && iP<12,SCM_ARGn,"int enc_scm::get_pos()const");
 return iP;
}

int enc_scm::get_te()const{
 int iTE=Series::TE_OK;
 m_assert(m_list_p(),SCM_ARGn,"int enc_scm::get_te()const");
 int iL=m_list_length();
 for(int i=0;i<iL;++i)iTE|=enc_scm(m_list_ref(i)).m_get_te_t(true);
 return iTE;
}

//member fu's
int enc_scm::m_get_te_t(bool _bDef)const{
#define X(x)\
  if(enc_scm(scm_equal_p(enc_scm(std::string(#x)),m_scm)).get<bool>())\
   return Series::TE_##x;
 X(OK)X(FOURTH)X(CHORD)X(EQ_INTERV)X(ALL_INTERV)if(_bDef)X(DEFAULT)
#undef X
 m_assert(false,SCM_ARGn,"int enc_scm::m_get_te_t()const");
 return Series::TE_OK;
}

void enc_scm::m_make_vector(int i){m_scm=scm_make_vector(enc_scm(i),enc_scm());}
void enc_scm::m_vector_set(int i,const enc_scm& _1){
 scm_vector_set_x(m_scm,enc_scm(i),_1);}
int enc_scm::m_vector_length()const{
 return enc_scm(scm_vector_length(m_scm)).get<int>();}
enc_scm enc_scm::m_vector_ref(int i)const{
 return scm_vector_ref(m_scm,enc_scm(i));}

bool enc_scm::m_list_p()const{return enc_scm(scm_list_p(m_scm)).get<bool>();}
int enc_scm::m_list_length()const{return enc_scm(scm_length(m_scm)).get<int>();}
enc_scm enc_scm::m_list_ref(int i)const{return scm_list_ref(m_scm,enc_scm(i));}

void enc_scm::m_make_pair(const enc_scm& _1,const enc_scm& _2){
 m_scm=scm_cons(_1,_2);}
enc_scm enc_scm::m_pair_1st()const{return SCM_CAR(m_scm);}
enc_scm enc_scm::m_pair_2nd()const{return SCM_CDR(m_scm);}

int enc_scm::port_read_char(){
 m_assert(scm_input_port_p(m_scm),SCM_ARGn,"int enc_scm::port_read_char()");
 enc_scm _scm=scm_read_char(m_scm);
 
 if(enc_scm(scm_eof_object_p(_scm)).get<bool>())return P_EOF;
 else return _scm.get<char>();
}

void enc_scm::port_write_char(char _c){
 m_assert(scm_output_port_p(m_scm),SCM_ARGn,
          "void enc_scm::port_write_char(char)");
 scm_write_char(enc_scm(_c),m_scm);
}

