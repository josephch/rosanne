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

#ifndef _RAINFO_H
#define _RAINFO_H

#include "ra/racommon.h"
#include "ra/raevents.h"

#ifdef raREAD_SEED_FROM_FILE
#include "wx/wfstream.h"
#include "wx/fileconf.h"
#endif

enum
{
	raINFO_CMD_NONE = 1,
	raINFO_CMD_NEW_DEAL,
	raINFO_CMD_SHOW_TRUMP
};

#define raINFO_SHOW_TRUMP_TEXT wxT("Show Trump")
#define raINFO_DEAL_TEXT wxT("New Deal")

typedef struct tagraINFO_DETAILS
{
	int deal_no;
	int dealer;
	int bid;
	int bidder;
	int trump;
	int points[raTOTAL_TEAMS];
	int pnlties[raTOTAL_PLAYERS];
}raInfoDetails;

class raGame;

class raInfo: public wxPanel
{
private:
	DECLARE_EVENT_TABLE()

	wxButton *m_button;
	wxStaticText *m_dealno; 
	wxStaticText *m_dealer; 
	wxStaticText *m_bid; 
	wxStaticText *m_trump;

	wxStaticText *m_nspts;
	wxStaticText *m_ewpts;

	wxStaticText *m_spnlty;
	wxStaticText *m_npnlty;
	wxStaticText *m_epnlty;
	wxStaticText *m_wpnlty;

	wxStaticText *m_instr;


	int m_curr_cmd;
	raGame *m_game;
	raInfoDetails m_details;
	wxString m_instruction;

	void OnButtonClick(wxCommandEvent &event);
public:
	raInfo(wxWindow* parent);
	~raInfo();
	bool SetDetails(raInfoDetails *details);
	void GetDetails(raInfoDetails *details);
	bool SetInstruction(wxString instruction, int cmd = raINFO_CMD_NONE);
	bool SetGamePanel(raGame *game_panel);
	bool ResetDetails(bool refresh = false);
};

#endif
