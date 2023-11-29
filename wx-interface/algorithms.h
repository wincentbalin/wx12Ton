// wx-interface/algorithms.h:
//  run algorithms on a series, where the algos may be guile functions

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

#ifndef ALGORITHMS_H
#define ALGORITHMS_H

#include "algo/series.h"
#include <string>
#include <vector>
#include <map>

class Algorithm{
 public:
  struct algorithm_base{
   virtual ~algorithm_base(){}
   virtual Series operator()(const Series& _s,int,int)=0;
  };

  ~Algorithm(){if(--(*m_ipRef))return;delete m_algo_ptr;delete m_ipRef;}
  Algorithm(const Algorithm& _a):
   m_algo_ptr(_a.m_algo_ptr),m_ipRef(_a.m_ipRef),m_mNames(_a.m_mNames){
    ++(*m_ipRef);}
  Algorithm(algorithm_base* _ap=NULL):
    m_algo_ptr(_ap),m_ipRef(new unsigned(1)){}

  void insert_name(const std::string& _s1,const std::string& _s2){
   m_mNames.insert(std::make_pair(_s1,_s2));}

  const std::string& find_name(const std::string& _s)const{
   std::map<std::string,std::string>::const_iterator i=m_mNames.find(_s);
   if(i==m_mNames.end())return m_mNames.begin()->second;
   return i->second;
  }

  Series operator()(const Series& _s,int _iTE,int _iM)const{
   return (*m_algo_ptr)(_s,_iTE,_iM);}

 private:
  algorithm_base* m_algo_ptr;
  unsigned* m_ipRef;
  std::map<std::string,std::string> m_mNames;
};

class AlgorithmList{
 public:
  AlgorithmList():m_iS(0){}
  const Algorithm& operator[](size_t _i)const{return m_dAlgos[_i];}

  void append(const Algorithm& _a){m_dAlgos.push_back(_a);++m_iS;}
  size_t size()const{return m_iS;}

 private:
  size_t m_iS;
  std::vector<Algorithm> m_dAlgos;
};

class GuileAlgorithms{
 public:
  static bool guile_is_available();
  static void guile_init(const std::string& ="");

  explicit GuileAlgorithms(const std::string& _s):m_s(_s){}

  void append_to(AlgorithmList& _a)const{
   std::vector<Algorithm> li=m_get_algorithms();
   for(std::vector<Algorithm>::const_iterator i=li.begin();i!=li.end();++i)
    _a.append(*i);
  }

  Series run(const std::string&,const Series&,int,int)const;

 private:
  std::string m_s;

  std::vector<Algorithm> m_get_algorithms()const;
};

#endif
