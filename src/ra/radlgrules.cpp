// rosanne : Twenty-Eight(28) Card Game
// Copyright (C) 2006-2007 Vipin Cherian
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
#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "radlgrules.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

////@begin includes
////@end includes

#include "ra/radlgrules.h"
#include "ra/raconfig.h"

////@begin XPM images
////@end XPM images

/*!
 * raDlgRules type definition
 */

IMPLEMENT_DYNAMIC_CLASS( raDlgRules, wxDialog )

/*!
 * raDlgRules event table definition
 */

BEGIN_EVENT_TABLE( raDlgRules, wxDialog )

////@begin raDlgRules event table entries
    EVT_INIT_DIALOG( raDlgRules::OnInitDialog )

    EVT_BUTTON( XRCID("ID_RULES_BTN_APPLY"), raDlgRules::OnRulesBtnApplyClick )

////@end raDlgRules event table entries

END_EVENT_TABLE()

/*!
 * raDlgRules constructors
 */

raDlgRules::raDlgRules( )
{
    Init();
}

raDlgRules::raDlgRules( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
    Init();
    Create(parent, id, caption, pos, size, style);
}

/*!
 * Dialog creator
 */

bool raDlgRules::Create( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
////@begin raDlgRules creation
    SetExtraStyle(GetExtraStyle()|wxWS_EX_BLOCK_EVENTS);
    SetParent(parent);
    CreateControls();
    if (GetSizer())
    {
        GetSizer()->SetSizeHints(this);
    }
    Centre();
////@end raDlgRules creation
    return true;
}

/*!
 * Member initialisation 
 */

void raDlgRules::Init()
{
////@begin raDlgRules member initialisation
////@end raDlgRules member initialisation
}
/*!
 * Control creation for Dialog
 */

void raDlgRules::CreateControls()
{    
////@begin raDlgRules content construction
    if (!wxXmlResource::Get()->LoadDialog(this, GetParent(), _T("ID_RADLGRULES")))
        wxLogError(wxT("Missing wxXmlResource::Get()->Load() in OnInit()?"));
////@end raDlgRules content construction

    // Create custom windows not generated automatically here.
////@begin raDlgRules content initialisation
////@end raDlgRules content initialisation
}

/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON
 */

void raDlgRules::OnRulesBtnApplyClick( wxCommandEvent& event )
{
	wxRadioButton *radio_clockwise;
	wxRadioButton *radio_anticlockwise;
	wxComboBox *combo_minbid3;
	wxCheckBox *check_waiverule4;
	wxCheckBox *check_sluffjacks;
	raConfData new_conf;

	raConfig::GetInstance()->GetData(&new_conf);

	radio_clockwise = XRCCTRL(*this, "ID_RULES_RAD_CLOCKWISE", wxRadioButton);
	radio_anticlockwise = XRCCTRL(*this, "ID_RULES_RAD_ANTICLOCKWISE", wxRadioButton);
	//ID_RULES_CMB_MINBID3RDROUND
	combo_minbid3 = XRCCTRL(*this, "ID_RULES_CMB_MINBID3RDROUND", wxComboBox);
	check_waiverule4 = XRCCTRL(*this, "ID_RULES_CHK_WAIVERULE4", wxCheckBox);
	check_sluffjacks = XRCCTRL(*this, "ID_RULES_CHK_SLUFFJACKS", wxCheckBox);

	if(radio_clockwise->GetValue())
	{
		new_conf.game_data.clockwise = true;
	}
	else if(radio_anticlockwise->GetValue())
	{
		new_conf.game_data.clockwise = false;
	}
	else
	{
		wxLogError(wxString::Format(wxT("Unexpected value. %s:%d"), __FILE__, __LINE__));
	}

	switch(combo_minbid3->GetSelection())
	{
	case 0:
		new_conf.game_data.min_bid3 = 23;
		break;
	case 1:
		new_conf.game_data.min_bid3 = 24;
		break;
	default:
		wxLogError(wxString::Format(wxT("Unexpected value. %s:%d"), __FILE__, __LINE__));
		break;
	}

	// Check waiver of rule 4
	new_conf.game_data.waive_rule4 = !check_waiverule4->GetValue();
	new_conf.game_data.sluff_jacks = check_sluffjacks->GetValue();


	raConfig::GetInstance()->SetData(&new_conf);
////@begin wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON in Dialog.
    // Before editing this code, remove the block markers.
    event.Skip();
////@end wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON in Dialog. 

	Destroy();
}

/*!
 * Should we show tooltips?
 */

bool raDlgRules::ShowToolTips()
{
    return true;
}

/*!
 * Get bitmap resources
 */

wxBitmap raDlgRules::GetBitmapResource( const wxString& name )
{
    // Bitmap retrieval
////@begin raDlgRules bitmap retrieval
    wxUnusedVar(name);
    return wxNullBitmap;
////@end raDlgRules bitmap retrieval
}

/*!
 * Get icon resources
 */

wxIcon raDlgRules::GetIconResource( const wxString& name )
{
    // Icon retrieval
////@begin raDlgRules icon retrieval
    wxUnusedVar(name);
    return wxNullIcon;
////@end raDlgRules icon retrieval
}
/*!
 * wxEVT_INIT_DIALOG event handler for ID_RADLGRULES
 */

void raDlgRules::OnInitDialog( wxInitDialogEvent& event )
{
	wxRadioButton *radio_clockwise;
	wxRadioButton *radio_anticlockwise;
	wxComboBox *combo_minbid3;
	wxCheckBox *check_waiverule4;
	wxCheckBox *check_sluffjacks;
	raConfData conf_data;

	raConfig::GetInstance()->GetData(&conf_data);
	radio_clockwise = XRCCTRL(*this, "ID_RULES_RAD_CLOCKWISE", wxRadioButton);
	radio_anticlockwise = XRCCTRL(*this, "ID_RULES_RAD_ANTICLOCKWISE", wxRadioButton);
	combo_minbid3 = XRCCTRL(*this, "ID_RULES_CMB_MINBID3RDROUND", wxComboBox);
	check_waiverule4 = XRCCTRL(*this, "ID_RULES_CHK_WAIVERULE4", wxCheckBox);
	check_sluffjacks = XRCCTRL(*this, "ID_RULES_CHK_SLUFFJACKS", wxCheckBox);

	if(conf_data.game_data.clockwise)
	{
		radio_clockwise->SetValue(true);
	}
	else
	{
		radio_anticlockwise->SetValue(true);
	}

	wxASSERT((conf_data.game_data.min_bid3 == 23) || (conf_data.game_data.min_bid3 == 24));
	switch(conf_data.game_data.min_bid3)
	{
	case 23:
		combo_minbid3->SetSelection(0);
		break;
	case 24:
		combo_minbid3->SetSelection(1);
		break;
	default:
		wxLogError(wxString::Format(wxT("Unexpected value. %s:%d"), __FILE__, __LINE__));
		break;
	}

	check_waiverule4->SetValue(!conf_data.game_data.waive_rule4);
	check_sluffjacks->SetValue(conf_data.game_data.sluff_jacks);


////@begin wxEVT_INIT_DIALOG event handler for ID_RADLGRULES in raDlgRules.
    // Before editing this code, remove the block markers.
    event.Skip();
////@end wxEVT_INIT_DIALOG event handler for ID_RADLGRULES in raDlgRules. 
}


