// guile-interface/guile_helper.h
//  some dirty hack to automatically define guile functions

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

#ifndef GUILE_HELPER_H
#define GUILE_HELPER_H

#include "enc_scm.h"
#include <list>
#include <string> 
#include <map>

namespace guile_helper{

class fu_info{
 public:
  fu_info(){}
  template <typename T>
  fu_info(const std::string& _s,int _i1,int _i2,T _t):
   m_s(_s),m_i1(_i1),m_i2(_i2),m_fp(reinterpret_cast<fu_p_t>(_t)){}
 
  void define_gsubr()const{
   scm_c_define_gsubr(m_s.c_str(),m_i1-m_i2,m_i2,0,m_fp);}

  void set_name(const std::string& _s){m_s=_s;}
  const std::string& get_name()const{return m_s;}

 private:
  typedef scm_unused_struct*(*fu_p_t)();
  std::string m_s;
  int m_i1,m_i2;
  fu_p_t m_fp;
};

typedef std::list<fu_info> ArgL;
ArgL argl;
typedef std::map<std::string,std::string> ChangeL;
ChangeL changel;

inline void run(){
 for(ArgL::iterator i=argl.begin();i!=argl.end();++i){
  ChangeL::const_iterator iN=changel.find(i->get_name());
  if(iN!=changel.end())i->set_name(iN->second);
  i->define_gsubr();
 }
 argl.clear();
 changel.clear();
}

}

#define def_guile_init(x) extern "C"{void x(){guile_helper::run();}}

#define f0(x,o)\
static enc_scm scmized_##x();\
extern "C"{scm_type x(){return scmized_##x();}}\
namespace guile_helper{\
 struct dummy_##x{\
  dummy_##x(){argl.push_back(ArgL::value_type(#x,0,o,&x));}\
 }dummy_inst_##x;\
}\
static enc_scm scmized_##x()

#define f1(x,o)\
static enc_scm scmized_##x(const enc_scm&);\
extern "C"{scm_type x(scm_type _1){return scmized_##x(_1);}}\
namespace guile_helper{\
 struct dummy_##x{\
  dummy_##x(){argl.push_back(ArgL::value_type(#x,1,o,&x));}\
 }dummy_inst_##x;\
}\
static enc_scm scmized_##x(const enc_scm& _1)

#define f2(x,o)\
static enc_scm scmized_##x(const enc_scm&,const enc_scm&);\
extern "C"{scm_type x(scm_type _1,scm_type _2){return scmized_##x(_1,_2);}}\
namespace guile_helper{\
 struct dummy_##x{\
  dummy_##x(){argl.push_back(ArgL::value_type(#x,2,o,&x));}\
 }dummy_inst_##x;\
}\
static enc_scm scmized_##x(const enc_scm& _1,const enc_scm& _2)

#define f3(x,o)\
static enc_scm scmized_##x(const enc_scm&,const enc_scm&,const enc_scm&);\
extern "C"{\
 scm_type x(scm_type _1,scm_type _2,scm_type _3){return scmized_##x(_1,_2,_3);}\
}\
namespace guile_helper{\
 struct dummy_##x{\
  dummy_##x(){argl.push_back(ArgL::value_type(#x,3,o,&x));}\
 }dummy_inst_##x;\
}\
static enc_scm scmized_##x(const enc_scm& _1,const enc_scm& _2,\
                           const enc_scm& _3)

#define change_guile_name(x,y)\
namespace guile_helper{\
 struct dummy_change_name_##x{\
  dummy_change_name_##x(){changel.insert(ChangeL::value_type(#x,(y)));}\
 }dummy_change_name_inst_##x;\
}

#endif
