// guile-interface/enc_scm.h:
//  encapsulate SCM data to convert easliy to C++ classes

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

#ifndef ENC_SCM_H
#define ENC_SCM_H

#include "algo/series.h"
#include <libguile.h>
#include <string>

typedef SCM scm_type;
enum {OCT_NONE=Note::OCT_MIN-1,NOTE_DIV=16};

//encapsulate SCM
class enc_scm{
 public:
  enc_scm():m_scm(SCM_UNDEFINED){}
  template <typename T> enc_scm(const T& _t){m_assign(_t);}

  operator const scm_type&()const{return m_scm;}

  template <typename T> T get()const;

  Note::NoteDiff get_nd  ()const; 
  Note::Tone     get_tone()const;
  Note::Octave   get_oct ()const; 
  int get_pos()const;
  int get_te()const;

  bool is_unbound()const{return SCM_UNBNDP(m_scm);}

  enum {P_EOF=-1000};
  int port_read_char();
  void port_write_char(char);

 private:
  template <typename T> void m_assign(const T&);

  void m_assert(bool _b,int _i,const std::string& _s)const{
   SCM_ASSERT(_b,m_scm,_i,_s.c_str());}
  void m_assert_is_oct(int _iO,int _iArg,const std::string& _s)const{
   m_assert(_iO>=Note::OCT_MIN && _iO<=Note::OCT_MAX,_iArg,_s);
  }
  void m_assert_is_tone(int _iT,int _iArg,const std::string& _s)const{
   m_assert(_iT>=Note::TONE_MIN && _iT<=Note::TONE_MAX,_iArg,_s);
  }

  void m_make_vector(int i);
  void m_vector_set(int i,const enc_scm&);
  bool m_vector_p()const{return SCM_VECTORP(m_scm);}
  int m_vector_length()const;
  enc_scm m_vector_ref(int)const;

  bool m_list_p()const;
  int m_list_length()const;
  enc_scm m_list_ref(int)const;

  void m_make_pair(const enc_scm&,const enc_scm&);
  bool m_pair_p()const{return SCM_CONSP(m_scm);}
  enc_scm m_pair_1st()const;
  enc_scm m_pair_2nd()const;

  int m_get_te_t(bool)const;

  scm_type m_scm;
};

#endif
