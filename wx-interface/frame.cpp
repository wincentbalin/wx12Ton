// wx-interface/frame.cpp

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

#include "frame.h"

#include "series_io.h"
#include "enter_series.h"
#include "wxfile_stl.h"
#include "flush_relat.h"
#include "timed_yield.h"
#include "wx12Ton.h"
#include "algo/find_series.h"
#include <wx/menu.h>
#include <wx/msgdlg.h>
#include <wx/filedlg.h>
#include <wx/string.h>
#include <wx/intl.h>
#include <wx/print.h>
#include <wx/progdlg.h>
#include <wx/textfile.h>

static const char cpNilFun[]=
"(define (info) '())";

FormsFrame::FormsFrame(wxWindow* _wp,wxWindowID _id,const Series& _s):
 wxFrame(_wp,_id,_T("!12Ton")),m_sFunctions(cpNilFun){
 wxMenuBar* mbp=new wxMenuBar;

#define BEGIN_MENU(x)  {wxMenu* mp=new wxMenu;mbp->Append(mp,(x));
#define SUBMENU(z,x,y)  mp->Append(new wxMenuItem(mp,MENU_ITEM_##z,(x),(y)));\
                        Connect(MENU_ITEM_##z,wxEVT_COMMAND_MENU_SELECTED,\
                         (wxObjectEventFunction) &FormsFrame::CB_Menu_##z);
#define SEPERATOR       mp->AppendSeparator();
#define END_MENU       }
#include "menu.inc"
#undef BEGIN_MENU
#undef SUBMENU
#undef SEPERATOR
#undef END_MENU

 SetMenuBar(mbp);
 CreateStatusBar();

 m_srp=new ShowForms(this,-1,_s);
 Fit();
 Show();
}

void FormsFrame::ReportStartPlay(){GetMenuBar()->Enable(MENU_ITEM_Play,false);}
void FormsFrame::ReportStopPlay() {GetMenuBar()->Enable(MENU_ITEM_Play,true );}

#define CB_I(x) void FormsFrame::CB_Menu_##x(wxCommandEvent&)

CB_I(Enter12T){
 AlgorithmList al(wxGetApp().get_algorithms());

 GuileAlgorithms ga(m_sFunctions);
 ga.append_to(al);

 new EnterSeries(this,-1,SeriesValidator(*m_srp),al);
}

CB_I(SelectNorm){m_srp->SetSelect(m_srp->get_series().GetNormalized());}

CB_I(Set){
 Series s=m_srp->get_series();
 m_srp->SetSelect(0);
 m_srp->set_series(s);
}

CB_I(Play){PlaySeries(m_srp->get_series());}

inline bool FileDlg(wxFile& _f,wxWindow* _wp,const wxString& _sT,
               const wxString& _sFN,bool _bO){
 wxFileDialog fdlg(_wp,_sT,_T(""),_sFN,
  _("!12Ton MIDI-file (*.mid)|*.mid|any file (*)|*"), //FIXME: *.* for windows?
  _bO?(wxOPEN|wxFILE_MUST_EXIST):(wxSAVE|wxOVERWRITE_PROMPT));

 if(fdlg.ShowModal()==wxID_OK)
  _f.Open(fdlg.GetPath(),_bO?wxFile::read:wxFile::write);
 else return false;
 //error msg is generate by wx
 return _f.IsOpened();
}

CB_I(Open){
 wxFile f;
 if(!FileDlg(f,this,_("Load series"),_T(""),true))return;

 std::pair<bool,Series> p=wxSeries_IO(f).read();
 if(!p.first){
  wxMessageBox(_("Invalid file format!"),_("Cannot read file"),
   wxOK|wxICON_ERROR,this);
  return;
 }
 m_srp->set_series(p.second);
}

CB_I(Save){
 wxFile f;
 if(!FileDlg(f,this,_("Save series"),
             wxString(m_srp->get_series().get_string().c_str(),wxConvLocal)+
              _T(".mid"),false))return;

 wxSeries_IO(f).write(m_srp->get_series());
}

CB_I(Print){
 wxPrinter prn;
 PrintSeries ps(m_srp->get_series());
 //error msg is generate by wx
 prn.Print(this,&ps);
}

class DisEnableMenu{
 public:
  DisEnableMenu(wxMenuBar* _mbp,int _id):m_mbp(_mbp),m_id(_id){
    m_mbp->Enable(m_id,false);}

  ~DisEnableMenu(){m_mbp->Enable(m_id,true);}
 private:
  wxMenuBar* m_mbp;
  int m_id;
};

CB_I(FlushRelatives){
 FlushRelDlg fr(this,-1);
 if(fr.ShowModal()!=wxID_OK)return;

 std::auto_ptr<SortedCS> scsp(
  new SortedCS(fr.GetValue(),crpCalcTable[fr.GetValue()].iSeries));
 {DisEnableMenu dem(GetMenuBar(),MENU_ITEM_FlushRelatives);
  wxProgressDialog pg(_("Find series"),_("Calculating..."),
   crpCalcTable[fr.GetValue()].iSteps,NULL,
   wxPD_CAN_ABORT|wxPD_ELAPSED_TIME|wxPD_REMAINING_TIME);
  Timed_Yield ty;
  unsigned iSteps,iVSteps=0;
  for(iSteps=0;scsp->Step();++iSteps)if(ty.stop()){
   if(!((++iVSteps)%5) && !pg.Update(iSteps))return;
   wxGetApp().Yield();
   ty.start();
  }
  
  if(iSteps!=crpCalcTable[fr.GetValue()].iSteps || 
     scsp->size()!=crpCalcTable[fr.GetValue()].iSeries){
   wxString s;
   s.Printf(_T("te=%02x Steps=%i Series=%i"),scsp->GetTonalError(),iSteps,
    scsp->size());
   wxMessageBox(s,_("This box must never appear!"),wxOK|wxICON_ERROR);
  }
 }

 new ShowRelatives(this,-1,scsp,SeriesValidator(*m_srp));
}

#ifdef USE_GUILE
CB_I(LoadAlgo){
 wxFileDialog fdlg(this,_("Load algorithms"),_T(""),_T(""),
  _("!12Ton algorithm file (*.scm)|*.scm|any file (*)|*"),
                                                      //FIXME: *.* for windows?
  wxOPEN|wxFILE_MUST_EXIST);

 wxTextFile f;
 if(fdlg.ShowModal()==wxID_OK)f.Open(fdlg.GetPath());
 else return;
 if(!f.IsOpened())return;

 m_sFunctions="";
 for(wxString s=f.GetFirstLine();!f.Eof();s=f.GetNextLine())
  m_sFunctions+=std::string((const char*)s.mb_str(wxConvUTF8))+"\n";
 m_sFunctions+=
  std::string((const char*)f.GetLastLine().mb_str(wxConvUTF8))+"\n";

}
#endif

CB_I(About){
 wxString s;
 s.Printf(_("wx12Ton %s is !12Ton for wxWidgets."),
          wxString(VERSION,wxConvLocal).c_str());
 s+=_T("\n\n"
"Copyright (C) 2004 Matthias Kleinmann <Matthias dot Kleinmann at gmx dot de>\n"
"All rights reserved.\n"
"\n"
"wx12Ton is free software; you can redistribute it and/or modify it under the\n"
"terms of the GNU General Public License as published by the Free Software\n"
"Foundation; either version 2 of the License, or (at your option) any later\n"
"version.\n"
"\n"
"wx12Ton is distributed in the hope that it will be useful, but WITHOUT ANY\n"
"WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS\n"
"FOR A PARTICULAR PURPOSE. See the GNU General Public License for more\n"
"details.\n"
"\n"
"You should have received a copy of the GNU General Public License along with\n"
"wx12Ton; see the file COPYING or COPYING.txt. If not, write to the Free\n"
"Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,\n"
"MA 02111-1307, USA.");

 wxMessageBox(s,_("About !12Ton"),wxOK|wxICON_INFORMATION,this);
}

CB_I(Exit){Close(true);}

#undef CB_I
