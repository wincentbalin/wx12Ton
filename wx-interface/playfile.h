// wx-interface/playfile.h:
//  play a midi file thru an external player

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

#ifndef PLAY_FILE_H
#define PLAY_FILE_H

#include "wxfile_stl.h"
#include <wx/string.h>
#include <wx/event.h>
#include <wx/process.h>
#include <memory>
#include <set>

class has_playable;

class play_midi_file:wxEvtHandler{
 public:
  play_midi_file():m_iPID(0),m_ppProc(NULL),m_objp(NULL){}
  ~play_midi_file(){if(m_ppProc)m_ppProc->Detach();}

  void set_command(const wxString& _s){m_sCommand=_s;}
  const wxString& get_command()const{return m_sCommand;}

  typedef std::auto_ptr<wxTempFile_scoped> scoped_file;
  bool Play(scoped_file,has_playable&);
  bool IsPlaying()const{return m_ppProc;}
  void Detach(const has_playable& _obj){if(&_obj==m_objp)m_objp=NULL;}

  void ReportPlaying(has_playable& _obj){m_shppReport.insert(&_obj);}
  void DontReportPlaying(has_playable& _obj){m_shppReport.erase(&_obj);}

 private:
  int m_iPID;
  wxProcess* m_ppProc;
  has_playable* m_objp;
  wxString m_sCommand;

  std::set<has_playable*> m_shppReport; //FIXME: sets of pointers are evil
  
  void CB_EndProc(wxProcessEvent&);
  DECLARE_EVENT_TABLE()
};

class has_playable{
 public:
  explicit has_playable(play_midi_file& _pmf):p_pmf(_pmf){
   p_pmf.ReportPlaying(*this);}
  virtual ~has_playable(){p_pmf.Detach(*this);p_pmf.DontReportPlaying(*this);}

  virtual void OnStartPlay(){}
  virtual void OnStopPlay(){}
  virtual void ReportStartPlay(){}
  virtual void ReportStopPlay(){}

  bool PlayMidi(play_midi_file::scoped_file _sf){return p_pmf.Play(_sf,*this);}
  bool MidiIsPlaying()const{return p_pmf.IsPlaying();}
 protected:
  play_midi_file& p_pmf;
};


#endif
