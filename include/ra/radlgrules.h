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
#ifndef _RADLGRULES_H_
#define _RADLGRULES_H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "radlgrules.h"
#endif

/*!
 * Includes
 */

////@begin includes
#include "wx/xrc/xmlres.h"
////@end includes

/*!
 * Forward declarations
 */

////@begin forward declarations
////@end forward declarations

/*!
 * Control identifiers
 */

////@begin control identifiers
#define ID_RADLGRULES 10010
#define SYMBOL_RADLGRULES_STYLE wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX
#define SYMBOL_RADLGRULES_TITLE _("Rules")
#define SYMBOL_RADLGRULES_IDNAME ID_RADLGRULES
#define SYMBOL_RADLGRULES_SIZE wxSize(400, 300)
#define SYMBOL_RADLGRULES_POSITION wxDefaultPosition
////@end control identifiers

/*!
 * Compatibility
 */

#ifndef wxCLOSE_BOX
#define wxCLOSE_BOX 0x1000
#endif

/*!
 * raDlgRules class declaration
 */

class raDlgRules: public wxDialog
{    
    DECLARE_DYNAMIC_CLASS( raDlgRules )
    DECLARE_EVENT_TABLE()

public:
    /// Constructors
    raDlgRules( );
    raDlgRules( wxWindow* parent, wxWindowID id = SYMBOL_RADLGRULES_IDNAME, const wxString& caption = SYMBOL_RADLGRULES_TITLE, const wxPoint& pos = SYMBOL_RADLGRULES_POSITION, const wxSize& size = SYMBOL_RADLGRULES_SIZE, long style = SYMBOL_RADLGRULES_STYLE );

    /// Creation
    bool Create( wxWindow* parent, wxWindowID id = SYMBOL_RADLGRULES_IDNAME, const wxString& caption = SYMBOL_RADLGRULES_TITLE, const wxPoint& pos = SYMBOL_RADLGRULES_POSITION, const wxSize& size = SYMBOL_RADLGRULES_SIZE, long style = SYMBOL_RADLGRULES_STYLE );

    /// Initialises member variables
    void Init();

    /// Creates the controls and sizers
    void CreateControls();

////@begin raDlgRules event handler declarations

    /// wxEVT_INIT_DIALOG event handler for ID_RADLGRULES
    void OnInitDialog( wxInitDialogEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_RULES_BTN_APPLY
    void OnRulesBtnApplyClick( wxCommandEvent& event );

////@end raDlgRules event handler declarations

////@begin raDlgRules member function declarations

    /// Retrieves bitmap resources
    wxBitmap GetBitmapResource( const wxString& name );

    /// Retrieves icon resources
    wxIcon GetIconResource( const wxString& name );
////@end raDlgRules member function declarations

    /// Should we show tooltips?
    static bool ShowToolTips();

////@begin raDlgRules member variables
////@end raDlgRules member variables
};

#endif
    // _RADLGRULES_H_
