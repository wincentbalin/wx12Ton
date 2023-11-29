// wx-interface/wx12Ton.cpp

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

#include "wx12Ton.h"

#include "frame.h"
#include "icon.h"
#include "playfile.h"
#include "series_io.h"
#include "timed_yield.h"
#include <wx/string.h>
#include <wx/log.h>
#include <wx/msgdlg.h>

//FIXME: should be on stderr on unix and a msgbox on win
inline void WarnAtCall(const wxString& _s){wxLogWarning(_s);}

void wx12Ton::OnInitCmdLine(wxCmdLineParser& _cl){
 wxApp::OnInitCmdLine(_cl);

 _cl.SetLogo(_T("wx12Ton"));
 _cl.AddOption(_T("l"),_T("set_language"),
  _T("set the language, one of (\"en\",\"de\")"));
 _cl.AddOption(_T("p"),_T("set_language_path"),
  _T("set path to localization files"));
 if(GuileAlgorithms::guile_is_available()){
  _cl.AddOption(_T("g"),_T("set_guile_data_dir"),
   _T("alternative directory to $GUILE_DATA_DIR"));
 }
 _cl.AddOption(_T("m"),_T("midi_player"),
  _T("set external MIDI player program (default: \"playmus_12T -f %s\")"));
 _cl.AddParam(_T("!12Ton-MIDI-file"),
  wxCMD_LINE_VAL_STRING,wxCMD_LINE_PARAM_OPTIONAL);
}

bool wx12Ton::OnCmdLineParsed(wxCmdLineParser& _cl){
 wxString sO;
 if(_cl.Found(_T("l"),&sO)){
       if(sO==_T("de"))m_locale.Init(wxLANGUAGE_GERMAN);
  else if(sO==_T("en"));//m_locale.Init(wxLANGUAGE_ENGLISH);
  else{WarnAtCall(
   _T("invalid command line parameter (--set_language): language \"")
   +sO+_T("\" not one of (\"en\",\"de\")."));
   return false;
  }
 }else m_locale.Init();
 if(m_locale.IsOk() && _cl.Found(_T("p"),&sO))
  m_locale.AddCatalogLookupPathPrefix(sO);
 
 if(GuileAlgorithms::guile_is_available()){
  if(_cl.Found(_T("g"),&sO))
   GuileAlgorithms::guile_init((const char*)sO.mb_str(wxConvUTF8));
#ifdef GUILE_LOAD_PATH
  else GuileAlgorithms::guile_init(GUILE_LOAD_PATH);
#else
  else GuileAlgorithms::guile_init();
#endif
 }else GuileAlgorithms::guile_init();
     
 if(_cl.Found(_T("m"),&sO));else sO=_T("playmus_12T -f %s");
 m_pmf.set_command(sO);

 if(_cl.GetParamCount()==1){
  sO=_cl.GetParam(0);
  wxFile f(sO,wxFile::read);
  if(!f.IsOpened()){
   WarnAtCall(_T("error on open file \"")+sO+_T("\"."));
   return false;
  }
  std::pair<bool,Series> p=wxSeries_IO(f).read();
  if(!p.first){
   WarnAtCall(_T("File \"")+sO+_T("\" has invalid file format."));
   return false;
  }
  m_s=p.second;
 }
 
 return wxApp::OnCmdLineParsed(_cl);
} 

bool wx12Ton::OnInit(){
 if(!wxApp::OnInit())return false;

 if(m_locale.IsOk())m_locale.AddCatalog(_T("wx12Ton"));

 FormsFrame* mfp=new FormsFrame(NULL,-1,m_s);
 mfp->SetIcon(wxICON(wx12Ton));
 mfp->Show(true);
 SetTopWindow(mfp);
 return true;
}


#define A(x,y) \
class A_##x:public Algorithm::algorithm_base{\
 public:\
  virtual Series operator()(const Series&,int,int);\
  static wxString get_name(){return (y);}\
};\
Series A_##x::operator()

#define A123 (const Series& _s,int _iTE,int _iM)
#define A12  (const Series& _s,int _iTE,int)
#define A13  (const Series& _s,int,int _iM)
#define A1   (const Series& _s,int,int)

A(Retrograde,_("retrograde"    ))A1{return _s.GetRetrograde();}
A(Inversion ,_("inversion"     ))A1{return _s.GetInversion();}
A(Transpose ,_("transpose (m)" ))A13{return _s.GetBasicForm(_iM);}
A(Normalize ,_("normalize"     ))A1{return _s.GetNormalized();}
A(Relative  ,_("relative (t,m)"))A123{
 Series s=_s;
 for(;_iM>0;--_iM){
  s.next_permutation();
  while(s.step_find_valid_forw(_iTE).second!=Series::TE_OK);
 }
 for(;_iM<0;++_iM){
  s.prev_permutation();
  while(s.step_find_valid_backw(_iTE).second!=Series::TE_OK);
 }
 return s;
}

#undef A
#undef A123
#undef A12
#undef A13
#undef A1

AlgorithmList wx12Ton::get_algorithms()const{
 AlgorithmList al;

#define A(x) {\
 Algorithm a(new A_##x);\
 a.insert_name(\
  std::string((const char*)getLocaleName().mb_str(wxConvUTF8),2),\
  std::string((const char*)A_##x::get_name().mb_str(wxConvUTF8))\
 );\
 al.append(a);}

 A(Retrograde)
 A(Inversion )
 A(Transpose )
 A(Normalize )
 A(Relative  )

#undef A
 
 return al;
}

bool has_playable_series::PlaySeries(const Series& _s){
 if(MidiIsPlaying()){
  wxMessageBox(_("MIDI program already running"),
   _("Cannot play MIDI sequence"),wxOK|wxICON_ERROR);
  return false;
 }
 play_midi_file::scoped_file sf(new wxTempFile_scoped(_T("wx12Ton")));
//FIXME: error handling done by wx?
 bool b=wxSeries_IO(sf->get_file()).write(_s);
 sf->get_file().Close();
 if(!b)return false;
 return PlayMidi(sf);
}

IMPLEMENT_APP(wx12Ton)
