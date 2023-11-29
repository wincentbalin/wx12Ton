// wx-interface/flush_relat.h:
//  create the list of all series

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

#ifndef FLUSH_RELAT_H
#define FLUSH_RELAT_H

#include "series_widget.h"
#include "algo/series.h"
#include "algo/find_series.h"
#include "ids.h"
#include <wx/dialog.h>
#include <wx/frame.h>
#include <memory>

class FlushRelDlg:public wxDialog{
 public:
  FlushRelDlg(wxWindow*,wxWindowID,int=Series::TE_DEFAULT);
  void CB_Set(int);

  int GetValue();
 private:
  enum {TEC_Errors=ID_FlushRelDlg,ST_Steps,ST_Series,ST_Memory};
};

class ShowRelatives:public wxFrame{
 public:
  ShowRelatives(wxWindow*,wxWindowID,std::auto_ptr<SortedCS>,
   const SeriesValidator&);
};

#endif
