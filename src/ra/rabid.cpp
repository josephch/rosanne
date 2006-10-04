// rosanne : Twenty-Eight(28) Card Game
// Copyright (C) 2006 Vipin Cherian
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

#include "ra/rabid.h"
#include "ra/ragame.h"

BEGIN_EVENT_TABLE(raBid, wxPanel)
	EVT_BUTTON(raBID_BTN_ID_ALL, raBid::OnButtonClick)
	EVT_BUTTON(raBID_BTN_ID_PASS, raBid::OnButtonClick)
	//EVT_COMMAND_RANGE(wxEVT_COMMAND_BUTTON_CLICKED, raBID_BTN_ID_START, raBID_BTN_ID_MAX, wxCommandEventHandler(raBid::OnButtonClick))
END_EVENT_TABLE()

raBid::raBid(const wxWindow* parent): wxPanel((wxWindow*)parent)
{
	int i, j;
	int best_width;
	int temp_width, temp_height;

	m_game = NULL;

	// Initilizing buttons to NULL
	for(i = 0; i < raBID_TOTAL_BTNS; i++)
		m_buttons[i] = NULL;

	m_button_all = NULL;
	m_button_pass = NULL;

	// Calculate the best width for the buttons
	best_width = 0;
	for(i = 0 ; i < raBID_TOTAL_BTNS; i++)
	{
		this->GetTextExtent(wxString::Format(wxT("%d"), i + 14),
			&temp_width, &temp_height);
		best_width = raMax(best_width, temp_width);
	}

	this->GetTextExtent(wxT("All"), &temp_width, &temp_height);
	best_width = raMax(best_width, temp_width);
	this->GetTextExtent(wxT("Pass"), &temp_width, &temp_height);
	best_width = raMax(best_width, temp_width);
	wxLogDebug(wxString::Format("Best width %d", best_width));

#ifdef __WXMSW__
	this->SetWindowStyle(wxRAISED_BORDER);
#endif
	

	m_main_panel = new wxPanel(this);
#ifdef __WXMSW__
	m_main_panel->SetWindowStyle(wxSUNKEN_BORDER);
#endif
	m_main_panel->SetBackgroundColour(*wxWHITE);
	m_main_sizer = new wxGridSizer(0, 0, 0, 0);


	// TODO : Add error checks
	m_main_panel_sizer = new wxBoxSizer(wxVERTICAL);

	m_head_panel = new wxPanel(m_main_panel);
	m_head_panel->SetWindowStyle(wxRAISED_BORDER);
	m_head_panel->SetBackgroundColour(*wxBLACK);
	m_head_panel->SetForegroundColour(*wxWHITE);

	m_bold_font = m_head_panel->GetFont();
	m_bold_font.SetWeight(wxFONTWEIGHT_BOLD);
	m_head_panel->SetFont(m_bold_font);

	m_head_panel_sizer = new wxGridSizer(0, 0, 0, 0);
	m_head_panel_text = new wxStaticText(m_head_panel, -1, wxT("Enter Bid"));
	m_head_panel_sizer->Add(m_head_panel_text, 0, 
		wxALIGN_CENTER_HORIZONTAL| wxALIGN_CENTER_VERTICAL|wxALL, 2);

	m_head_panel->SetSizer(m_head_panel_sizer);

	m_bidbtn_panel = new wxPanel(m_main_panel);
	//m_bidbtn_panel->SetWindowStyle(wxSUNKEN_BORDER);
	m_bidbtn_panel_sizer = new wxGridSizer(5, 3, 0, 0);

	for(i = 0; i < raBID_BTN_ROWS; i++)
	{
		for(j = 0; j < raBID_BTN_COLS; j++)
		{
			m_buttons[(i * raBID_BTN_COLS) + j] = new wxButton(m_bidbtn_panel, 
				raBID_BTN_ID_START + (i * raBID_BTN_COLS) + j, 
				wxString::Format(wxT("%d"), (i * raBID_BTN_COLS) + j + 14),
				wxDefaultPosition, wxSize(best_width, -1));
			m_bidbtn_panel_sizer->Add(m_buttons[(i * raBID_BTN_COLS) + j], 0, 
				wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxEXPAND|wxALL, 1);
			//m_buttons[(i * raBID_BTN_COLS) + j]->SetSize(10, 20);
			//m_buttons[(i * raBID_BTN_COLS) + j]->SetWindowStyle(wxNO_BORDER);
			m_buttons[(i * raBID_BTN_COLS) + j]->SetEventHandler(this->GetEventHandler());
		}
	}

	m_bidbtn_panel->SetSizer(m_bidbtn_panel_sizer);
	m_bidbtn_panel_sizer->Fit(m_bidbtn_panel);

	// Create panel, assosiated sizer to hold the buttons 
	// to bid "All" and "Pass"

	m_btns_panel = new wxPanel(m_main_panel);
	m_btns_panel_sizer = new wxGridSizer(1, 2, 0, 0);

	m_button_all = new wxButton(m_btns_panel, raBID_BTN_ID_ALL, wxT("All"));
	m_button_pass = new wxButton(m_btns_panel, raBID_BTN_ID_PASS, wxT("Pass"));

	//m_button_all->SetWindowStyle(wxNO_BORDER);
	//m_button_pass->SetWindowStyle(wxNO_BORDER);
	
	m_button_all->SetEventHandler(this->GetEventHandler());
	m_button_pass->SetEventHandler(this->GetEventHandler());

	m_btns_panel_sizer->Add(m_button_all, 0, 
		wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxEXPAND|wxALL, 1);
	m_btns_panel_sizer->Add(m_button_pass, 0, 
		wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxEXPAND|wxALL, 1);

	m_btns_panel->SetSizer(m_btns_panel_sizer);
	m_btns_panel_sizer->Fit(m_btns_panel);

	m_main_panel_sizer->Add(m_head_panel, 0, wxEXPAND, 0);
	m_main_panel_sizer->Add(m_bidbtn_panel, 0, wxEXPAND, 0);
	m_main_panel_sizer->Add(m_btns_panel, 0, wxEXPAND, 0);

	m_main_sizer->Add(m_main_panel, 0, wxEXPAND, 0);

	m_main_panel->SetSizer(m_main_panel_sizer);
	m_main_panel_sizer->Fit(m_main_panel);

	this->SetSizer(m_main_sizer);
	m_main_sizer->Fit(this);

	this->GetEventHandler()->Connect(raBID_BTN_ID_START, raBID_BTN_ID_START + raBID_TOTAL_BTNS - 1, 
		wxEVT_COMMAND_BUTTON_CLICKED,wxCommandEventHandler(raBid::OnButtonClick));
}
raBid::~raBid()
{
}

// 
// Public method/s
//
bool raBid::SetGamePanel(raGame *game_panel)
{
	m_game = game_panel;
	return true;
}
bool raBid::SetPassable(bool passable)
{
	// Enable/disable the pass button
	// as per the input criteria

	if(passable)
		m_button_pass->Enable(true);
	else
		m_button_pass->Enable(false);

	return true;
}
bool raBid::SetMinimumBid(int min_bid)
{
	int i;

	// Disable all the bid buttons less than the minimum bid
	// and enable the rest

	for(i = 0; i < raBID_TOTAL_BTNS; i++)
	{
		if(i < (min_bid - 14))
			m_buttons[i]->Enable(false);
		else
			m_buttons[i]->Enable(true);
	}
	return true;
}

//
// Private method/s
//
void raBid::OnButtonClick(wxCommandEvent &event)
{
	raBidEvent new_event;

	if(m_game)
	{
		switch(event.GetId())
		{
		case raBID_BTN_ID_ALL:
			new_event.SetBid(raBID_ALL);
			break;
		case raBID_BTN_ID_PASS:
			new_event.SetBid(raBID_PASS);
			break;
		default:
			new_event.SetBid(event.GetId() - raBID_BTN_ID_START + 14);
			break;
		}
		m_game->AddPendingEvent(new_event);
	}
	else
		wxLogError(wxString::Format(wxT("Game panel not sent in raBid. %s:%d"), __FILE__, __LINE__));

	event.Skip();
}
