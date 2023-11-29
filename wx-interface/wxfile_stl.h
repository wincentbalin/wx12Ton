// wx-interface/wxfile_stl.h:
//  a scoped wxTempFile

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

#ifndef WXFILE_STL
#define WXFILE_STL

#include <wx/file.h>
#include <wx/filename.h>

class wxTempFile_scoped{
 public:
  ~wxTempFile_scoped(){
   if(m_f.IsOpened())m_f.Close();
   if(!m_sFN.empty())wxRemoveFile(m_sFN);
  }
  explicit wxTempFile_scoped(const wxString& _s){
   m_sFN=wxFileName::CreateTempFileName(_s,&m_f);}

  wxFile& get_file(){return m_f;}
  const wxString& get_file_name()const{return m_sFN;}

 private:
  wxFile m_f;
  wxString m_sFN;
};

#endif
