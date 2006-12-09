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

#include "ra/rainfo.h"
#include "ra/ragame.h"

BEGIN_EVENT_TABLE(raInfo, wxPanel)
	EVT_SIZE(raInfo::OnSize)
	EVT_BUTTON(raINFO_CMD_BUTTON_ID, raInfo::OnButtonClick)
END_EVENT_TABLE()

raInfo::raInfo(const wxWindow* parent): wxPanel((wxWindow*)parent)
{
	int i, j;
	wxColour *colours[raINFO_DEAL_COUNT];

	m_main_sizer = NULL;
	m_deal_head_panel = NULL;
	m_game = NULL;
	m_curr_cmd = raINFO_CMD_NONE;

	ResetDetails();

	//colours[0] = new wxColour(74, 201, 255);
	//colours[1] = new wxColour(180, 233, 255);
	//colours[2] = new wxColour(125, 173, 193);
	//colours[3] = new wxColour(187, 210, 220);

	colours[0] = new raCLR_BLUE_DARK;
	colours[1] = new raCLR_BLUE_LITE;
	colours[2] = new raCLR_PURP_DARK;
	colours[3] = new raCLR_PURP_LITE;

	// Set the background white
	if(!SetBackgroundColour(*wxWHITE))
	{
		wxLogError(wxString::Format(wxT("Attempt to set the background colour failed. %s:%d"), __FILE__, __LINE__));
		return;
	}

	// Create the main sizer for the panel
	m_main_sizer = new wxBoxSizer(wxVERTICAL);
	wxASSERT(m_main_sizer);

	// Create the header panel
	m_deal_head_panel = new wxPanel(this);
	wxASSERT(m_deal_head_panel);

	//m_deal_head_panel->SetWindowStyle(wxRAISED_BORDER);
	// Set the background colour for the header panel
	if(!m_deal_head_panel->SetBackgroundColour(raCLR_HEAD_DARK))
	{
		wxLogError(wxString::Format(wxT("Attempt to set the background colour failed. %s:%d"), __FILE__, __LINE__));
		return;
	}

	// Set the foreground colour for the header panel
	if(!m_deal_head_panel->SetForegroundColour(*wxWHITE))
	{
		wxLogError(wxString::Format(wxT("Attempt to set the foreground colour failed. %s:%d"), __FILE__, __LINE__));
		return;
	}

	// Obtain the font assosiated with the header panel,
	// make it bold, and assosiate it back with the header panel
	m_bold_font = m_deal_head_panel->GetFont();
	if(!m_bold_font.Ok())
	{
		wxLogError(wxString::Format(wxT("The font obtained is not Ok. %s:%d"), __FILE__, __LINE__));
		return;
	}
	m_bold_font.SetWeight(wxFONTWEIGHT_BOLD);
	m_deal_head_panel->SetFont(m_bold_font);

	// Create sizer and add text
	m_deal_head_panel_sizer = new wxGridSizer(0, 0, 0, 0);
	wxASSERT(m_deal_head_panel_sizer);
	m_deal_head_panel_text = new wxStaticText(m_deal_head_panel, -1, wxT("Deal"));
	wxASSERT(m_deal_head_panel_text);
	if(!m_deal_head_panel_sizer->Add(m_deal_head_panel_text, 0, 
		wxALIGN_CENTER_HORIZONTAL| wxALIGN_CENTER_VERTICAL|wxALL, raINFO_TEXT_RELIEF))
	{
		wxLogError(wxString::Format(wxT("Addition of text control to sizer failed. %s:%d"), __FILE__, __LINE__));
		return;
	}

	// Set the sizer for the header panel
	m_deal_head_panel->SetSizer(m_deal_head_panel_sizer);
	if(!m_main_sizer->Add(m_deal_head_panel, 0, wxEXPAND|wxALL, raINFO_PNL_BORDER))
	{
		wxLogError(wxString::Format(wxT("Addition of panel to sizer failed. %s:%d"), __FILE__, __LINE__));
		return;
	}

	// Create the caption and text panels
	// Add sizers to each and eventually add text controls
	for(i = 0; i < raINFO_DEAL_COUNT; i++)
	{
		m_deal_info_sizers[i] = new wxBoxSizer(wxHORIZONTAL);
		wxASSERT(m_deal_info_sizers[i]);

		for(j = 0; j < raINFO_TWO; j++)
		{
			m_deal_info_panels[j][i] = new wxPanel(this);
			wxASSERT(m_deal_info_panels[j][i]);

			if(!m_deal_info_panels[j][i]->SetBackgroundColour(
				*colours[((i * raINFO_TWO) + j) % raINFO_DEAL_COUNT]))
			{
				wxLogError(wxString::Format(wxT("Attempt to set the background colour failed. %s:%d"), __FILE__, __LINE__));
				return;
			}
			if(!m_deal_info_sizers[i]->Add(m_deal_info_panels[j][i], wxEXPAND, wxALL, raINFO_PNL_BORDER))
			{
				wxLogError(wxString::Format(wxT("Attempt to add panel to sizer failed. %s:%d"), __FILE__, __LINE__));
				return;
			}

			m_deal_info_panel_sizers[j][i] = new wxGridSizer(0, 0, 0, 0);
			wxASSERT(m_deal_info_panel_sizers[j][i]);
			m_deal_info_panel_texts[j][i] = new wxStaticText(m_deal_info_panels[j][i], -1, wxT(""));
			wxASSERT(m_deal_info_panel_texts[j][i]);
			if(!m_deal_info_panel_sizers[j][i]-> Add(m_deal_info_panel_texts[j][i], 
				0, wxALIGN_LEFT| wxALIGN_CENTER_VERTICAL|wxALL, raINFO_TEXT_RELIEF))
			{
				wxLogError(wxString::Format(wxT("Attempt to add text control to sizer failed. %s:%d"), __FILE__, __LINE__));
				return;
			}
			m_deal_info_panels[j][i]->SetSizer(m_deal_info_panel_sizers[j][i]);

		}
		// Add individual panel sizers to the main sizer
		if(!m_main_sizer->Add(m_deal_info_sizers[i], 0, wxEXPAND, 0))
		{
			wxLogError(wxString::Format(wxT("Attempt to add sizer to sizer failed. %s:%d"), __FILE__, __LINE__));
			return;
		}
	}

	// Set the static information in the deal information panel
	m_deal_info_panel_texts[0][0]->SetLabel(raINFO_DEAL_NO_TEXT);
	m_deal_info_panel_texts[0][1]->SetLabel(raINFO_DEALER_TEXT);
	m_deal_info_panel_texts[0][2]->SetLabel(raINFO_BID_TEXT);
	m_deal_info_panel_texts[0][3]->SetLabel(raINFO_TRUMP_TEXT);

	//
	// Creation of panels and text controls to hold
	// information related to the point table
	//

	// Create the header panel  
	m_point_head_panel = new wxPanel(this);
	wxASSERT(m_point_head_panel);

	//m_point_head_panel->SetWindowStyle(wxRAISED_BORDER);
	// Set the background colour for the header panel
	if(!m_point_head_panel->SetBackgroundColour(raCLR_HEAD_DARK))
	{
		wxLogError(wxString::Format(wxT("Attempt to set the background colour failed. %s:%d"), __FILE__, __LINE__));
		return;
	}

	// Set the foreground colour for the header panel
	if(!m_point_head_panel->SetForegroundColour(*wxWHITE))
	{
		wxLogError(wxString::Format(wxT("Attempt to set the foreground colour failed. %s:%d"), __FILE__, __LINE__));
		return;
	}

	// Assosiate a bold font with the header panel
	m_point_head_panel->SetFont(m_bold_font);

	// Create sizer and add text
	m_point_head_panel_sizer = new wxGridSizer(0, 0, 0, 0);
	wxASSERT(m_point_head_panel_sizer);
	m_point_head_panel_text = new wxStaticText(m_point_head_panel, -1, raINFO_PTS_TEXT);
	wxASSERT(m_point_head_panel_text);
	if(!m_point_head_panel_sizer->Add(m_point_head_panel_text, 0, 
		wxALIGN_CENTER_HORIZONTAL| wxALIGN_CENTER_VERTICAL|wxALL, raINFO_TEXT_RELIEF))
	{
		wxLogError(wxString::Format(wxT("Addition of text control to sizer failed. %s:%d"), __FILE__, __LINE__));
		return;
	}

	// Set the sizer for the header panel
	m_point_head_panel->SetSizer(m_point_head_panel_sizer);
	// Add panel to sizer
	if(!m_main_sizer->Add(m_point_head_panel, 0, wxEXPAND | wxALL, raINFO_PNL_BORDER))
	{
		wxLogError(wxString::Format(wxT("Addition of panel to sizer failed. %s:%d"), __FILE__, __LINE__));
		return;
	}

	// Create the caption and text panels
	// Add sizers to each and eventually add text controls
	for(i = 0; i < raINFO_POINT_COUNT; i++)
	{
		m_point_info_sizers[i] = new wxBoxSizer(wxHORIZONTAL);
		wxASSERT(m_point_info_sizers[i]);

		for(j = 0; j < raINFO_TWO; j++)
		{
			m_point_info_panels[j][i] = new wxPanel(this);
			wxASSERT(m_point_info_panels[j][i]);

			if(!m_point_info_panels[j][i]->SetBackgroundColour(
				*colours[((i * raINFO_TWO) + j) % raINFO_DEAL_COUNT]))
			{
				wxLogError(wxString::Format(wxT("Attempt to set the background colour failed. %s:%d"), __FILE__, __LINE__));
				return;
			}
			if(!m_point_info_sizers[i]->Add(m_point_info_panels[j][i], wxEXPAND, wxALL, raINFO_PNL_BORDER))
			{
				wxLogError(wxString::Format(wxT("Attempt to add panel to sizer failed. %s:%d"), __FILE__, __LINE__));
				return;
			}

			m_point_info_panel_sizers[j][i] = new wxGridSizer(0, 0, 0, 0);
			wxASSERT(m_point_info_panel_sizers[j][i]);
			m_point_info_panel_texts[j][i] = new wxStaticText(m_point_info_panels[j][i], -1, wxT(""));
			wxASSERT(m_point_info_panel_texts[j][i]);
			if(!m_point_info_panel_sizers[j][i]-> Add(m_point_info_panel_texts[j][i], 
				0, wxALIGN_LEFT| wxALIGN_CENTER_VERTICAL|wxALL, raINFO_TEXT_RELIEF))
			{
				wxLogError(wxString::Format(wxT("Attempt to add text control to sizer failed. %s:%d"), __FILE__, __LINE__));
				return;
			}
			m_point_info_panels[j][i]->SetSizer(m_point_info_panel_sizers[j][i]);

		}
		// Add individual panel sizers to the main sizer
		if(!m_main_sizer->Add(m_point_info_sizers[i], 0, wxEXPAND, 0))
		{
			wxLogError(wxString::Format(wxT("Attempt to add sizer to sizer failed. %s:%d"), __FILE__, __LINE__));
			return;
		}
	}

	// Set the static information in the points panel
	m_point_info_panel_texts[0][0]->SetLabel(raINFO_NS_TEXT);
	m_point_info_panel_texts[0][1]->SetLabel(raINFO_EW_TEXT);

	//
	// Creation of panels and text controls to hold
	// information related to the penalty table
	//

	// Create the header panel 
	m_pnlty_head_panel = new wxPanel(this);
	wxASSERT(m_pnlty_head_panel);

	//m_pnlty_head_panel->SetWindowStyle(wxRAISED_BORDER);
	// Set the background colour for the header panel
	if(!m_pnlty_head_panel->SetBackgroundColour(raCLR_HEAD_DARK))
	{
		wxLogError(wxString::Format(wxT("Attempt to set the background colour failed. %s:%d"), __FILE__, __LINE__));
		return;
	}

	// Set the foreground colour for the header panel
	if(!m_pnlty_head_panel->SetForegroundColour(*wxWHITE))
	{
		wxLogError(wxString::Format(wxT("Attempt to set the foreground colour failed. %s:%d"), __FILE__, __LINE__));
		return;
	}

	// Assosiate a bold font with the header panel
	m_pnlty_head_panel->SetFont(m_bold_font);

	// Create sizer and add text
	m_pnlty_head_panel_sizer = new wxGridSizer(0, 0, 0, 0);
	wxASSERT(m_pnlty_head_panel_sizer);
	m_pnlty_head_panel_text = new wxStaticText(m_pnlty_head_panel, -1, raINFO_PEN_TEXT);
	wxASSERT(m_pnlty_head_panel_text);
	if(!m_pnlty_head_panel_sizer->Add(m_pnlty_head_panel_text, 0, 
		wxALIGN_CENTER_HORIZONTAL| wxALIGN_CENTER_VERTICAL|wxALL, raINFO_TEXT_RELIEF))
	{
		wxLogError(wxString::Format(wxT("Addition of text control to sizer failed. %s:%d"), __FILE__, __LINE__));
		return;
	}

	// Set the sizer for the header panel
	m_pnlty_head_panel->SetSizer(m_pnlty_head_panel_sizer);
	// Add panel to sizer
	if(!m_main_sizer->Add(m_pnlty_head_panel, 0, wxEXPAND | wxALL, raINFO_PNL_BORDER))
	{
		wxLogError(wxString::Format(wxT("Addition of panel to sizer failed. %s:%d"), __FILE__, __LINE__));
		return;
	}

	// Create the caption and text panels
	// Add sizers to each and eventually add text controls
	for(i = 0; i < raINFO_PNLTY_COUNT; i++)
	{
		m_pnlty_info_sizers[i] = new wxBoxSizer(wxHORIZONTAL);
		wxASSERT(m_pnlty_info_sizers[i]);

		for(j = 0; j < raINFO_TWO; j++)
		{
			m_pnlty_info_panels[j][i] = new wxPanel(this);
			wxASSERT(m_pnlty_info_panels[j][i]);

			if(!m_pnlty_info_panels[j][i]->SetBackgroundColour(
				*colours[((i * raINFO_TWO) + j) % raINFO_DEAL_COUNT]))
			{
				wxLogError(wxString::Format(wxT("Attempt to set the background colour failed. %s:%d"), __FILE__, __LINE__));
				return;
			}
			if(!m_pnlty_info_sizers[i]->Add(m_pnlty_info_panels[j][i], wxEXPAND, wxALL, raINFO_PNL_BORDER))
			{
				wxLogError(wxString::Format(wxT("Attempt to add panel to sizer failed. %s:%d"), __FILE__, __LINE__));
				return;
			}

			m_pnlty_info_panel_sizers[j][i] = new wxGridSizer(0, 0, 0, 0);
			wxASSERT(m_pnlty_info_panel_sizers[j][i]);
			m_pnlty_info_panel_texts[j][i] = new wxStaticText(m_pnlty_info_panels[j][i], -1, wxT(""));
			wxASSERT(m_pnlty_info_panel_texts[j][i]);
			if(!m_pnlty_info_panel_sizers[j][i]-> Add(m_pnlty_info_panel_texts[j][i], 
				0, wxALIGN_CENTER_HORIZONTAL| wxALIGN_CENTER_VERTICAL|wxALL, raINFO_TEXT_RELIEF))
			{
				wxLogError(wxString::Format(wxT("Attempt to add text control to sizer failed. %s:%d"), __FILE__, __LINE__));
				return;
			}
			m_pnlty_info_panels[j][i]->SetSizer(m_pnlty_info_panel_sizers[j][i]);

		}
		// Add individual panel sizers to the main sizer
		if(!m_main_sizer->Add(m_pnlty_info_sizers[i], 0, wxEXPAND, 0))
		{
			wxLogError(wxString::Format(wxT("Attempt to add sizer to sizer failed. %s:%d"), __FILE__, __LINE__));
			return;
		}
	}

	// Set the static information in the penalties panel
	//m_pnlty_info_panel_texts[0][0]->SetLabel(raINFO_NS_TEXT);
	//m_pnlty_info_panel_texts[0][1]->SetLabel(raINFO_EW_TEXT);

	//
	// Creation of panels and text controls to hold
	// information related to the instructions
	//

	// Create the header panel 
	m_instr_head_panel = new wxPanel(this);
	wxASSERT(m_instr_head_panel);

	//m_instr_head_panel->SetWindowStyle(wxRAISED_BORDER);
	// Set the background colour for the header panel
	if(!m_instr_head_panel->SetBackgroundColour(raCLR_HEAD_DARK))
	{
		wxLogError(wxString::Format(wxT("Attempt to set the background colour failed. %s:%d"), __FILE__, __LINE__));
		return;
	}

	// Set the foreground colour for the header panel
	if(!m_instr_head_panel->SetForegroundColour(*wxWHITE))
	{
		wxLogError(wxString::Format(wxT("Attempt to set the foreground colour failed. %s:%d"), __FILE__, __LINE__));
		return;
	}

	// Assosiate a bold font with the header panel
	m_instr_head_panel->SetFont(m_bold_font);

	// Create sizer and add text
	m_instr_head_panel_sizer = new wxGridSizer(0, 0, 0, 0);
	wxASSERT(m_instr_head_panel_sizer);
	m_instr_head_panel_text = new wxStaticText(m_instr_head_panel, -1, wxT("Instructions"));
	wxASSERT(m_instr_head_panel_text);
	if(!m_instr_head_panel_sizer->Add(m_instr_head_panel_text, 0, 
		wxALIGN_CENTER_HORIZONTAL| wxALIGN_CENTER_VERTICAL|wxALL, raINFO_TEXT_RELIEF))
	{
		wxLogError(wxString::Format(wxT("Addition of text control to sizer failed. %s:%d"), __FILE__, __LINE__));
		return;
	}

	// Set the sizer for the header panel
	m_instr_head_panel->SetSizer(m_instr_head_panel_sizer);
	// Add panel to sizer
	if(!m_main_sizer->Add(m_instr_head_panel, 0, wxEXPAND | wxALL, raINFO_PNL_BORDER))
	{
		wxLogError(wxString::Format(wxT("Addition of panel to sizer failed. %s:%d"), __FILE__, __LINE__));
		return;
	}

	// Create the text panel 
	m_instr_text_panel = new wxPanel(this);
	wxASSERT(m_instr_text_panel);

	// Set the background colour for the text panel
	if(!m_instr_text_panel->SetBackgroundColour(*wxWHITE))
	{
		wxLogError(wxString::Format(wxT("Attempt to set the background colour failed. %s:%d"), __FILE__, __LINE__));
		return;
	}

	// Set the foreground colour for the text panel
	if(!m_instr_text_panel->SetForegroundColour(raCLR_INSTR))
	{
		wxLogError(wxString::Format(wxT("Attempt to set the foreground colour failed. %s:%d"), __FILE__, __LINE__));
		return;
	}

	// Assosiate a bold font with the text panel
	m_instr_text_panel->SetFont(m_bold_font);

	// Create sizer and add text
	m_instr_text_panel_sizer = new wxGridSizer(0, 0, 0, 0);
	wxASSERT(m_instr_text_panel_sizer);
	m_instr_text_panel_text = new wxStaticText(m_instr_text_panel, -1, wxT(""));
	wxASSERT(m_instr_text_panel_text);
	if(!m_instr_text_panel_sizer->Add(m_instr_text_panel_text, wxEXPAND, 
		wxEXPAND | wxALL, raINFO_INSTR_RELIEF))
	{
		wxLogError(wxString::Format(wxT("Addition of text control to sizer failed. %s:%d"), __FILE__, __LINE__));
		return;
	}

	// Set the sizer for the text panel
	m_instr_text_panel->SetSizer(m_instr_text_panel_sizer);

	// Add panel to sizer
	if(!m_main_sizer->Add(m_instr_text_panel, wxEXPAND, wxEXPAND, 0))
	{
		wxLogError(wxString::Format(wxT("Addition of panel to sizer failed. %s:%d"), __FILE__, __LINE__));
		return;
	}

	// Add the command button
	m_button = new wxButton(this, raINFO_CMD_BUTTON_ID, wxT(""));
	m_main_sizer->Add(m_button, 0, wxRIGHT | wxLEFT | wxBOTTOM | wxEXPAND, 20);
	m_button->Show(false);

	// Hide the points information
	//m_main_sizer->Hide(m_point_head_panel, true);
	//for(i = 0; i < raINFO_POINT_COUNT; i++)
	//	for(j = 0; j < raINFO_TWO; j++)
	//		m_main_sizer->Hide(m_point_info_panels[i][j], true);


	// Set the sizer for this panel 
	SetSizer(m_main_sizer);
	m_main_sizer->Fit(this);

	SetDetails(&m_details);

	for(i = 0; i < raINFO_DEAL_COUNT; i++)
		delete colours[i];

}
raInfo::~raInfo()
{
	if(m_deal_head_panel)
		delete m_deal_head_panel;
	m_deal_head_panel = NULL;
	//if(m_main_sizer)
	//	delete m_main_sizer;
	//m_main_sizer = NULL;
}

bool raInfo::SetDetails(raInfoDetails *details)
{
	int i;

	// Validate input data and set the deal number
	if(details->deal_no < 0)
	{
		wxLogError(wxString::Format(wxT("Negative deal number passed. %s:%d"), __FILE__, __LINE__));
		return false;
	}
	m_deal_info_panel_texts[1][0]->
		SetLabel(wxString::Format(wxT("%d"), details->deal_no));

	// Validate input data and set the dealer
	if((details->dealer < raPLAYER_INVALID) || (details->dealer > raTOTAL_PLAYERS))
	{
		wxLogError(wxString::Format(wxT("Incorrect dealer. %s:%d"), __FILE__, __LINE__));
		return false;
	}
	if(details->dealer == raPLAYER_INVALID)
		m_deal_info_panel_texts[1][1]->
			SetLabel(wxT("N/A"));
	else
		m_deal_info_panel_texts[1][1]->
			SetLabel(wxString::Format(wxT("%s"), raLib::m_long_locs[details->dealer].c_str()));


	// Validate input data and set the bidder
	if((details->bidder < raPLAYER_INVALID) || (details->bidder > raTOTAL_PLAYERS))
	{
		wxLogError(wxString::Format(wxT("Incorrect bidder. %s:%d"), __FILE__, __LINE__));
		return false;
	}

	if(details->bidder == raPLAYER_INVALID)
		m_deal_info_panel_texts[1][2]->
			SetLabel(wxString::Format(wxT("N/A")));
	else
		m_deal_info_panel_texts[1][2]->
			SetLabel(wxString::Format(wxT("%d by %s"), details->bid, raLib::m_long_locs[details->bidder].c_str()));

	// Validate input data and set the trump
	if((details->trump < raSUIT_INVALID) || (details->trump > raTOTAL_SUITS))
	{
		wxLogError(wxString::Format(wxT("Incorrect trump suit. %s:%d"), __FILE__, __LINE__));
		return false;
	}

	if(details->trump == raSUIT_INVALID)
		m_deal_info_panel_texts[1][3]->
			SetLabel(wxString::Format(wxT("Not Shown")));
	else
		m_deal_info_panel_texts[1][3]->
			SetLabel(wxString::Format(wxT("%s"), raLib::m_suits[details->trump].c_str()));

	for(i = 0; i < raTOTAL_TEAMS; i++)
	{
		m_point_info_panel_texts[1][i]->SetLabel(wxString::Format(wxT("%d"), details->points[i]));
	}

	m_pnlty_info_panel_texts[0][0]->SetLabel(wxString::Format(wxT("%s-%d"), 
		raLib::m_short_locs[0].c_str(), details->pnlties[0]));
	m_pnlty_info_panel_texts[0][1]->SetLabel(wxString::Format(wxT("%s-%d"), 
		raLib::m_short_locs[1].c_str(), details->pnlties[1]));
	m_pnlty_info_panel_texts[1][0]->SetLabel(wxString::Format(wxT("%s-%d"), 
		raLib::m_short_locs[2].c_str(), details->pnlties[2]));
	m_pnlty_info_panel_texts[1][1]->SetLabel(wxString::Format(wxT("%s-%d"), 
		raLib::m_short_locs[3].c_str(), details->pnlties[3]));

	memcpy(&m_details, details, sizeof(raInfoDetails));
	return true;
}
void raInfo::GetDetails(raInfoDetails *details)
{
	memcpy(details, &m_details, sizeof(raInfoDetails));
}

bool raInfo::SetInstruction(wxString instruction, int cmd)
{
	unsigned int rand_seed;
#ifdef raREAD_SEED_FROM_FILE
	long seed_read;
#endif
	if((cmd != m_curr_cmd) || (cmd == raINFO_CMD_NONE))
	{
		m_instruction = instruction;

		// Depending on the command id passed,
		// enable/disable the command button
		// and set it's text accordingly
		switch(cmd)
		{
		case raINFO_CMD_NONE:
			m_button->Show(false);
			break;
		case raINFO_CMD_NEW_DEAL:
			m_button->Show(true);
			m_button->SetLabel(raINFO_DEAL_TEXT);
			if(m_instruction.IsEmpty())
				m_instruction = wxT("Click on the button below to start a new Deal.");
			//m_instruction.Append(wxT("Click on the button below to start a new Deal."));

			// Generate a random number and set it as the random seed
			// Also print the same so that games can be replicated

			rand_seed = rand();
			srand(rand_seed);
#ifdef raREAD_SEED_FROM_FILE
			if(::wxFileExists(raTEST_DATA_FILE))
			{
				wxFFileInputStream in(raTEST_DATA_FILE);
				wxFileConfig fcfg(in);
				if(fcfg.Exists(raTEXT_SEED))
				{
					wxLogDebug(wxString::Format(
						wxT("Reading seed from %s. %s:%d"),
						raTEST_DATA_FILE, __FILE__, __LINE__));

					seed_read = -1;
					if(!fcfg.Read(raTEXT_SEED, &seed_read))
					{
						wxLogError(wxString::Format(
							wxT("Read failed. %s:%d"), __FILE__, __LINE__));

					}
					else
					{
						rand_seed = (unsigned int)seed_read;
						srand(rand_seed);
					}
				}
				else
				{
					wxLogError(wxString::Format(
						wxT("Could not find seed in %s. %s:%d"),
						raTEST_DATA_FILE, __FILE__, __LINE__));
				}
			}
#endif
			wxLogMessage(wxString::Format("Deal ID - %u", rand_seed));
			//srand(11428);

			break;
		case raINFO_CMD_SHOW_TRUMP:
			m_button->Show(true);
			m_button->SetLabel(raINFO_SHOW_TRUMP_TEXT);
			break;
		default:
			wxLogError(wxString::Format(wxT("Unexpected value in switch statement. %s:%d"), __FILE__, __LINE__));
			return false;
		}
		m_curr_cmd = cmd;
	}

	// Set the instuction text, wrap and fit
	m_instr_text_panel_text->SetLabel(m_instruction);
	m_instr_text_panel_text->Wrap
		(this->GetClientSize().GetWidth() - (2 * raINFO_INSTR_RELIEF));
	m_instr_text_panel_sizer->Fit(m_instr_text_panel);

	return true;
}
bool raInfo::SetGamePanel(raGame *game_panel)
{
	wxASSERT(game_panel);
	m_game = game_panel;
	return true;
}

bool raInfo::ResetDetails(bool refresh)
{
	int i;
	m_details.bid = 0;
	m_details.bidder = raPLAYER_INVALID;
	m_details.dealer = raPLAYER_INVALID;
	m_details.deal_no = 1;
	for(i = 0; i < raTOTAL_TEAMS; i++)
	{
		m_details.points[i] = 5;
	}
	for(i = 0; i < raTOTAL_PLAYERS; i++)
	{
		m_details.pnlties[i] = 0;
	}
	m_details.trump = raSUIT_INVALID;
	if(refresh)
	{
		SetDetails(&m_details);
	}
	return true;
}

// 
// private methods
//

void raInfo::OnSize(wxSizeEvent &event)
{
	// Set the instruction again
	// to maintain the wrap
	SetInstruction(m_instruction, m_curr_cmd);

	// Pass the event on...
	event.Skip(true);
}
void raInfo::OnButtonClick(wxCommandEvent &event)
{
	wxASSERT(m_game);
	wxASSERT(m_curr_cmd != raINFO_CMD_NONE);

	raInfoEvent new_event;
	new_event.SetCommand(m_curr_cmd);
	m_game->AddPendingEvent(new_event);
}
