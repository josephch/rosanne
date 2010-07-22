// Rosanne : Twenty Eight (28) Card Game
// Copyright (C) 2006-2009 Vipin Cherian
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, 
// Boston, MA  02110-1301, USA

#ifndef _RADLGABOUT_H_
#define _RADLGABOUT_H_

//#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
//#pragma interface "radlgabout.h"
//#endif

#include "wx/xrc/xmlres.h"

class raDlgAbout: public wxDialog
{    
    DECLARE_DYNAMIC_CLASS( raDlgAbout )
    DECLARE_EVENT_TABLE()

public:
    raDlgAbout( );
    raDlgAbout( wxWindow* parent, wxWindowID id , const wxString& caption, const wxPoint& pos, const wxSize& size, long style);
    void OnAboutBtnOkClick( wxCommandEvent& event );
};

#endif
