/// \file algo/find_series.h
/// \author Matthias Kleinmann \<Matthias dot Kleinmann at gmx dot de\>
/// \brief store valid series in a container

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

#ifndef FIND_SERIES_H
#define FIND_SERIES_H

#include "series.h"
#include <vector>
//lexicalogaphical_compare,equal,upper_bound,lower_bound
#include <algorithm>
//uint8_t
#include <stdint.h>

struct CalcResult{
 uint32_t iSteps,iSeries;
};
extern const CalcResult crpCalcTable[];

/*! \brief Save a series, as it appears in #SortedCS.

  This class is optimized for low memory usage (5 octets at the moment, the
  theoretical minimum would be 4, but this is hard to implement). For that,
  no  octave information is stored and the first note is assumed to be
  #Note::TONE_MIN. Further it is assumed, that no note occurs twice.

  As side effect, the comparing operators are much faster then in the class
  #Series.

  \sa #SortedCS, #Series
*/
class CompactSeries{
 public:
  /// plain c'tor (does not initalize)
  CompactSeries(){}
  /*! \brief construct from class #Series

      Any octave information contained in #Series is discareded. Further
      it is assumed, that the first note of the series is #Note::TONE_MIN
      and that no note occurs twice.
  */
  CompactSeries(const Series& _s){m_assign(_s);}

//the following would make assignments of the kind CompactSeries=Series
//ambigous, thus it is if'ed out.
#if 0
  CompactSeries& operator =(const Series& _s){m_assign(_s);return *this;}
#endif
  
  /*! \brief convert to class #Series

      no octave values are stored
  */
  operator Series()const;

  /// get note value (access to element \c 11 is slow)
  Note::NoteDiff get_val_at(int)const;
  /// set note value (only possible for elements \c 1,2,...,10)
  void set_val_at(int _i,Note::NoteDiff _nd){
   if(_i%2)m_cp[(_i-1)/2]=m_cp[(_i-1)/2]&0x0f|(uint8_t(_nd&0x0f)<<4);
   else    m_cp[(_i-2)/2]=m_cp[(_i-2)/2]&0xf0|(uint8_t(_nd&0x0f));
  }

  /*! \name comparing support */
  //@{
  /// lexicalographical compare note values
  bool operator <(const CompactSeries& _r)const{return
   std::lexicographical_compare(m_cp,m_cp+5,_r.m_cp,_r.m_cp+5);}
  bool operator ==(const CompactSeries& _r)const{return
   std::equal(m_cp,m_cp+5,_r.m_cp);}
  //@}

 private:
  void m_assign(const Series& _s){
   for(int i=0;i<5;++i)
    m_cp[i]=(uint8_t(_s.at(2*i+1).get_val())<<4)|
             uint8_t(_s.at(2*i+2).get_val());
  }
 
  uint8_t m_cp[5];
};

/*! \brief Stores the set of all normalized series, which do not have
           certain tonal errors.

    This class can be used to generate a complete set of all normalized
    series. By convention (and without loss of generality) the first note
    of each series is #Note::TONE_MIN. Once all valid series are found,
    this class provides an interface, which is equivalent to the STL container
    \c const \c std::set<#CompactSeries>.

    The testing for tonal pharses is done via #Series::FindError(int)const.
    The testing whether the series is normalized is done via
    #Series::IsNormalized()const.

    \sa #CompactSeries, #Series
*/
class SortedCS{
 public:
  /// create empty list
  /*! \param _iTE combination of #Series::TonalError, which makes a series
                  invalid
      \param _iS guess of the number of valid series, that will be found

      This c'tor just initalizes the class. The parameter \p _iTE controls,
      which series will not be taken into the list. The parameter \p _iS
      can be used to reserve the correct ammount of memory in advance.
      This is done via \c std::vector<CompactSeries>::reserve(size_type).
  */
  explicit SortedCS(int _iTE=Series::TE_DEFAULT,size_t _iS=0):
   m_iTE(_iTE),m_s(sFirstValidSeries){m_vcs.reserve(_iS);}
  /// perform a single calculation step
  /*! \retval false the last step has been performed
      \retval true  the last step has not yet been performed
     \ex SortedCS scs;
 while(scs.Step()); \end_ex
     \note The number of steps strongly depends on the value of
           #GetTonalError()const.
  */
  bool Step(){m_step();return (m_s<sFirstInvalidSeries);}
  /// return the tonal error, which was passed to the c'tor.
  int GetTonalError()const{return m_iTE;}

  typedef std::vector<CompactSeries>::const_iterator const_iterator;
  /*! \name STL containter interface*/
  //{@
  const CompactSeries& front()const{return m_vcs.front();}
  const CompactSeries& back()const{return m_vcs.back();}
  const_iterator begin()const{return m_vcs.begin();} 
  const_iterator end()const{return m_vcs.end();} 
  size_t size()const{return m_vcs.size();}
  bool empty()const{return m_vcs.empty();}

  const_iterator lower_bound(const CompactSeries& _cs)const{
   return std::lower_bound(begin(),end(),_cs);} 
  const_iterator upper_bound(const CompactSeries& _cs)const{
   return std::upper_bound(begin(),end(),_cs);}
  const_iterator find(const CompactSeries& _cs)const{
   const_iterator i=lower_bound(_cs);if(*i==_cs)return i;return end();}
  const CompactSeries& operator[](const CompactSeries& _cs)const{
   return *find(_cs);}
  const CompactSeries& operator[](size_t _i)const{return m_vcs[_i];}
  //@}

 private:
  void m_step();

  const int m_iTE;
  Series m_s;
  static const Series sFirstValidSeries,sFirstInvalidSeries;
  std::vector<CompactSeries> m_vcs;
};

#endif
