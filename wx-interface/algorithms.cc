// wx-interface/algorithms.cc

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

#include "algorithms.h"

#ifdef USE_GUILE

#include "guile-interface/enc_scm.h"
#include "guile-interface/libguile-12ton.h"
#include <sstream>
#include <list>
#include <stdlib.h>

class guile_algorithm:public Algorithm::algorithm_base{
 public:
  guile_algorithm(const GuileAlgorithms& _ga,const std::string& _s):
   m_ga(_ga),m_sFu(_s){}

  virtual Series operator()(const Series& _s,int _iTE,int _iM){
   return m_ga.run(m_sFu,_s,_iTE,_iM);}

 private:
  const GuileAlgorithms m_ga;
  std::string m_sFu;
};


bool GuileAlgorithms::guile_is_available(){return true;}
void GuileAlgorithms::guile_init(const std::string& _s){
 if(!_s.empty()){
  std::string s=std::string("GUILE_LOAD_PATH=")+_s;
  char* cp=new char[s.size()];
  s.copy(cp,s.size());
  putenv(cp);
 }
 scm_init_guile();
 INIT_12TON();
}

std::vector<Algorithm> GuileAlgorithms::m_get_algorithms()const{
 std::vector<Algorithm> li;

 typedef std::list<enc_scm> scmlist;
 scmlist l1=enc_scm(scm_c_eval_string((m_s+"(info)").c_str())).get<scmlist>();
 while(!l1.empty()){
  scmlist l2=l1.front().get<scmlist>();
  Algorithm algo(new guile_algorithm(*this,l2.front().get<std::string>()));
  l2.pop_front();
  while(!l2.empty()){
   scmlist l3=l2.front().get<scmlist>();
   algo.insert_name(l3.front().get<std::string>(),l3.back().get<std::string>());
   l2.pop_front();
  }
  l1.pop_front();
  li.push_back(algo);
 }
 return li;
}

Series GuileAlgorithms::run(const std::string& _sFu,const Series& _s,
 int _iTE,int _iM)const{

 std::stringstream ss;
 ss << "(" << _sFu << " (series_new \"" << _s.get_string() << "\") '(";

#define X(x) if(_iTE&Series::TE_##x)ss <<"\"" << #x << "\" ";
 X(OK)X(FOURTH)X(CHORD)X(EQ_INTERV)X(ALL_INTERV)
#undef X

 ss << ") " << _iM << ")";

 return enc_scm(scm_c_eval_string((m_s+ss.str()).c_str())).get<Series>();
}

#else

bool GuileAlgorithms::guile_is_available(){return false;}
void GuileAlgorithms::guile_init(const std::string&){}

std::vector<Algorithm> GuileAlgorithms::m_get_algorithms()const{
 return std::vector<Algorithm>();
}

Series GuileAlgorithms::run(const std::string&,const Series&,int,int)const{
 return Series();
}

#endif
