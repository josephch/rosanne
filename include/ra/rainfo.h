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
//#include "ra/ragame.h"

#ifdef raREAD_SEED_FROM_FILE
#include "wx/wfstream.h"
#include "wx/fileconf.h"
#endif

#define raINFO_DEAL_COUNT 4
#define raINFO_POINT_COUNT 2
#define raINFO_PNLTY_COUNT 2
#define raINFO_TWO 2
#define raINFO_TEXT_RELIEF 2
#define raINFO_INSTR_RELIEF 10
#define raINFO_PNL_BORDER 1

#define raINFO_TEXT_SHOW_TRUMP wxT("Show Trump")

#define raINFO_DEAL_NO_TEXT wxT("Deal Number")
#define raINFO_DEALER_TEXT wxT("Dealer")
#define raINFO_BID_TEXT wxT("Bid")
#define raINFO_TRUMP_TEXT wxT("Trump")

//#define raINFO_NS_TEXT wxT("North/South")
//#define raINFO_EW_TEXT wxT("East/West")
#define raINFO_NS_TEXT wxT("N/S")
#define raINFO_EW_TEXT wxT("E/W")

#define raINFO_PTS_TEXT wxT("Game Points")
#define raINFO_PEN_TEXT wxT("Penalties")



enum
{
	raINFO_CMD_NONE = 1,
	raINFO_CMD_NEW_DEAL,
	raINFO_CMD_SHOW_TRUMP
};

enum
{
	raINFO_CMD_BUTTON_ID = 100
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
	wxBoxSizer * m_main_sizer;

	wxPanel * m_deal_head_panel;
	wxGridSizer * m_deal_head_panel_sizer;
	wxStaticText * m_deal_head_panel_text;

	wxPanel * m_deal_info_panels[raINFO_TWO][raINFO_DEAL_COUNT];
	wxGridSizer * m_deal_info_panel_sizers[raINFO_TWO][raINFO_DEAL_COUNT];
	wxBoxSizer * m_deal_info_sizers[raINFO_DEAL_COUNT];
	wxStaticText * m_deal_info_panel_texts[raINFO_TWO][raINFO_DEAL_COUNT];

	wxPanel * m_point_head_panel;
	wxGridSizer * m_point_head_panel_sizer;
	wxStaticText * m_point_head_panel_text;

	wxPanel * m_point_info_panels[raINFO_TWO][raINFO_POINT_COUNT];
	wxGridSizer * m_point_info_panel_sizers[raINFO_TWO][raINFO_POINT_COUNT];
	wxBoxSizer * m_point_info_sizers[raINFO_POINT_COUNT];
	wxStaticText * m_point_info_panel_texts[raINFO_TWO][raINFO_POINT_COUNT];

	wxPanel * m_pnlty_head_panel;
	wxGridSizer * m_pnlty_head_panel_sizer;
	wxStaticText * m_pnlty_head_panel_text;

	wxPanel * m_pnlty_info_panels[raINFO_TWO][raINFO_PNLTY_COUNT];
	wxGridSizer * m_pnlty_info_panel_sizers[raINFO_TWO][raINFO_PNLTY_COUNT];
	wxBoxSizer * m_pnlty_info_sizers[raINFO_PNLTY_COUNT];
	wxStaticText * m_pnlty_info_panel_texts[raINFO_TWO][raINFO_PNLTY_COUNT];

	wxPanel * m_instr_head_panel;
	wxGridSizer * m_instr_head_panel_sizer;
	wxStaticText * m_instr_head_panel_text;

	//wxTextCtrl *m_instr_text_ctrl;
	wxPanel *m_instr_text_panel;
	wxGridSizer * m_instr_text_panel_sizer;
	wxStaticText * m_instr_text_panel_text;

	wxButton *m_button;
	wxFont m_bold_font;

	wxString m_instruction;
	int m_curr_cmd;

	raGame *m_game;
	raInfoDetails m_details;

	void OnSize(wxSizeEvent &event);
	void OnButtonClick(wxCommandEvent &event);
public:
	raInfo(const wxWindow* parent);
	~raInfo();
	bool SetDetails(raInfoDetails *details);
	void GetDetails(raInfoDetails *details);
	bool SetInstruction(wxString instruction, int cmd = raINFO_CMD_NONE);
	bool SetGamePanel(raGame *game_panel);
	bool ResetDetails(bool refresh = false);
};

#endif
