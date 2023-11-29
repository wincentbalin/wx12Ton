// wx-interface/tonal_widget.cpp:

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

#include "tonal_widget.h"
#include "helper.h"
#include "pretty_sizer.h"
#include <wx/intl.h>
#include <wx/checkbox.h>
#include <wx/validate.h>
#include <wx/statbox.h>

using namespace pretty_sizer;

template <typename T>
inline void set_bit(T& _tV,const T& _tB,bool _b){_tV=(_tV&(~_tB))|(_b?_tB:0);}
template <typename T>
inline bool get_bit(const T& _tV,const T& _tB){return _tV&_tB;}

class TonalErrorsValidator:public wxValidator{
 public:
  TonalErrorsValidator(int& _i,Series::TonalError _te):
   wxValidator(),m_i(_i),m_te(_te){}
  TonalErrorsValidator(const TonalErrorsValidator& _tev):
   wxValidator(),m_i(_tev.m_i),m_te(_tev.m_te){Copy(_tev);}
  virtual TonalErrorsValidator* Clone()const{
   return new TonalErrorsValidator(*this);}

  virtual bool TransferFromWindow(){
   set_bit<int>(m_i,m_te,
    dynamic_cast<wxCheckBox*>(m_validatorWindow)->GetValue());
   return true;
  }
  virtual bool TransferToWindow(){
   dynamic_cast<wxCheckBox*>(m_validatorWindow)->SetValue(
    get_bit<int>(m_i,m_te));
   return true;
  }

 private:
  int& m_i;
  const Series::TonalError m_te;
};

TonalErrorsCtl::TonalErrorsCtl(wxWindow* _wp,wxWindowID _id,int _i,
 const wxString& _s):
 wxPanel(_wp,_id),m_cb(NULL,NULL),m_i(_i){
 Sizer s(new wxStaticBoxSizer(new wxStaticBox(this,-1,_s),wxVERTICAL));

#define ADD_CheckB(id,en) {\
 wxCheckBox* cbp=new wxCheckBox(this,CB_##id,\
  tonal_error_str(Series::TE_##id),\
  wxDefaultPosition,wxDefaultSize,0,TonalErrorsValidator(m_i,Series::TE_##id));\
 s << cbp /Expand(0,wxEXPAND);\
 if(!(en))cbp->Disable();} 
 
 ADD_CheckB(FOURTH    ,!(m_i&Series::TE_ALL_INTERV))
 ADD_CheckB(CHORD     ,true)
 ADD_CheckB(EQ_INTERV ,!(m_i&Series::TE_ALL_INTERV))
 ADD_CheckB(ALL_INTERV,!(m_i&Series::TE_FOURTH))
#undef ADD_CheckB

 SetSizerAndFit(s);
 TransferDataToWindow();
}

void TonalErrorsCtl::CB_Clicked(wxCommandEvent& _ce){
#define DISABLE(id,val) {\
 wxCheckBox* cbp=dynamic_cast<wxCheckBox*>(FindWindow(CB_##id));\
 cbp->SetValue(val);\
 cbp->Disable();}
#define ENABLE(id) {\
 wxCheckBox* cbp=dynamic_cast<wxCheckBox*>(FindWindow(CB_##id));\
 cbp->Enable();}
 switch(_ce.GetId()){
  case CB_FOURTH:
   if(_ce.IsChecked())DISABLE(ALL_INTERV,false)
   else               ENABLE (ALL_INTERV)
   break;
  case CB_ALL_INTERV:
   if(_ce.IsChecked()){
    DISABLE(FOURTH   ,false)
    DISABLE(EQ_INTERV,true)
   }else{
    ENABLE(FOURTH   )
    ENABLE(EQ_INTERV)
   }
   break;
 }
#undef DISABLE
#undef ENABLE
 if(m_cb.first)(m_cb.first->*m_cb.second)(GetValue());
}

BEGIN_EVENT_TABLE(TonalErrorsCtl,wxPanel)
 EVT_CHECKBOX(CB_FOURTH    ,TonalErrorsCtl::CB_Clicked)
 EVT_CHECKBOX(CB_CHORD     ,TonalErrorsCtl::CB_Clicked)
 EVT_CHECKBOX(CB_EQ_INTERV ,TonalErrorsCtl::CB_Clicked)
 EVT_CHECKBOX(CB_ALL_INTERV,TonalErrorsCtl::CB_Clicked)
END_EVENT_TABLE()

