// wx-interface/playfile.cpp

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

#include "playfile.h"

#include <wx/intl.h>
#include <wx/msgdlg.h>
//std::for_each
#include <algorithm>
//std::mem_fun
#include <functional>

class midi_process:public wxProcess{
 public:
  explicit midi_process(wxEvtHandler* _ehp,play_midi_file::scoped_file _sf):
   wxProcess(_ehp),m_sf(_sf){}

 private:
  play_midi_file::scoped_file m_sf;
};

void play_midi_file::CB_EndProc(wxProcessEvent& _pe){
 if(_pe.GetExitCode())
  wxMessageBox(_("Error running MIDI program"),
   _("Error playing MIDI sequence"),wxOK|wxICON_ERROR);
 delete m_ppProc;m_ppProc=NULL;
 m_iPID=0;
 std::for_each(m_shppReport.begin(),m_shppReport.end(),
               std::mem_fun(&has_playable::ReportStopPlay));
 if(m_objp)m_objp->OnStopPlay();m_objp=NULL;
}

bool play_midi_file::Play(scoped_file _sf,has_playable& _obj){
 wxString sC;sC.Printf(m_sCommand,_sf->get_file_name().c_str());
 std::auto_ptr<midi_process> proc_ptr(new midi_process(this,_sf));

 m_iPID=wxExecute(sC,wxEXEC_ASYNC,&*proc_ptr);
 if(m_iPID){
  m_ppProc=proc_ptr.release();
  m_objp=&_obj;
  m_objp->OnStartPlay();
  std::for_each(m_shppReport.begin(),m_shppReport.end(),
                std::mem_fun(&has_playable::ReportStartPlay));
  return true;
 }
 sC.Printf(_("Cannot execute command \"%s\""),sC.c_str());
 wxMessageBox(_("Error running MIDI program"),sC,wxOK|wxICON_ERROR);
 return false;
}

BEGIN_EVENT_TABLE(play_midi_file,wxEvtHandler)
 EVT_END_PROCESS(-1,play_midi_file::CB_EndProc)
END_EVENT_TABLE()

