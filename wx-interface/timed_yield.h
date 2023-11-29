// wx-interface/timed_yield.h:
//  support wxYield() at equal time intervalls

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
// wx12Ton; see the file COPYING or COPYING.txt. If not, write to the Free      // Software Foundation, 59 Temple Place - Suite 330, Boston, MA 02111-1307,
// USA.

#include <wx/timer.h>

class Timed_Yield{
 public:
  Timed_Yield(long _i=10000,long _iR=20):m_iR(_iR),m_iF(_i),m_i(0){}

  bool stop(){
   if((++m_i)%m_iF)return false;
   long l=m_sw.Time();
   if(l<m_iR)m_iF+=m_iF/10;
   if(l>m_iR*2)m_iF-=m_iF/20;
   return true;
  }

  void start(){m_sw.Start();}

 private:
  const long m_iR;
  long m_iF,m_i;
  wxStopWatch m_sw;
};
