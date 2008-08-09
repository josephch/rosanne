// rosanne : Twenty-Eight(28) Card Game
// Copyright (C) 2006-2008 Vipin Cherian
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

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "ra/radlgabout.h"
#include "ra/racommon.h"

IMPLEMENT_DYNAMIC_CLASS( raDlgAbout, wxDialog )

// Event table
BEGIN_EVENT_TABLE( raDlgAbout, wxDialog )
    EVT_BUTTON( XRCID("m_radlgabout_ok"), raDlgAbout::OnAboutBtnOkClick )
END_EVENT_TABLE()


raDlgAbout::raDlgAbout( )
{
}

raDlgAbout::raDlgAbout( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
	SetParent(parent);
	if (!wxXmlResource::Get()->LoadDialog(this, GetParent(), _T("raDlgAbout")))
		wxLogError(wxT("Missing wxXmlResource::Get()->Load() in OnInit()?"));
}

void raDlgAbout::OnAboutBtnOkClick( wxCommandEvent& event )
{
    Destroy();
}
