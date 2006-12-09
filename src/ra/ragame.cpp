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

#include "ra/ragame.h"

#include "red_arrow_top.xpm"
#include "red_arrow_bottom.xpm"
#include "red_arrow_left.xpm"
#include "red_arrow_right.xpm"

#include "green_arrow_top.xpm"
#include "green_arrow_bottom.xpm"
#include "green_arrow_left.xpm"
#include "green_arrow_right.xpm"

#include "bubble_arrow_bottom.xpm"
#include "bubble_arrow_left.xpm"
#include "bubble_arrow_right.xpm"
#include "bubble_arrow_top.xpm"

#include "bubble_corner_ne.xpm"
#include "bubble_corner_nw.xpm"
#include "bubble_corner_se.xpm"
#include "bubble_corner_sw.xpm"

#include "bubble_edge_bottom.xpm"
#include "bubble_edge_left.xpm"
#include "bubble_edge_right.xpm"
#include "bubble_edge_top.xpm"

//#define raGAME_SHOW_BID_BUBBLES 0

BEGIN_EVENT_TABLE(raGame, ggPanel)
	EVT_SIZE(raGame::OnSize)
	EVT_INFO(raGame::OnInfo)
	EVT_BID(raGame::OnBid)
	EVT_LEFT_DCLICK(raGame::OnLeftDClick)
	EVT_LEFT_UP(raGame::OnLeftUp)
	//EVT_ERASE_BACKGROUND(wxcTable::OnErase)
END_EVENT_TABLE()

// TODO : For all wxMessageBox-es, provide appropraite heading, icon etc

//
// Constructor/s
//

raGame::raGame(const wxWindow* parent): ggPanel((wxWindow*)parent)
{
	// TODO : Consider each item and move to Reset if found appropriate
	int i, j;
	ggCard *card = NULL;

	m_hand_rot = 0;
	m_tile = NULL;
	m_info = NULL;
	m_bid = NULL;
	m_clockwise = true;
	m_pref_card_back = raCONFIG_PREFS_CARDBACK_BLUE;
	m_play_card_on = raCONFIG_PREFS_PLAYCARDON_SCLICK;
	m_auto_play = true;


	// Set the properties of the bold font to be used to write the text
	// especially locations
	m_font_bold = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
	m_font_bold.SetWeight(wxFONTWEIGHT_BOLD);

	// Create bitmaps from xpms 

	m_bmp_green_arrow_bottom = new wxBitmap(green_arrow_bottom_xpm);
	m_bmp_green_arrow_top = new wxBitmap(green_arrow_top_xpm);
	m_bmp_green_arrow_right = new wxBitmap(green_arrow_right_xpm);
	m_bmp_green_arrow_left = new wxBitmap(green_arrow_left_xpm);

	m_bmp_red_arrow_bottom = new wxBitmap(red_arrow_bottom_xpm);
	m_bmp_red_arrow_top = new wxBitmap(red_arrow_top_xpm);
	m_bmp_red_arrow_right = new wxBitmap(red_arrow_right_xpm);
	m_bmp_red_arrow_left = new wxBitmap(red_arrow_left_xpm);

	m_bmp_bubble_arrow_bottom = new wxBitmap(bubble_arrow_bottom_xpm);
	m_bmp_bubble_arrow_left = new wxBitmap(bubble_arrow_left_xpm);
	m_bmp_bubble_arrow_right = new wxBitmap(bubble_arrow_right_xpm);
	m_bmp_bubble_arrow_top = new wxBitmap(bubble_arrow_top_xpm);

	m_bmp_bubble_corner_ne = new wxBitmap(bubble_corner_ne_xpm);
	m_bmp_bubble_corner_nw = new wxBitmap(bubble_corner_nw_xpm);
	m_bmp_bubble_corner_se = new wxBitmap(bubble_corner_se_xpm);
	m_bmp_bubble_corner_sw = new wxBitmap(bubble_corner_sw_xpm);

	m_bmp_bubble_edge_bottom = new wxBitmap(bubble_edge_bottom_xpm);
	m_bmp_bubble_edge_left = new wxBitmap(bubble_edge_left_xpm);
	m_bmp_bubble_edge_right = new wxBitmap(bubble_edge_right_xpm);
	m_bmp_bubble_edge_top = new wxBitmap(bubble_edge_top_xpm);

	ResetGame();

	// Initiate the bitmap pointer array for card faces to NULL
	for(i = 0; i < raTOTAL_PLAYERS; i++)
	{
		m_players[i].SetLocation(i);
		m_players[i].SetType(raPLAYER_TYPE_AI);
	}
	m_players[0].SetType(raPLAYER_TYPE_HUMAN);

	for(i = 0; i < raTOTAL_CARDS; i++)
		m_card_faces[i] = NULL;

	// Initiate the bitmap pointer array for card backs to NULL
	for(i = 0; i < raTOTAL_CARD_BACKS; i++)
		m_card_backs[i] = NULL;

	// Load card faces 
	for(i = 0; i < raTOTAL_SUITS; i++)
	{
		for(j = 0; j < raTOTAL_VALUES; j++)
		{
			card = new ggCard(i, raLib::m_value_trans[j]);
			wxASSERT(card);
			m_card_faces[(i * raTOTAL_VALUES) + j] = new wxBitmap(*card->GetFace());
			if(!m_card_faces[(i * raTOTAL_VALUES) + j])
			{
				wxLogError(wxString::Format(wxT("Bitmap creation failed. %s:%d"), __FILE__, __LINE__));
				return;
			}
			delete card;
			card = NULL;
		}
	}

	// Load card backs
	card = new ggCard(GG_CARD_BACK_1);
	wxASSERT(card);
	m_card_backs[0] = new wxBitmap(*card->GetFace());
	if(!m_card_backs[0])
	{
		wxLogError(wxString::Format(wxT("Bitmap creation failed. %s:%d"), __FILE__, __LINE__));
		return;
	}
	delete card;
	card = NULL;

	card = new ggCard(GG_CARD_BACK_2);
	wxASSERT(card);
	m_card_backs[1] = new wxBitmap(*card->GetFace());
	if(!m_card_backs[1])
	{
		wxLogError(wxString::Format(wxT("Bitmap creation failed. %s:%d"), __FILE__, __LINE__));
		return;
	}
	delete card;
	card = NULL;

	ReloadFromConfig();

	// TODO : Should DrawBack be called here?
	m_bid = new raBid(this);
	//m_bid->SetSize(2 * GG_CARD_WIDTH, 2 * GG_CARD_HEIGHT);
	//m_bid->SetWindowStyle(wxRAISED_BORDER);
	//m_bid->Refresh();
	m_bid->Show(false);
	m_bid->SetGamePanel(this);
}

//
// Destructor/s
//
raGame::~raGame()
{
	int i, j;

	// Delete card faces
	for(i = 0; i < raTOTAL_SUITS; i++)
	{
		for(j = 0; j < raTOTAL_VALUES; j++)
		{
			if(m_card_faces[(i * raTOTAL_VALUES) + j])
				delete m_card_faces[(i * raTOTAL_VALUES) + j];
			m_card_faces[(i * raTOTAL_VALUES) + j] = NULL;
		}
	}

	// Delete card backs
	for(i = 0; i < raTOTAL_CARD_BACKS; i++)
	{
		if(m_card_backs[i])
			delete m_card_backs[i];
		m_card_backs[i] = NULL;
	}

	// Delete tile bitmap
	if(m_tile)
		delete m_tile;

	// Delete the bitmaps created from xpms

	if(m_bmp_green_arrow_bottom)
		delete m_bmp_green_arrow_bottom;
	if(m_bmp_green_arrow_top)
		delete m_bmp_green_arrow_top;
	if(m_bmp_green_arrow_right)
		delete m_bmp_green_arrow_right;
	if(m_bmp_green_arrow_left)
		delete m_bmp_green_arrow_left;

	if(m_bmp_red_arrow_bottom)
		delete m_bmp_red_arrow_bottom;
	if(m_bmp_red_arrow_top)
		delete m_bmp_red_arrow_top;
	if(m_bmp_red_arrow_right)
		delete m_bmp_red_arrow_right;
	if(m_bmp_red_arrow_left)
		delete m_bmp_red_arrow_left;

	if(m_bmp_bubble_arrow_bottom)
		delete m_bmp_bubble_arrow_bottom;
	if(m_bmp_bubble_arrow_left)
		delete m_bmp_bubble_arrow_left;
	if(m_bmp_bubble_arrow_right)
		delete m_bmp_bubble_arrow_right;
	if(m_bmp_bubble_arrow_top)
		delete m_bmp_bubble_arrow_top;

	if(m_bmp_bubble_corner_ne)
		delete m_bmp_bubble_corner_ne;
	if(m_bmp_bubble_corner_nw)
		delete m_bmp_bubble_corner_nw;
	if(m_bmp_bubble_corner_se)
		delete m_bmp_bubble_corner_se;
	if(m_bmp_bubble_corner_sw)
		delete m_bmp_bubble_corner_sw;

	if(m_bmp_bubble_edge_bottom)
		delete m_bmp_bubble_edge_bottom;
	if(m_bmp_bubble_edge_left)
		delete m_bmp_bubble_edge_left;
	if(m_bmp_bubble_edge_right)
		delete m_bmp_bubble_edge_right;
	if(m_bmp_bubble_edge_top)
		delete m_bmp_bubble_edge_top;

	m_tile = NULL;
}

//
// Public method/s
//

bool raGame::SetTile(wxBitmap *tile)
{
	wxASSERT(tile);

	// Cleanup if m_tile is already created
	if(m_tile)
		delete m_tile;
	m_tile = NULL;

	// Create a new tile bitmap
	m_tile = new wxBitmap(*tile);
	if(!m_tile)
	{
		wxLogError(wxString::Format(wxT("Attempt create bitmap failed. %s:%d"), __FILE__, __LINE__));
		return false;
	}

	// As the tile has now changed, redraw the back buffer
	if(!RedrawBack())
	{
		wxLogError(wxString::Format(wxT("Attempt to redraw back buffer failed. %s:%d"), __FILE__, __LINE__));
		return false;
	}

	return true;
}

bool raGame::SetInfoPanel(raInfo *info_panel)
{
	wxASSERT(info_panel);
	m_info = info_panel;
	return true;
}
bool raGame::NewGame(int dealer)
{
	if(!ResetGame())
	{
		wxLogError(wxString::Format(wxT("ResetGame failed. %s:%d"), __FILE__, __LINE__));
		return false;
	}
	if(dealer == raPLAYER_INVALID)
		dealer = 0;

	m_engine.SetDealer(dealer);

	m_info->ResetDetails(true);
	m_info->SetInstruction(wxT("New Game started. Please click on the button below to deal cards."), 
		raINFO_CMD_NEW_DEAL);
	return true;
}

bool raGame::SetClockwise(bool flag)
{
	raRuleEngineData data;
	int i;

	// TODO : Add appropriate checks as to when this can be done.

	// Set the direction of game play for the rule engine
	m_engine.GetData(&data);
	if(flag)
		data.rules.rot_addn = 1;
	else
		data.rules.rot_addn = 3;
	m_engine.SetData(&data, false);

	// TODO : Set the direction of game play for the AI players
	for(i = 0; i < raTOTAL_PLAYERS; i++)
	{
		m_players[i].SetClockwise(flag);
	}

	m_clockwise = flag;
	return true;
}
bool raGame::GetClockwise()
{
	return m_clockwise;
}
bool raGame::ReloadFromConfig()
{
	raConfData conf_data;

	raConfig::GetInstance()->GetData(&conf_data);
	
	m_pref_card_back = conf_data.prefs_data.card_back;
	m_play_card_on = conf_data.prefs_data.play_card_on;
	m_auto_play =conf_data.prefs_data.auto_play_single;

	UpdateDrawAndRefresh();

	return true;
}


//
// Private method/s
//
void raGame::OnSize(wxSizeEvent& event)
{
	wxPaintEvent new_event;

	// Set the location of the bid panel
	//m_bid->SetSize(
	//	(this->GetClientSize().GetWidth() - m_bid->GetSize().GetWidth()) / 2,
	//	(this->GetClientSize().GetHeight() - m_bid->GetSize().GetHeight()) / 2,
	//	m_bid->GetSize().GetWidth(),
	//	m_bid->GetSize().GetHeight()
	//	);
	m_bid->Move(
		(this->GetClientSize().GetWidth() - m_bid->GetSize().GetWidth()) / 2,
		(this->GetClientSize().GetHeight() - m_bid->GetSize().GetHeight()) / 2
		);

	// Get all the elements assosiated with this panel resized
	if(!Size())
		wxLogError(wxString::Format(wxT("Attempt to size the panel failed. %s:%d"), __FILE__, __LINE__));

	// Redraw the back buffer
	/*if(!RedrawBack())
	{
		wxLogError(wxString::Format(wxT("Attempt to redraw back buffer failed. %s:%d"), __FILE__, __LINE__));
	}

	// Refresh the screen after drawing the back buffer
	if(!RefreshScreen())
	{
		wxLogError(wxString::Format(wxT("Call to raGame::RefreshScreen() failed. %s:%d"), __FILE__, __LINE__));
	}*/
	if(!UpdateDrawAndRefresh(false))
	{
		wxLogError(wxString::Format(wxT("Call to UpdateDrawAndRefresh() failed. %s:%d"), __FILE__, __LINE__));
	}

	event.Skip();
}
bool raGame::RedrawBack(raBackDrawInfo *info)
{
	int i, j, x, y, wt, ht, wb, hb;
	wxMemoryDC mdc;
	wxString loc_text;
	int text_width, text_height;
	int horz_pnl_relief = 0, vert_pnl_relief = 0;

	//wxBitmap bmp_green_arrow_bottom(green_arrow_bottom_xpm);
	//wxBitmap bmp_green_arrow_top(green_arrow_top_xpm);
	//wxBitmap bmp_green_arrow_right(green_arrow_right_xpm);
	//wxBitmap bmp_green_arrow_left(green_arrow_left_xpm);

	// Obtain the dimensions of the back buffer
	wb = 0; 
	hb = 0;
	this->GetClientSize(&wb, &hb);

	// If there is a valid tile, draw the tile all over the place
	if(m_tile)
	{
		// Obtain the dimensions of the tile
		wt = m_tile->GetWidth();
		ht = m_tile->GetHeight();

		// Obtain the dimensions of the back buffer
		wb = 0; 
		hb = 0;
		this->GetClientSize(&wb, &hb);

		// Calculate the number of times the tile
		// has to blitted to the back buffer vertically
		// and horizontally
		x = (wb / wt) + !!(wb % wt);
		y = (hb / ht) + !!(hb % ht);

		// Blit repeatedly
		mdc.SelectObject(*m_tile);

		for(i = 0; i < x; i++)
		{
			for(j = 0; j < y;  j++)
			{
				if(!BlitToBack(i * wt, j * ht, wt, ht, &mdc, 0, 0))
				{
					wxLogError(wxString::Format(wxT("Blit to back buffer failed. %s:%d"), __FILE__, __LINE__));
					return false;
				}
			}
		}

		// Cleanup the object selection
		mdc.SelectObject(wxNullBitmap);
	}

	// Invalidate the existing hand positions and dimensions
	for(i = 0; i < raTOTAL_PLAYERS; i++)
		m_hand_rects[i] = wxRect(0, 0, 0, 0);

	mdc.SetFont(m_font_bold);
	//
	// Draw the location text at the bottom
	//

	i = (m_hand_rot * (raTOTAL_PLAYERS - 1)) % 4;

	loc_text = raLib::m_long_locs[i];
	mdc.GetTextExtent(loc_text, &text_width, &text_height);
	x = (wb - text_width) / 2;
	y = hb - text_height;
	DrawTextOnBack(loc_text, wxPoint(x + 1, y + 1), *wxBLACK, m_font_bold);
	DrawTextOnBack(loc_text, wxPoint(x, y), *wxWHITE, m_font_bold);

	vert_pnl_relief = raMax(vert_pnl_relief, text_height);

	//
	// Draw the location text at the left
	//

	i = ((m_hand_rot * (raTOTAL_PLAYERS - 1)) + 1) % 4;

	loc_text = raLib::m_long_locs[i];
	mdc.GetTextExtent(loc_text, &text_width, &text_height);
	x = 0;
	y = (hb - text_height) / 2;
	DrawTextOnBack(loc_text, wxPoint(x + 1, y + 1), *wxBLACK, m_font_bold);
	DrawTextOnBack(loc_text, wxPoint(x, y), *wxWHITE, m_font_bold);

	horz_pnl_relief = raMax(horz_pnl_relief, text_width);

	//
	// Draw the text for the location at top
	//

	i = ((m_hand_rot * (raTOTAL_PLAYERS - 1)) + 2) % 4;

	loc_text = raLib::m_long_locs[i];
	mdc.GetTextExtent(loc_text, &text_width, &text_height);
	x = (wb - text_width) / 2;
	y = 0;
	DrawTextOnBack(loc_text, wxPoint(x + 1, y + 1), *wxBLACK, m_font_bold);
	DrawTextOnBack(loc_text, wxPoint(x, y), *wxWHITE, m_font_bold);

	vert_pnl_relief = raMax(vert_pnl_relief, text_height);

	//
	// Draw the location text at the right
	//

	i = ((m_hand_rot * (raTOTAL_PLAYERS - 1)) + 3) % 4;

	loc_text = raLib::m_long_locs[i];
	mdc.GetTextExtent(loc_text, &text_width, &text_height);
	x = wb - text_width;
	y = (hb - text_height) / 2;
	DrawTextOnBack(loc_text, wxPoint(x + 1, y + 1), *wxBLACK, m_font_bold);
	DrawTextOnBack(loc_text, wxPoint(x, y), *wxWHITE, m_font_bold);

	horz_pnl_relief = raMax(horz_pnl_relief, text_width);

	//
	// Draw the hand to be shown at the bottom
	//

	i = (m_hand_rot * (raTOTAL_PLAYERS - 1)) % 4;

	// Calculate the starting position to draw
	x = (wb - (GG_CARD_WIDTH + ((m_hands[i].count - 1) * raCARD_HORZ_RELIEF))) / 2;
	y = hb - raMax(raCARD_PANEL_RELIEF, vert_pnl_relief) - GG_CARD_HEIGHT;

	// Draw the hand at the calculated location
	DrawHand(i, x, y);

	//
	// Draw the hand to be shown at the left
	//

	i = ((m_hand_rot * (raTOTAL_PLAYERS - 1)) + 1) % 4;

	// Calculate the starting position to draw
	if(m_orientation == raGAME_ORIENT_ALL_HORZ)
	{
		x = raMax(raCARD_PANEL_RELIEF, horz_pnl_relief);
		y = (hb - GG_CARD_HEIGHT) / 2;

		// Draw the hand at the calculated location
		DrawHand(i, x, y);
	}
	else
	{
		x = raMax(raCARD_PANEL_RELIEF, horz_pnl_relief);
		y = (hb - (GG_CARD_HEIGHT + ((m_hands[i].count - 1) * raCARD_VERT_RELIEF))) / 2;

		// Draw the hand at the calculated location
		DrawHand(i, x, y, raHAND_VERTICAL);
	}

	//
	// Draw the hand to be shown at the top
	//

	i = ((m_hand_rot * (raTOTAL_PLAYERS - 1)) + 2) % 4;

	// Calculate the starting position to draw
	x = (wb - (GG_CARD_WIDTH + ((m_hands[i].count - 1) * raCARD_HORZ_RELIEF))) / 2;
	y = raMax(raCARD_PANEL_RELIEF, vert_pnl_relief);

	// Draw the hand at the calculated location
	DrawHand(i, x, y);

	//
	// Draw the hand to be shown at the right
	//

	i = ((m_hand_rot * (raTOTAL_PLAYERS - 1)) + 3) % 4;

	// Calculate the starting position to draw
	if(m_orientation == raGAME_ORIENT_ALL_HORZ)
	{
		x = (wb - raMax(raCARD_PANEL_RELIEF, horz_pnl_relief) - 
			(GG_CARD_WIDTH + ((m_hands[i].count - 1) * raCARD_HORZ_RELIEF)));
		y = (hb - GG_CARD_HEIGHT) / 2;
		// Draw the hand at the calculated location
		DrawHand(i, x, y);

	}
	else
	{
		x = (wb - raMax(raCARD_PANEL_RELIEF, horz_pnl_relief) - GG_CARD_WIDTH);
		y = (hb - (GG_CARD_HEIGHT + ((m_hands[i].count - 1) * raCARD_VERT_RELIEF))) / 2;
		// Draw the hand at the calculated location
		DrawHand(i, x, y, raHAND_VERTICAL);
	}

	// Show green arrow for the next card to be played
	if(m_trick.count < raTOTAL_PLAYERS)
	{
		i = m_engine.GetTrickNextToPlay();	
		if(i != raPLAYER_INVALID)
		{
			switch(i)
			{
			case 0:
				mdc.SelectObject(*m_bmp_green_arrow_top);
				x = (wb - raARROW_WIDTH) / 2;
				y = m_hand_rects[0].GetTop() - raARROW_WIDTH - raGAME_ARROW_RELIEF;
				//x = m_hand_rects[0].GetRight() + raGAME_ARROW_RELIEF;
				//y = (hb - raARROW_WIDTH) / 2;
				break;
			case 1:
				mdc.SelectObject(*m_bmp_green_arrow_right);
				x = m_hand_rects[1].GetRight() + raGAME_ARROW_RELIEF;
				y = (hb - raARROW_WIDTH) / 2;
				//x = (wb - raARROW_WIDTH) / 2;
				//y = m_hand_rects[1].GetBottom() + raGAME_ARROW_RELIEF;
				break;
			case 2:
				mdc.SelectObject(*m_bmp_green_arrow_bottom);
				x = (wb - raARROW_WIDTH) / 2;
				y = m_hand_rects[2].GetBottom() + raGAME_ARROW_RELIEF;
				//x = m_hand_rects[2].GetLeft() - raARROW_WIDTH - raGAME_ARROW_RELIEF;
				//y = (hb - raARROW_WIDTH) / 2;
				break;
			case 3:
				mdc.SelectObject(*m_bmp_green_arrow_left);
				x = m_hand_rects[3].GetLeft() - raARROW_WIDTH - raGAME_ARROW_RELIEF;
				y = (hb - raARROW_WIDTH) / 2;
				//x = (wb - raARROW_WIDTH) / 2;
				//y = m_hand_rects[3].GetTop() - raARROW_WIDTH - raGAME_ARROW_RELIEF;
				break;
			}
			BlitToBack(x, y, raARROW_WIDTH, raARROW_WIDTH, &mdc, 0, 0, wxCOPY, true);
		}
	}


	// Draw the cards in the current trick
	DrawTrick();

	// Draw the trump card
	DrawTrump();

	// Draw the text for the trump
	loc_text = wxT("Trump");
	mdc.GetTextExtent(loc_text, &text_width, &text_height);
	mdc.GetTextExtent(loc_text, &text_width, &text_height);
	x = raCARD_PANEL_RELIEF + ((GG_CARD_WIDTH - text_width) / 2);
	y = 0;
	DrawTextOnBack(loc_text, wxPoint(x + 1, y + 1), *wxBLACK, m_font_bold);
	DrawTextOnBack(loc_text, wxPoint(x, y), *wxWHITE, m_font_bold);

#ifdef raGAME_SHOW_BID_BUBBLES
	// If required, draw the bubble which has the bid
	if(info)
	{
		if(info->draw_bid)
		{
			wxMemoryDC temp_dc1, temp_dc2;
			wxString bubb_text;
			int u, v;

			if(info->bid == raBID_PASS)
				bubb_text = wxString::Format("%s passes the bid", raLib::m_long_locs[info->bid_loc].c_str());
			else
				bubb_text = wxString::Format("%s bids %d", raLib::m_long_locs[info->bid_loc].c_str(), info->bid);

			u = 0;
			v = 0;
			
			mdc.SetFont(m_font_bold);

			mdc.GetTextExtent(bubb_text, &u, &v);

			// TODO : Remove hard coding of the relief

			x = raMax((u + 8), raBUBB_MIN_WIDTH);
			y = raMax((v + 8), raBUBB_MIN_HEIGHT);

			// Increment x and y to the equal or higher multiple of raBUBB_UNIT_MIN
			x = ((x / raBUBB_UNIT_MIN) + (!!(x % raBUBB_UNIT_MIN))) * raBUBB_UNIT_MIN;
			y = ((y / raBUBB_UNIT_MIN) + (!!(y % raBUBB_UNIT_MIN))) * raBUBB_UNIT_MIN;

			wxBitmap bmp_bubb_rect(x, y);

			mdc.SelectObject(bmp_bubb_rect);
			mdc.SetTextForeground(*wxBLACK);

			// Fill with the masking colour
			mdc.SetBrush(wxBrush(*wxRED));
			mdc.SetPen(wxPen(*wxRED, 0));
			mdc.DrawRectangle(wxRect(0, 0, x, y));

			// Fill with the background colour for the bubble
			mdc.SetBrush(wxBrush(raCLR_BUBB_FILL));
			mdc.SetPen(wxPen(raCLR_BUBB_FILL, 0));
			mdc.DrawRectangle(wxRect(raBUBB_UNIT_MIN, raBUBB_UNIT_MIN, 
				x - (2 * raBUBB_UNIT_MIN), y - (2 * raBUBB_UNIT_MIN)));

			// Draw corners
			temp_dc1.SelectObject(*m_bmp_bubble_corner_nw);
			mdc.Blit(0, 0, raBUBB_UNIT_MIN, raBUBB_UNIT_MIN, &temp_dc1, 0, 0, wxCOPY, true);
			temp_dc1.SelectObject(*m_bmp_bubble_corner_ne);
			mdc.Blit(x - raBUBB_UNIT_MIN, 0, raBUBB_UNIT_MIN, raBUBB_UNIT_MIN, &temp_dc1, 0, 0, wxCOPY, true);
			temp_dc1.SelectObject(*m_bmp_bubble_corner_sw);
			mdc.Blit(0, y - raBUBB_UNIT_MIN, raBUBB_UNIT_MIN, raBUBB_UNIT_MIN, &temp_dc1, 0, 0, wxCOPY, true);
			temp_dc1.SelectObject(*m_bmp_bubble_corner_se);
			mdc.Blit(x - raBUBB_UNIT_MIN, y - raBUBB_UNIT_MIN, raBUBB_UNIT_MIN, raBUBB_UNIT_MIN, &temp_dc1, 0, 0, wxCOPY, true);

			// Draw the top and bottom edges
			temp_dc1.SelectObject(*m_bmp_bubble_edge_top);
			temp_dc2.SelectObject(*m_bmp_bubble_edge_bottom);
			for(i = 1; i <= ((x / raBUBB_UNIT_MIN) - 2); i++)
			{
				mdc.Blit(i * raBUBB_UNIT_MIN, 0, raBUBB_UNIT_MIN, raBUBB_UNIT_MIN, &temp_dc1, 0, 0, wxCOPY, true);
				mdc.Blit(i * raBUBB_UNIT_MIN, y - raBUBB_UNIT_MIN, raBUBB_UNIT_MIN, raBUBB_UNIT_MIN, &temp_dc2, 0, 0, wxCOPY, true);
			}

			// Draw the left and right edges
			temp_dc1.SelectObject(*m_bmp_bubble_edge_left);
			temp_dc2.SelectObject(*m_bmp_bubble_edge_right);
			for(i = 1; i <= ((y / raBUBB_UNIT_MIN) - 2); i++)
			{
				mdc.Blit(0, i * raBUBB_UNIT_MIN, raBUBB_UNIT_MIN, raBUBB_UNIT_MIN, &temp_dc1, 0, 0, wxCOPY, true);
				mdc.Blit(x - raBUBB_UNIT_MIN, i * raBUBB_UNIT_MIN, raBUBB_UNIT_MIN, raBUBB_UNIT_MIN, &temp_dc2, 0, 0, wxCOPY, true);
			}

			//mdc.Blit(10, 0, raBUBB_UNIT_MIN, raBUBB_UNIT_MIN, &temp_dc1, 0, 0, wxCOPY, true);

			mdc.SelectObject(wxNullBitmap);

			// Create a mask for the bitmap
			// (There is no need to delete the mask
			// wxWidgets will take care of it. Koppu!)
			wxMask *mask_bubb_rect = new wxMask(bmp_bubb_rect, *wxRED);
			bmp_bubb_rect.SetMask(mask_bubb_rect);

			// Draw the text
			mdc.SelectObject(bmp_bubb_rect);
			wxLogDebug(wxString::Format("x - %d u - %d", x, u));
			mdc.DrawText(bubb_text, wxPoint((x - u) / 2, (y - v) / 2));
			mdc.SelectObject(wxNullBitmap);

			// Create a new bitmap which will hold the bubble
			// rectangle as well as the arrow

			// u and v will contain the dimensions of the full bubble
			// i and j will contain the position of the bubble

			wxBitmap *bmp_bubb_full = NULL;
			switch(info->bid_loc)
			{
			case 0:
				u = x;
				v = y + raBUBB_ARROW_PROTUN; 
				bmp_bubb_full = new wxBitmap(u, v);
				mdc.SelectObject(*bmp_bubb_full);
				mdc.SetBrush(wxBrush(*wxRED));
				mdc.SetPen(wxPen(*wxRED, 0));
				mdc.DrawRectangle(wxRect(0, 0, u, v));
				temp_dc1.SelectObject(bmp_bubb_rect);
				mdc.Blit(0, 0, x, y, &temp_dc1, 0, 0, wxCOPY, true);
				temp_dc1.SelectObject(*m_bmp_bubble_arrow_bottom);
				mdc.Blit((x - raBUBB_ARROW_WIDTH) / 2, y - raBUBB_ARROW_OVERLAP, x, y, &temp_dc1, 0, 0, wxCOPY, true);

				i = (wb - u) / 2;
				j = m_hand_rects[0].GetTop() - v - raGAME_ARROW_RELIEF;
				break;
			case 1:
				u = x + raBUBB_ARROW_PROTUN; 
				v = y;
				bmp_bubb_full = new wxBitmap(u, v);
				mdc.SelectObject(*bmp_bubb_full);
				mdc.SetBrush(wxBrush(*wxRED));
				mdc.SetPen(wxPen(*wxRED, 0));
				mdc.DrawRectangle(wxRect(0, 0, u, v));
				temp_dc1.SelectObject(bmp_bubb_rect);
				mdc.Blit(raBUBB_ARROW_PROTUN, 0, x, y, &temp_dc1, 0, 0, wxCOPY, true);
				temp_dc1.SelectObject(*m_bmp_bubble_arrow_left);
				mdc.Blit(0, (y - raBUBB_ARROW_WIDTH) / 2, x, y, &temp_dc1, 0, 0, wxCOPY, true);

				i = m_hand_rects[1].GetRight() + raGAME_ARROW_RELIEF;
				j = (hb - v) / 2;
				break;
			case 2:
				u = x;
				v = y + raBUBB_ARROW_PROTUN; 
				bmp_bubb_full = new wxBitmap(u, v);
				mdc.SelectObject(*bmp_bubb_full);
				mdc.SetBrush(wxBrush(*wxRED));
				mdc.SetPen(wxPen(*wxRED, 0));
				mdc.DrawRectangle(wxRect(0, 0, u, v));
				temp_dc1.SelectObject(bmp_bubb_rect);
				mdc.Blit(0, raBUBB_ARROW_PROTUN, x, y, &temp_dc1, 0, 0, wxCOPY, true);
				temp_dc1.SelectObject(*m_bmp_bubble_arrow_top);
				mdc.Blit((x - raBUBB_ARROW_WIDTH) / 2, 0, x, y, &temp_dc1, 0, 0, wxCOPY, true);

				i = (wb - u) / 2;
				j = m_hand_rects[2].GetBottom() + raGAME_ARROW_RELIEF;
				break;
			case 3:
				u = x + raBUBB_ARROW_PROTUN; 
				v = y;
				bmp_bubb_full = new wxBitmap(u, v);
				mdc.SelectObject(*bmp_bubb_full);
				mdc.SetBrush(wxBrush(*wxRED));
				mdc.SetPen(wxPen(*wxRED, 0));
				mdc.DrawRectangle(wxRect(0, 0, u, v));
				temp_dc1.SelectObject(bmp_bubb_rect);
				mdc.Blit(0, 0, x, y, &temp_dc1, 0, 0, wxCOPY, true);
				temp_dc1.SelectObject(*m_bmp_bubble_arrow_right);
				mdc.Blit(x - raBUBB_ARROW_OVERLAP, (y - raBUBB_ARROW_WIDTH) / 2, x, y, &temp_dc1, 0, 0, wxCOPY, true);

				i = m_hand_rects[3].GetLeft() - u - raGAME_ARROW_RELIEF;
				j = (hb - v) / 2;
				break;
			}
			mdc.SelectObject(wxNullBitmap);

			wxMask *mask_bubb_full = new wxMask(*bmp_bubb_full, *wxRED);
			bmp_bubb_full->SetMask(mask_bubb_full);

			mdc.SelectObject(*bmp_bubb_full);
			BlitToBack(i, j, u, v, &mdc, 0, 0, wxCOPY, true);
			if(bmp_bubb_full)
				delete bmp_bubb_full;

			//mdc.SelectObject(bmp_bubb_rect);
			//BlitToBack(0, 0, x, y, &mdc, 0, 0, wxCOPY, true);

		}
	}
#endif

	return true;
}

bool raGame::DrawHand(int loc, int x, int y, int orientation)
{
	int j;
	wxMemoryDC cfdc, cbdc;
	int player_type;
	wxASSERT((orientation == raHAND_HORIZONTAL) || 
		(orientation == raHAND_VERTICAL));

	player_type = m_players[loc].GetType(); 

#ifdef raGAME_HIDE_AI_HANDS
	if(player_type == raPLAYER_TYPE_AI)
		cbdc.SelectObject(*m_card_backs[m_pref_card_back]);
#endif

	if(orientation == raHAND_HORIZONTAL)
	{
		for(j = 0; j < m_hands[loc].count; j++)
		{
#ifdef raGAME_HIDE_AI_HANDS
			if(player_type == raPLAYER_TYPE_AI)
			{
				BlitToBack(x + (raCARD_HORZ_RELIEF * j), y, GG_CARD_WIDTH, GG_CARD_HEIGHT, &cbdc, 0, 0, wxCOPY, true);
			}
			else
			{
#endif
				cfdc.SelectObject(*m_card_faces[m_hands[loc].card_indexes[j]]);
				BlitToBack(x + (raCARD_HORZ_RELIEF * j), y, GG_CARD_WIDTH, GG_CARD_HEIGHT, &cfdc, 0, 0, wxCOPY, true);
#ifdef raGAME_HIDE_AI_HANDS
			}
#endif

			// Update the position and dimensions of cards and hands
			m_hand_card_rects[loc][j] = wxRect(x + (raCARD_HORZ_RELIEF * j), y, GG_CARD_WIDTH, GG_CARD_HEIGHT);

			// TODO : Correction for overlapping
			m_hand_rects[loc].Union(m_hand_card_rects[loc][j]);
		}
	}
	else
	{
		for(j = 0; j < m_hands[loc].count; j++)
		{
#ifdef raGAME_HIDE_AI_HANDS
			if(player_type == raPLAYER_TYPE_AI)
			{
				BlitToBack(x, y + (raCARD_VERT_RELIEF * j), GG_CARD_WIDTH, GG_CARD_HEIGHT, &cbdc, 0, 0, wxCOPY, true);
			}
			else
			{
#endif
				cfdc.SelectObject(*m_card_faces[m_hands[loc].card_indexes[j]]);
				BlitToBack(x, y + (raCARD_VERT_RELIEF * j), GG_CARD_WIDTH, GG_CARD_HEIGHT, &cfdc, 0, 0, wxCOPY, true);
#ifdef raGAME_HIDE_AI_HANDS
			}
#endif

			// Update the position and dimensions of cards and hands
			if(orientation == raHAND_HORIZONTAL)
				m_hand_card_rects[loc][j] = wxRect(x + (raCARD_HORZ_RELIEF * j), y, GG_CARD_WIDTH, GG_CARD_HEIGHT);
			else 
				m_hand_card_rects[loc][j] = wxRect(x, y + (raCARD_VERT_RELIEF * j), GG_CARD_WIDTH, GG_CARD_HEIGHT);

			// TODO : Correction for overlapping
			m_hand_rects[loc].Union(m_hand_card_rects[loc][j]);
		}
	}

	return true;
}

bool raGame::DrawTrick()
{
	int i;
	int loc;
	wxMemoryDC mdc;
	int rot_addn;

	//wxBitmap bmp_red_arrow_bottom(red_arrow_bottom_xpm);
	//wxBitmap bmp_red_arrow_top(red_arrow_top_xpm);
	//wxBitmap bmp_red_arrow_right(red_arrow_right_xpm);
	//wxBitmap bmp_red_arrow_left(red_arrow_left_xpm);

	if(m_clockwise)
		rot_addn = 1;
	else
		rot_addn = 3;

	for(i = 0; i < raTOTAL_PLAYERS; i++)
		m_trick_card_rects[i] = wxRect(0, 0, 0, 0);

	for(i = 0; i < m_trick.count; i++)
	{
		//loc = (m_trick.lead_loc + i) % raTOTAL_PLAYERS;
		loc = (m_trick.lead_loc + (i * rot_addn)) % raTOTAL_PLAYERS;
		mdc.SelectObject(*m_card_faces[m_trick.cards[loc]]);
		switch(loc)
		{
		case 0:
			// Draw at the bottom
			m_trick_card_rects[0] = wxRect(
				(this->GetClientSize().GetWidth() - GG_CARD_WIDTH) / 2, 
				((this->GetClientSize().GetHeight() - GG_CARD_HEIGHT) / 2) + (GG_CARD_HEIGHT / 4), 
				GG_CARD_WIDTH, GG_CARD_HEIGHT);
			BlitToBack(m_trick_card_rects[0].x, m_trick_card_rects[0].y,
				GG_CARD_WIDTH, GG_CARD_HEIGHT, &mdc, 0, 0, wxCOPY, true);
			break;
		case 1:
			m_trick_card_rects[1] = wxRect(
				((this->GetClientSize().GetWidth() - GG_CARD_WIDTH) / 2) - (GG_CARD_WIDTH / 4), 
				(this->GetClientSize().GetHeight() - GG_CARD_HEIGHT) / 2, 
				GG_CARD_WIDTH, GG_CARD_HEIGHT);
			BlitToBack(m_trick_card_rects[1].x, m_trick_card_rects[1].y,
				GG_CARD_WIDTH, GG_CARD_HEIGHT, &mdc, 0, 0, wxCOPY, true);
			break;
		case 2:
			m_trick_card_rects[2] = wxRect(
				(this->GetClientSize().GetWidth() - GG_CARD_WIDTH) / 2, 
				((this->GetClientSize().GetHeight() - GG_CARD_HEIGHT) / 2) - (GG_CARD_HEIGHT / 4), 
				GG_CARD_WIDTH, GG_CARD_HEIGHT);
			BlitToBack(m_trick_card_rects[2].x, m_trick_card_rects[2].y,
				GG_CARD_WIDTH, GG_CARD_HEIGHT, &mdc, 0, 0, wxCOPY, true);
			break;
		case 3:
			m_trick_card_rects[3] = wxRect(
				((this->GetClientSize().GetWidth() - GG_CARD_WIDTH) / 2) + (GG_CARD_WIDTH / 4), 
				(this->GetClientSize().GetHeight() - GG_CARD_HEIGHT) / 2, 
				GG_CARD_WIDTH, GG_CARD_HEIGHT);
			BlitToBack(m_trick_card_rects[3].x, m_trick_card_rects[3].y,
				GG_CARD_WIDTH, GG_CARD_HEIGHT, &mdc, 0, 0, wxCOPY, true);
			break;
		}
	}

	// If the trick has ended, graphically indicate the winner
	// using the red arrow
	if(m_trick.count == raTOTAL_PLAYERS)
	{
		int x, y;

		switch(m_trick.winner)
		{
		case 0:
			mdc.SelectObject(*m_bmp_red_arrow_bottom);
			x = (this->GetClientSize().GetWidth() - raARROW_WIDTH) / 2;
			y = m_trick_card_rects[0].GetBottom() + raGAME_ARROW_RELIEF;
			break;
		case 1:
			mdc.SelectObject(*m_bmp_red_arrow_left);
			x = m_trick_card_rects[1].GetLeft() - raARROW_WIDTH - raGAME_ARROW_RELIEF;
			y = (this->GetClientSize().GetHeight() - raARROW_WIDTH) / 2;
			break;
		case 2:
			mdc.SelectObject(*m_bmp_red_arrow_top);
			x = (this->GetClientSize().GetWidth() - raARROW_WIDTH) / 2;
			y = m_trick_card_rects[2].GetTop() - raARROW_WIDTH - raGAME_ARROW_RELIEF;
			break;
		case 3:
			mdc.SelectObject(*m_bmp_red_arrow_right);
			x = m_trick_card_rects[3].GetRight() + raGAME_ARROW_RELIEF;
			y = (this->GetClientSize().GetHeight() - raARROW_WIDTH) / 2;
			break;
		default:
			break;
		}
		BlitToBack(x, y, raARROW_WIDTH, raARROW_WIDTH,
			&mdc, 0, 0, wxCOPY, true);
	}

	return true;
}

bool raGame::DrawTrump()
{
	int trump_card;
	int max_bidder;
	int player_type;
	wxMemoryDC mdc;

	if(!m_engine.GetMaxBid(NULL, &max_bidder))
	{
		wxLogError(wxString::Format(wxT("GetMaxBid failed. %s:%d"), __FILE__, __LINE__));
		return false;
	}

	wxASSERT((max_bidder >= raPLAYER_INVALID) && (max_bidder < raTOTAL_PLAYERS));
	if(max_bidder == raPLAYER_INVALID)
		return true;

	player_type = m_players[max_bidder].GetType();
	wxASSERT((player_type >= raPLAYER_TYPE_INVALID) && (player_type <= raPLAYER_TYPE_AI));

	trump_card = m_engine.GetTrumpCard();
	wxASSERT((trump_card >= raCARD_INVALID) && (trump_card < raTOTAL_CARDS));

	// Draw the trump only if
	// 1. There is a valid trump set
	// 2. and if the trump is not shown yet.

	if((trump_card != raCARD_INVALID) && !m_engine.IsTrumpShown())
	{
#ifdef raGAME_HIDE_AI_HANDS
		if(player_type == raPLAYER_TYPE_AI)
			mdc.SelectObject(*m_card_backs[m_pref_card_back]);
		else
#endif
			mdc.SelectObject(*m_card_faces[trump_card]);

		if(!BlitToBack(raCARD_PANEL_RELIEF, raCARD_PANEL_RELIEF,
			GG_CARD_WIDTH, GG_CARD_HEIGHT, &mdc, 0, 0, wxCOPY, true))
		{
			wxLogError(wxString::Format(wxT("BlitToBack failed. %s:%d"), __FILE__, __LINE__));
			return false;
		}
	}

	return true;
}

void raGame::OnInfo(raInfoEvent& event)
{
	int ret_val;
	int dealer;
	switch(event.GetCommand())
	{
	case raINFO_CMD_NEW_DEAL:
		// Save the dealer information lest it gets reset
		// while resetting the rule engine
		dealer = m_engine.GetDealer();
		if(!ResetDeal())
		{
			wxLogError(wxString::Format(wxT("ResetDeal failed. %s:%d"), __FILE__, __LINE__));
		}
		m_engine.SetDealer(dealer);
		
		while(Continue());
		break;
	case raINFO_CMD_SHOW_TRUMP:
		if((ret_val = ShowTrump()) != 0)
		{
			wxMessageBox(wxT("Cannot ask for trump"));
		}
		while(Continue());
		break;
	default:
		wxLogError(wxString::Format(wxT("Unrecognized info event received. %s:%d"), __FILE__, __LINE__));
		break;
	}
}
void raGame::OnBid(raBidEvent& event)
{
	int ret_val;
	int bid;

	bid = event.GetBid();

	if((ret_val = MakeBid(bid)) != 0)
	{
		// In case of error, show appropriate message
		switch(ret_val)
		{
		case raERR_CANNOT_PASS:
			wxMessageBox(wxT("Cannot pass."));
			break;
		case raERR_BID_LESS_THAN_MIN:
			wxMessageBox(wxString::Format(wxT("Bid less than the minimum possible")));
			break;
		default:
			wxLogError(wxString::Format(wxT("Unexpected error value. %s:%d"), __FILE__, __LINE__));
			return;
			break;
		}
	}

	// Continue the game
	while(Continue());
}

void raGame::OnLeftDClick(wxMouseEvent &event)
{
	// If card play and trump selection are on double click
	if(m_play_card_on == raCONFIG_PREFS_PLAYCARDON_DCLICK)
	{
		if(!OnCardClick(event.GetPosition()))
		{
			wxLogError(wxString::Format(wxT("OnCardClick failed. %s:%d"), __FILE__, __LINE__));
			return;
		}
	}
	return;
}
void raGame::OnLeftUp(wxMouseEvent &event)
{
	wxPoint pt;
	int i;

	if(m_wait_trick)
	{
		pt = event.GetPosition();
		for(i = 0; i < raTOTAL_PLAYERS; i++)
		{
			if(m_trick_card_rects[i].Inside(pt))
			{
				raRuleEngine::ResetTrick(&m_trick);
				// Redraw back buffer and refrsh the screen 
				// to reflect acceptance of the trick
				if(!UpdateDrawAndRefresh(false))
				{
					wxLogError(wxString::Format(wxT("Call to UpdateDrawAndRefresh() failed. %s:%d"), __FILE__, __LINE__));
				}

				m_wait_trick = false;

				// Check if the deal has ended
				if(HasDealEnded())
				{
					if(!EndDeal())
					{
						wxLogError(wxString::Format(wxT("EndDeal failed. %s:%d"), __FILE__, __LINE__));
						return;
					}
					return;
				}

				while(Continue());
				return;
			}
		}

	}
	else
	{
		// If card play and trump selection are on double click
		if(m_play_card_on == raCONFIG_PREFS_PLAYCARDON_SCLICK)
		{
			if(!OnCardClick(event.GetPosition()))
			{
				wxLogError(wxString::Format(wxT("OnCardClick failed. %s:%d"), __FILE__, __LINE__));
				return;
			}
		}
	}
	return;
}

int raGame::GetCardAtPos(wxPoint pt, int loc)
{
	int i, j;

	if(loc == raPLAYER_INVALID)
	{

		// Check whether the position is inside any of the hands
		for(i = 0; i < raTOTAL_PLAYERS; i++)
		{
			// If so, find the card
			if(m_hand_rects[i].Inside(pt))
			{
				// Consider each of the cards in the hand.
				// This is done in reverse order is because the 
				// dimension/position of each of the cards is calculated 
				// without considering the possibility of overlapping
				for(j = m_hands[i].count - 1; j >= 0; j--)
				{
					if(m_hand_card_rects[i][j].Inside(pt))
					{
						//wxMessageBox(wxString::Format("Inside %d", m_hands[i].card_indexes[j]));
						return m_hands[i].card_indexes[j]; 
					}
				}
			}
		}
	}
	else
	{
		wxASSERT((loc >= 0) && (loc < raTOTAL_PLAYERS));
		for(i = m_hands[loc].count - 1; i >= 0; i--)
		{
			if(m_hand_card_rects[loc][i].Inside(pt))
			{
				return m_hands[loc].card_indexes[i]; 
			}
		}
	}
		
	return raCARD_INVALID;
}
int raGame::GetHandAtPos(wxPoint pt)
{
	int i;

	// Check whether the position is inside any of the hands
	for(i = 0; i < raTOTAL_PLAYERS; i++)
	{
		// If so, return the player/location
		if(m_hand_rects[i].Inside(pt))
		{
			return i;
		}
	}
		
	return raPLAYER_INVALID;

}

bool raGame::Continue()
{
	unsigned long hands[raTOTAL_PLAYERS];
	bool m_quit = false;
	raOutputDealInfo out_deal_info;
	raOutputDealEndInfo out_deal_end_info;
	raInputBidInfo in_bid_info;
	raInputTrumpselInfo in_trumpsel_info;
	raInputTrickInfo in_trick_info;
	raInfoDetails info_dtls;
	wxString instruction;
	raRuleEngineData re_data;
	//raRuleEngineData pplay_data;
	int bid;
	int trump;
	int card;
	//int old_deal_no;
	int i;
	//wxString msg_game;
	wxString msg;
	//int loc;
	//int old_pts[raTOTAL_TEAMS];
	//int game_pts;
	bool should_abandon;
	raRules rules;

	// The game cannot continue if we are waiting
	// for the user to confirm a trick
	if(m_wait_trick)
		return false;

	// The game cannot continue if deal has ended
	if(m_deal_ended)
		return false;

	while(!m_quit)
	{
		if(m_engine.IsOutputPending())
		{
			switch(m_engine.GetPendingOutputType())
			{
			case raOUTPUT_STARTED:
				m_engine.GetOutput(NULL, NULL);

				// Set the dealer in the information panel
				m_info->GetDetails(&info_dtls);
				info_dtls.dealer = m_engine.GetDealer();
				m_info->SetDetails(&info_dtls);

				// TODO : This instruction is not shown or is overwritten. Consider removal
				m_info->SetInstruction(wxT("Deal started."), raINFO_CMD_NONE);
				break;
			case raOUTPUT_DEAL:
				memset(&out_deal_info, 0, sizeof(raOutputDealInfo));
				m_engine.GetOutput(NULL, &out_deal_info);
				m_info->SetInstruction(wxT("Cards dealt."), raINFO_CMD_NONE);

				m_engine.GetHands(hands);
				wxLogDebug(raLib::PrintHands(hands));

				// Update hands, redraw back buffer and refresh the screen
				if(!UpdateDrawAndRefresh())
				{
					wxLogError(wxString::Format(wxT("UpdateDrawAndRefresh failed. %s:%d"), __FILE__, __LINE__));
					return false;
				}

				// Check whether any of the players have all the
				// four jacks. In which case, the game is abandoned
				for(i = 0; i < raTOTAL_PLAYERS; i++)
				{
					if((hands[i] & raGAME_FOUR_JACKS) == raGAME_FOUR_JACKS)
					{
						msg.Empty();
						msg.Append(raLib::m_long_locs[i]);
						msg.Append(wxT(" has all the four Jacks. This deal has been abandoned"));
						wxMessageBox(msg, wxT("Deal abandoned"), wxICON_INFORMATION);
						if(!EndDeal(true))
						{
							wxLogError(wxString::Format(wxT("EndDeal failed. %s:%d"), __FILE__, __LINE__));
							return false;
						}

					}
				}
				// Check whether any of the players have all low cards
				// after the second round of deal.
				// In which case, the game *can be* abandonded if decided by
				// the player.
				if(out_deal_info.round == raDEAL_ROUND_2)
				{
					for(i = 0; i < raTOTAL_PLAYERS; i++)
					{
						if(!(hands[i] & raGAME_ALL_HIGH_CARDS))
						{
							should_abandon = false;
							if(m_players[i].GetType() == raPLAYER_TYPE_AI)
							{
								m_players[i].AbandonGame(&should_abandon);

							}
							else
							{
								msg.Empty();
								msg.Append(wxT("All the cards dealt to you ("));
								msg.Append(raLib::m_long_locs[i]);
								msg.Append(wxT(") are low cards."));
								msg.Append(wxT("\n"));
								msg.Append(wxT("Do you want to abandon the deal?"));
								msg.Append(wxT("\n\n"));
								msg.Append(wxT("Click Yes to abandon the deal"));
								msg.Append(wxT("\n"));
								msg.Append(wxT("Click No to continue with the auction and deal"));
								if(wxMessageBox(msg, wxT("Abandon the deal?"), 
									wxYES_NO | wxICON_QUESTION) == wxYES)
								{	
									should_abandon = true;
								}
							}

							if(should_abandon)
							{
								if(m_players[i].GetType() == raPLAYER_TYPE_AI)
								{
									msg.Empty();
									msg.Append(wxT("All the cards dealt to "));
									msg.Append(raLib::m_long_locs[i]);
									msg.Append(wxT(" are low cards (without points)"));
									msg.Append(wxT("\n"));
									msg.Append(wxT("The player has decided to abandon the deal"));
									wxMessageBox(msg, wxT("Deal abandoned"), 
										wxOK | wxICON_INFORMATION);
								}
								if(!EndDeal(true))
								{
									wxLogError(wxString::Format(wxT("EndDeal failed. %s:%d"), 
										__FILE__, __LINE__));
								}
								return false;
							}
						}
					}
				}

				break;
			case raOUTPUT_DEAL_END:
				memset(&out_deal_end_info, 0, sizeof(raOutputDealEndInfo));
				m_engine.GetOutput(NULL, &out_deal_end_info);
				if(!EndDeal())
				{
					wxLogError(wxString::Format(wxT("EndDeal failed. %s:%d"), __FILE__, __LINE__));
					return false;
				}

				m_quit = true;
				break;
			}
		}
		else if(m_engine.IsInputPending())
		{
			switch(m_engine.GetPendingInputType())
			{
			case raINPUT_BID:
				if(!m_engine.GetPendingInputCriteria(NULL, &in_bid_info))
				{
					wxLogError(wxString::Format
						(wxT("Call to raRuleEngine::GetPendingInputCriteria failed. %s:%d"), 
						__FILE__, __LINE__));
					return false;
				}
				// For the first bid is being made and if the the player
				// does not have any cards with points, the player can
				// choose to abandon the deal
				m_engine.GetRules(&rules);
				if(in_bid_info.min == rules.min_bid_1)
				{
					m_engine.GetHands(hands);
					if(!(hands[in_bid_info.player] & raGAME_ALL_HIGH_CARDS))
					{
						should_abandon = false;
						//can_abandon = true;
						if(m_players[in_bid_info.player].GetType() == raPLAYER_TYPE_AI)
						{
							m_players[in_bid_info.player].AbandonGame(&should_abandon);

						}
						else
						{
							msg.Empty();
							msg.Append(wxT("All the cards dealt to you ("));
							msg.Append(raLib::m_long_locs[in_bid_info.player]);
							msg.Append(wxT(") are low cards."));
							msg.Append(wxT("\n"));
							msg.Append(wxT("Do you want to abandon the deal?"));
							msg.Append(wxT("\n\n"));
							msg.Append(wxT("Click Yes to abandon the deal"));
							msg.Append(wxT("\n"));
							msg.Append(wxT("Click No to continue and make a bid"));
							if(wxMessageBox(msg, wxT("Abandon the deal?"), 
								wxYES_NO | wxICON_QUESTION) == wxYES)
							{	
								should_abandon = true;
							}
						}

						if(should_abandon)
						{
							if(m_players[in_bid_info.player].GetType() == raPLAYER_TYPE_AI)
							{
								msg.Empty();
								msg.Append(wxT("All the cards dealt to "));
								msg.Append(raLib::m_long_locs[in_bid_info.player]);
								msg.Append(wxT(" are low cards (without points)"));
								msg.Append(wxT("\n"));
								msg.Append(wxT("The player has decided to abandon the deal"));
								wxMessageBox(msg, wxT("Deal abandoned"), 
									wxOK | wxICON_INFORMATION);
							}
							if(!EndDeal(true))
							{
								wxLogError(wxString::Format(wxT("EndDeal failed. %s:%d"), 
									__FILE__, __LINE__));
							}
							return false;
						}
					}
				}

				// TODO : Consider whether more elaboration is required
				// TODO : Add error check
				m_info->SetInstruction(wxString::Format
					(wxT("%s, please enter a bid."), raLib::m_long_locs[in_bid_info.player].c_str()));

				// Enable/disable relevant buttons in the bid panel
				if(!m_bid->SetPassable(in_bid_info.passable))
				{
					wxLogError(wxString::Format(wxT("Call to raBid::SetPassable failed. %s:%d"), 
						__FILE__, __LINE__));
					return false;
				}

				if(!m_bid->SetMinimumBid(in_bid_info.min))
				{
					wxLogError(wxString::Format(wxT("Call to raBid::SetMinimumBid failed. %s:%d"), 
						__FILE__, __LINE__));
					return false;
				}
				// Try out bidding
				// TODO : Cleanup this bloody mess!
				if(m_players[in_bid_info.player].GetType() == raPLAYER_TYPE_AI)
				{
					m_engine.GetData(&re_data);
					m_players[in_bid_info.player].SetRuleEngineData(&re_data);

					bid = raBID_INVALID;
					trump = raSUIT_INVALID;

					m_players[in_bid_info.player].GetBid(&bid, &trump, in_bid_info.min, !in_bid_info.passable);
					wxASSERT(bid != raBID_INVALID);

					//wxMessageBox(wxString::Format(
					//	"Bid suggested by AI for %d is %d %d", in_bid_info.player , bid, trump));

					//if(bid != raBID_PASS)
					//	in_bid_info.trump = trump;
					//in_bid_info.bid = bid;
					if(MakeBid(bid))
					{
						wxLogError(wxString::Format(wxT("MakeBid failed. %s:%d"), 
							__FILE__, __LINE__));
					}

					break;
				}


				// Show the bid panel
				//if(!m_bid->IsShown())
				m_bid->Show(true);
				m_quit = true;
				break;
			case raINPUT_TRUMPSEL:
				// If bid panel is shown, hid it
				//if(m_bid->IsShown())
				m_bid->Show(false);

				if(!m_engine.GetPendingInputCriteria(NULL, &in_trumpsel_info))
				{
					wxLogError(wxString::Format
						(wxT("Call to raRuleEngine::GetPendingInputCriteria failed. %s:%d"), 
						__FILE__, __LINE__));
					return false;
				}

				// If the player to select the trump is an AI based player
				// obtain a bid from the player and set the same
				if(m_players[in_trumpsel_info.player].GetType() == raPLAYER_TYPE_AI)
				{
					// Obtain trump from the AI based player
					trump = m_players[in_trumpsel_info.player].GetTrump();

					// Check for error
					if(trump == raCARD_INVALID)
					{
						wxLogError(wxString::Format
							(wxT("GetBid failed. %s:%d"), __FILE__, __LINE__));
					}
					wxASSERT((trump >= 0) && (trump < raTOTAL_CARDS));

					// Set the trump
					if(SetTrump(trump))
					{
						wxLogError(wxString::Format
							(wxT("SetTrump failed. %s:%d"), __FILE__, __LINE__));
					}

					// Check if the opponents have trumps after the final 
					// trump is set. If not, deal is abandoned
					if(CheckOppTrumps() == 1)
					{
						m_quit = true;
					}
					break;
				}

				// TODO : Consider whether more elaboration is required
				// TODO : Add error check
				m_info->SetInstruction(wxString::Format
					(wxT("%s, please select a trump."), raLib::m_long_locs[in_trumpsel_info.player].c_str()));

				m_quit = true;
				break;

			case raINPUT_TRICK:

				// If bid panel is shown, hide it
				//if(m_bid->IsShown())
				//{
				m_bid->Show(false);
				//	Refresh();
				//}

				if(!m_engine.GetPendingInputCriteria(NULL, &in_trick_info))
				{
					wxLogError(wxString::Format
						(wxT("Call to raRuleEngine::GetPendingInputCriteria failed. %s:%d"), 
						__FILE__, __LINE__));
					return false;
				}

				if(!UpdateDrawAndRefresh(false))
				{
					wxLogError(wxString::Format(wxT("UpdateDrawAndRefresh() failed. %s:%d"), 
						__FILE__, __LINE__));
				}

				// After applying the mask to the hand held by the player
				// if only one card can be played, play the same
				// without hesitation
				m_engine.GetHands(hands);
				if(
					(bhLib::CountBitsSet(in_trick_info.mask & hands[in_trick_info.player]) == 1) &&
					(m_auto_play || (m_players[in_trick_info.player].GetType() == raPLAYER_TYPE_AI))
					)
				{
					// Begin the busy state
					if(!::wxIsBusy())
						BeginBusyState();

					card = bhLib::HighestBitSet(in_trick_info.mask & hands[in_trick_info.player]);
					wxASSERT((card >= 0) && (card < raTOTAL_CARDS));
					if(PlayCard(card))
					{
						wxLogError(wxString::Format
							(wxT("PlayCard failed. %s:%d"), __FILE__, __LINE__));
						wxLogError(wxString::Format("Card played is %s%s", 
							raLib::m_suits[raGetSuit(card)].c_str(),
							raLib::m_values[raGetValue(card)].c_str()
							));
					}	

					// End the busy state
					if(::wxIsBusy())
						EndBusyState();

					// Check if the deal has ended, if so quit the loop
					if(m_deal_ended)
					{
						m_quit = true;
						break;
					}
					if(m_wait_trick)
						return false;
					break;

				}

				// If the player to play the next card is AI,
				if(m_players[in_trick_info.player].GetType() == raPLAYER_TYPE_AI)
				{
					m_info->SetInstruction(wxString::Format("%s is thinking...", 
						raLib::m_long_locs[in_trick_info.player].c_str()));

					// Begin the busy state
					if(!::wxIsBusy())
						BeginBusyState();

					m_engine.GetData(&re_data);

					//if(!HideInfo(&re_data, in_trick_info.player))
					//{
					//	wxLogError(wxString::Format
					//		(wxT("HideInfo failed. %s:%d"), __FILE__, __LINE__));
					//}

					m_players[in_trick_info.player].SetRuleEngineData(&re_data);

					card = m_players[in_trick_info.player].GetPlay();
					if(card == -2)
					{
						wxLogError(wxString::Format
							(wxT("GetPlay failed. %s:%d"), __FILE__, __LINE__));
					}
					if(card == -1)
					{
						if(ShowTrump())
						{
							wxLogError(wxString::Format
								(wxT("GetPlay failed. %s:%d"), __FILE__, __LINE__));
						}

						// End the busy state
						if(::wxIsBusy())
							EndBusyState();

						break;
					}
					wxASSERT((card >= 0) && (card < raTOTAL_CARDS));

					// End the busy state
					if(::wxIsBusy())
						EndBusyState();

					if(PlayCard(card))
					{
						wxLogError(wxString::Format
							(wxT("PlayCard failed. %s:%d"), __FILE__, __LINE__));
						wxLogError(wxString::Format("Card played is %s%s", 
							raLib::m_suits[raGetSuit(card)].c_str(),
							raLib::m_values[raGetValue(card)].c_str()
							));
					}
						
					if(m_wait_trick)
						return false;
					break;

				}
				// TODO : Consider whether more elaboration is required
				// TODO : Add error check
				if(m_play_card_on == raCONFIG_PREFS_PLAYCARDON_SCLICK)
					instruction.sprintf(wxT("%s, please click on the card that you want to play."), 
						raLib::m_long_locs[in_trick_info.player].c_str());
				else
					instruction.sprintf(wxT("%s, please double-click on the card that you want to play."), 
						raLib::m_long_locs[in_trick_info.player].c_str());

				if(in_trick_info.can_ask_trump)
				{
					instruction.Append(wxT(" Or ask for trump by clicking on the button below."));
					m_info->SetInstruction(instruction, raINFO_CMD_SHOW_TRUMP);
				}
				else
				{
					m_info->SetInstruction(instruction);
				}
				m_quit = true;
				break;
			}
		}
		// If the current deal has finished, no need to keep on continuing
		//else if(m_deal_ended)
		if(m_deal_ended)
		{
			m_quit = true;
			break;
		}
		while(m_engine.Continue());
	}
	return false;
}

bool raGame::ResetDeal()
{
	int i, j;

	if(!m_engine.Reset())
	{
		wxLogError(wxString::Format(wxT("Attempt to reset the rule engine failed. %s:%d"), __FILE__, __LINE__));
		return false;
	}

	m_wait_trick = false;
	m_deal_ended = false;

	// Initialize the hand information to zeros
	memset(m_hands, 0, sizeof(m_hands));

	// Initialize the trick
	raRuleEngine::ResetTrick(&m_trick);
	
	// Initialize the position/dimentions of hands
	// and the cards in the trick
	for(i = 0; i < raTOTAL_PLAYERS; i++)
	{
		m_hand_rects[i] = wxRect(0, 0, 0, 0);
		m_trick_card_rects[i] = wxRect(0, 0, 0, 0);
		//m_trick_cards[i] = raCARD_INVALID;
	}

	// Initialize card positions and dimensions
	for(i = 0; i < raTOTAL_PLAYERS; i++)
		for(j = 0; j < raMAX_CARDS_PER_HAND; j++)
			m_hand_card_rects[i][j] = wxRect(0, 0, 0, 0);

	return true;
}
bool raGame::ResetGame()
{
	int i;
	//m_orientation = raGAME_ORIENT_ALL_HORZ;
	m_orientation = raGAME_ORIENT_MIXED;

	ResetDeal();
	SetClockwise(true);
	m_engine.SetDealer(0);
	for(i = 0; i < raTOTAL_TEAMS; i++)
		m_game_pts[i] = 5;
	for(i = 0; i < raTOTAL_PLAYERS; i++)
		m_pnlties[i] = 0;
	return true;
}


bool raGame::UpdateHands(unsigned long *hands)
{
	int i, j;
	unsigned long cards_left;
	//int temp;

	for(i = 0; i < raTOTAL_PLAYERS; i++)
	{
		m_hands[i].cards = hands[i];
		//m_hands[i].count = bhLib::CountBitsSet(hands[i]);

		memset(m_hands[i].card_indexes, 0, sizeof(m_hands[i].card_indexes));

		cards_left = hands[i];
		j = 0;
		while(cards_left > 0)
		{
			//temp = bhLib::HighestBitSet(cards_left);
			//m_hands[i].card_info[j].suit = raGetSuit(temp);
			//m_hands[i].card_info[j].value = raGetValue(temp);
			m_hands[i].card_indexes[j] = bhLib::HighestBitSet(cards_left);
			//wxLogDebug(wxString::Format("Adding %s %s", 
			//	raLib::m_suits[raGetSuit(m_hands[i].card_indexes[j])].c_str(), 
			//	raLib::m_values[raGetValue(m_hands[i].card_indexes[j])].c_str()
			//	));
			cards_left &= ~(1 << m_hands[i].card_indexes[j]);
			j++;
		}
		m_hands[i].count = j;
		wxASSERT((m_hands[i].count >= 0) && (m_hands[i].count <= raMAX_CARDS_PER_HAND));

		// Since the composition of the hands could have changed
		// invalidating the current calcuations related to hand positions
		// and dimensions
		//m_hand_rects[i] = wxRect(0, 0, 0, 0);
	}
	return true;
}
// TODO : Implement the usage of loc
int raGame::PlayCard(int card, int loc)
{
	raInputTrickInfo trick_info;
	int trick_round;
	int ret_val;
	//raInfoDetails info_details;
	raRuleEngineData pplay_data;
	raRuleEngineData re_data;
	int i;

	// Validate the input card
	wxASSERT((card >= 0) && (card < raTOTAL_CARDS));

	if(!m_engine.GetPendingInputCriteria(NULL, &trick_info))
	{
		wxLogError(wxString::Format(wxT("Failed to get pending input criteria. %s:%d"), 
			__FILE__, __LINE__));
		return -1;
	}

	// Check whether the card is valid as per the mask
	if(!(trick_info.mask & (1 << card)))
	{
		wxASSERT(trick_info.rules);
		return raERR_TRICK_MASK_MISMATCH;
	}

	// Save the trick round
	trick_round = m_engine.GetTrickRound();

	m_engine.GetData(&re_data);

	// Post the input and check for error
	trick_info.card = card;
	if((ret_val = m_engine.PostInputMessage(raINPUT_TRICK, &trick_info)) != 0)
	{
		return ret_val;
	}

	// Update other players about the game play
	for(i = 0; i < raTOTAL_PLAYERS; i++)
	{
		// Update only if the player is AI
		if((m_players[i].GetType() == raPLAYER_TYPE_AI) && 
			(re_data.in_trick_info.player != i))
		{
			// Hide information before sending
			memcpy(&pplay_data, &re_data, 
				sizeof(raRuleEngineData));
			HideInfo(&pplay_data, i);

			// Inferences could be made from the mask or 
			//the rules and hence hide the same
			pplay_data.in_trick_info.mask = 0;
			pplay_data.in_trick_info.rules = 0;
			m_players[i].PostPlayUpdate(&pplay_data, card);
			memcpy(&pplay_data, &re_data, 
				sizeof(raRuleEngineData));
			m_players[i].CheckAssumptions(&pplay_data);
		}
	}

	// If the card played was accepted by the engine,

	// Update the cards in the trick for the round saved
	// so that the same is reflected when the back buffer
	// is redrawn
	m_engine.GetTrick(trick_round, &m_trick);
	//memcpy(m_trick_cards, trick.cards, sizeof(m_trick_cards));

	// Update hands, redraw back buffer and refresh the screen
	if(!UpdateDrawAndRefresh())
	{
		wxLogError(wxString::Format(wxT("UpdateDrawAndRefresh failed. %s:%d"), __FILE__, __LINE__));
		return -1;
	}

	// If the trick that ended, need to wait for the user to accept the trick
	if(m_trick.count == raTOTAL_PLAYERS)
	{
		// TODO : Remove the commented code
		//m_info->GetDetails(&info_details);
		//for(i = 0; i < raTOTAL_TEAMS; i++)
		//	info_details.points[i] = m_engine.GetPoints(i);
		//m_info->SetDetails(&info_details);

		m_info->SetInstruction(wxT("Click on the cards to continue."), raINFO_CMD_NONE);

		// If the deal has ended
//#ifndef raGAME_PLAY_TILL_END
//		if(HasDealEnded())
//		{
//			if(!EndDeal())
//			{
//				wxLogError(wxString::Format(wxT("EndDeal failed. %s:%d"), __FILE__, __LINE__));
//				return -1;
//			}
//		}
//		else
//#endif
			m_wait_trick = true;
	}
	
	return 0;
}

// TODO : Implement loc
int raGame::SetTrump(int card, int loc)
{
	raInputTrumpselInfo trumpsel_info;
	int ret_val;

	if(!m_engine.GetPendingInputCriteria(NULL, &trumpsel_info))
	{
		wxLogError(wxString::Format(wxT("Failed to get pending input criteria. %s:%d"), 
			__FILE__, __LINE__));
		return -1;
	}

	trumpsel_info.card = card;
	// Post the input and check for error
	if((ret_val = m_engine.PostInputMessage(raINPUT_TRUMPSEL, &trumpsel_info)) != 0)
	{
		wxLogError(wxString::Format(wxT("PostInputMessage failed. %s:%d"), 
			__FILE__, __LINE__));
		return ret_val;
	}

	// Update hands, redraw back buffer and refresh the screen
	if(!UpdateDrawAndRefresh())
	{
		wxLogError(wxString::Format(wxT("UpdateDrawAndRefresh failed. %s:%d"), __FILE__, __LINE__));
		return -1;
	}

	return 0;
}

int raGame::ShowTrump(int loc)
{
	int ret_val;
	raInputTrickInfo in_trick_info;
	raInfoDetails info_dtls;
	int player;
	wxString msg;
	
	m_engine.GetPendingInputCriteria(NULL, &in_trick_info);
	// Check whether trump can be asked raERR_TRICK_INVALID_TRUMP_REQ
	if(loc != raPLAYER_INVALID)
	{
		wxASSERT((loc >= 0) && (loc < raTOTAL_PLAYERS));
		if(in_trick_info.player != loc)
		{
			wxLogError(wxString::Format(wxT("Invalid request for trump to be shown. %s:%d"), __FILE__, __LINE__));
			return raERR_TRICK_INVALID_TRUMP_REQ;
		}
	}

	in_trick_info.ask_trump = true;
	if((ret_val = m_engine.PostInputMessage(raINPUT_TRICK, &in_trick_info)) != 0)
	{
		wxLogError(wxString::Format(wxT("PostInputMessage failed. %s:%d"), __FILE__, __LINE__));
		return ret_val;
	}

	player = in_trick_info.player;
	msg.Append(wxString::Format("%s asked for trump.", raLib::m_long_locs[player].c_str()));
	msg.Append(wxString::Format("\nTrump is %s(%s).", 
		raLib::m_suits[m_engine.GetTrump()].c_str(),
		raLib::m_values[raGetValue(m_engine.GetTrumpCard())].c_str() ));
	wxMessageBox(msg, wxT("Trump Shown"), wxOK | wxICON_INFORMATION);

	// Reflect the trump in the info planel
	m_info->GetDetails(&info_dtls);
	info_dtls.trump = m_engine.GetTrump();
	m_info->SetDetails(&info_dtls);

	// Update hands, redraw back buffer and refresh the screen
	if(!UpdateDrawAndRefresh())
	{
		wxLogError(wxString::Format(wxT("UpdateDrawAndRefresh failed. %s:%d"), __FILE__, __LINE__));
		return -1;
	}

	return 0;
}

int raGame::MakeBid(int bid, int loc)
{
	raInputBidInfo bid_info;
	int ret_val;
	raInfoDetails info_dtls;
	raBackDrawInfo back_draw_info;

	// Check if the engine is waiting for a bid input
	if((m_engine.IsInputPending()) && (m_engine.GetPendingInputType() == raINPUT_BID))
	{
		if(!m_engine.GetPendingInputCriteria(NULL, &bid_info))
		{
			wxLogError(wxString::Format(wxT("Failed to get pending input criteria. %s:%d"), __FILE__, __LINE__));
			return -1;
		}
		// Check if the bid is by the correct player
		if((loc != raPLAYER_INVALID) && (loc != bid_info.player))
		{
			wxLogDebug(wxString::Format(wxT("Bid by the wrong player. %s:%d"), 
				__FILE__, __LINE__));
			return raERR_BID_BY_WRONG_PLAYER;
		}

		// If the bid is a pass,
		if((bid == raBID_PASS) && (!bid_info.passable))
		{
			wxLogDebug(wxString::Format(wxT("Cannot pass. %s:%d"), 
				__FILE__, __LINE__));
			return raERR_CANNOT_PASS;
		}

		// If the bid is less than minimum
		if((bid != raBID_ALL) && (bid != raBID_PASS) && (bid < bid_info.min))
		{
			wxLogDebug(wxString::Format(wxT("Bid less than minimum allowed. %s:%d"), 
				__FILE__, __LINE__));
			return raERR_BID_LESS_THAN_MIN;
		}

		bid_info.bid = bid;

		// Post the input and check for error
		if((ret_val = m_engine.PostInputMessage(raINPUT_BID, &bid_info)) != 0)
		{
			wxLogDebug(wxString::Format(wxT("PostInputMessage failed. %s:%d"), 
				__FILE__, __LINE__));
			return ret_val;
		}

	}
	else
	{
		wxLogDebug(wxString::Format(wxT(
			"Cannot make a bid when one is not expected. %s:%d"), 
			__FILE__, __LINE__));
		return -1;
	}

	// Update Info panel
	if(bid != raBID_PASS)
	{
		m_info->GetDetails(&info_dtls);
		info_dtls.bid = bid;
		info_dtls.bidder = bid_info.player;
		m_info->SetDetails(&info_dtls);
	}

	//
	// Show the bid graphically in a bubble
	//

	back_draw_info.draw_bid = true;
	back_draw_info.bid = bid_info.bid;
	back_draw_info.bid_loc = bid_info.player;

#ifdef raGAME_SHOW_BID_BUBBLES
	// Hide the bid window
	m_bid->Show(false);

	// Redraw the screen with bid bubble
	if(!UpdateDrawAndRefresh(false, &back_draw_info))
	{
		wxLogError(wxString::Format(wxT("UpdateDrawAndRefresh failed. %s:%d"), __FILE__, __LINE__));
	}

	// Sleep for some time so that the user can 
	// read the information
	wxThread::Sleep(1000);

#endif
	// Redraw the screen without bid bubble
	if(!UpdateDrawAndRefresh())
	{
		wxLogError(wxString::Format(wxT("UpdateDrawAndRefresh failed. %s:%d"), __FILE__, __LINE__));
	}

	return 0;
}


bool raGame::UpdateDrawAndRefresh(bool update, raBackDrawInfo *info)
{
	unsigned long hands[raTOTAL_PLAYERS];

	if(update)
	{
		m_engine.GetHands(hands);
		wxLogDebug(wxString::Format("%s\n %s:%d", raLib::PrintHands(hands).c_str(), __FILE__, __LINE__));
		// Update hand/card positions and dimensions
		if(!UpdateHands(hands))
		{
			wxLogError(wxString::Format(wxT("Call to UpdateHands failed. %s:%d"), __FILE__, __LINE__));
			return false;
		}
	}

	// Redraw back buffer and refrsh the screen 
	// to reflect the card play
	if(!RedrawBack(info))
	{
		wxLogError(wxString::Format(wxT("RedrawBack failed. %s:%d"), __FILE__, __LINE__));
		return false;
	}
	if(!RefreshScreen())
	{
		wxLogError(wxString::Format(wxT("RefreshScreen failed. %s:%d"), __FILE__, __LINE__));
		return false;
	}
	this->Update();
	return true;
}

//
//bool raGame::UpdateTrick(raTrick *trick)
//{
//	int i;
//	for(i = 0; i < raTOTAL_PLAYERS; i++)
//		;//m_trick_cards[i] = trick->cards[i];
//
//	return true;
//}

bool raGame::HideInfo(raRuleEngineData *data, int player)
{
	int i;

	wxASSERT((player >= 0) && (player < raTOTAL_PLAYERS));

	// Hide information that is not known the player
	for(i = 0; i < raTOTAL_PLAYERS; i++)
	{
		if(i != player)
		{
			data->hands[i] = 0;
		}
	}

	if((data->curr_max_bidder != player) && !data->trump_shown)
	{
		data->trump_card = raCARD_INVALID;
		data->trump_suit = raSUIT_INVALID;
	}

	return true;
}
bool raGame::HasDealEnded(int *winner)
{
	int bid, loc;
	int pts[raTOTAL_TEAMS];

	if(!m_engine.GetMaxBid(&bid, &loc))
	{
		wxLogError(wxString::Format(wxT("GetMaxBid failed. %s:%d"), __FILE__, __LINE__));
		return false;
	}
	m_engine.GetPoints(pts);
	
	// Check if the max bidders team has won the deal
	if(pts[raGetTeam(loc)] >= bid)
	{
		if(winner)
			*winner = raGetTeam(loc);
		return true;
	}
	if(pts[raGetOpponent(raGetTeam(loc))] > (28 - bid))
	{
		if(winner)
			*winner = raGetOpponent(raGetTeam(loc));
		return true;
	}
	return false;

}
bool raGame::EndDeal(bool abandon)
{
	int bid, loc;
	raInfoDetails info_dtls;
	int game_pts;
	int winner;
	wxString msg_deal;
	wxString msg_pnlty;
	int i;
	int old_deal_no;

	// If the game is not being abandoned, 
	// show the detail of the deal that has ended.
	if(!abandon)
	{
		if(!HasDealEnded(&winner))
		{
			wxLogError(wxString::Format(wxT("HasDealEnded failed. %s:%d"), __FILE__, __LINE__));
			return false;
		}

		if(!m_engine.GetMaxBid(&bid, &loc))
		{
			wxLogError(wxString::Format(wxT("GetMaxBid failed. %s:%d"), __FILE__, __LINE__));
			return false;
		}
	}

	m_info->GetDetails(&info_dtls);

	if(!abandon)
	{
		// TODO : Remove hard coding
		// Calculate the game points to be awarded
		// and set it in the info details
		if(bid < 20)
			game_pts = 1;
		else if(bid < 23)
			game_pts = 2;
		else
			game_pts = 3;

		msg_pnlty = wxT("");
		// If the max bidder is the winner and he has at least one 
		// penalty, one will be deducted
		if(winner == raGetTeam(loc))
		{
			if(m_pnlties[loc] > 0)
			{
				m_pnlties[loc]--;
				msg_pnlty.Append(wxT("One penalty deducted from "));
				msg_pnlty.Append(raLib::m_short_locs[loc]);
				msg_pnlty.Append(wxT("."));
			}
		}
		else
			game_pts++;

		m_game_pts[winner] += game_pts;
		m_game_pts[raGetOpponent(winner)] -= game_pts;

		for(i = 0; i < raTOTAL_TEAMS; i++)
		{
			if(m_game_pts[i] < 0)
			{
				m_pnlties[i]++;
				// TODO : Use raGetPartner
				m_pnlties[i + 2]++;
				// TODO : Remove hard coding
				m_game_pts[0] = 5;
				m_game_pts[1] = 5;

				if(!msg_pnlty.IsEmpty())
					msg_pnlty.Append("\n");
				msg_pnlty.Append("Game points reset and penalties awarded to ");
				msg_pnlty.Append(raLib::m_short_teams[i]);
				msg_pnlty.Append(".");
				break;
			}
		}

		// Show a message box with details
		// as to who won the deal

		msg_deal = wxT("");
		msg_deal.Append(wxString::Format(wxT("Game won by %s."), 
			raLib::m_short_teams[winner].c_str()));
		msg_deal.Append(wxString::Format("\n\nHighest bid was %d by %s.", 
			bid, raLib::m_long_locs[loc].c_str()));
		msg_deal.Append(wxString::Format("\n%s won %d points.", 
			raLib::m_short_teams[0].c_str(), m_engine.GetPoints(0)));
		msg_deal.Append(wxString::Format("\n%s won %d points.", 
			raLib::m_short_teams[1].c_str(), m_engine.GetPoints(1)));

		msg_deal.Append(wxString::Format("\n\n%d game point(s) awarded to %s.", 
			game_pts, 
			raLib::m_short_teams[winner].c_str()));

		if(!msg_pnlty.IsEmpty())
		{
			msg_deal.Append("\n\n");
			msg_deal.Append(msg_pnlty);
		}

		::wxMessageBox(msg_deal, wxT("Deal completed"), wxOK | wxICON_INFORMATION);
	}

	// Reset all players
	for(i = 0; i < raTOTAL_PLAYERS; i++)
		m_players[i].Reset();

	m_info->SetInstruction(wxT(""),
		raINFO_CMD_NEW_DEAL);

	// Set the dealer for the next deal
	// TODO : +1 is hardcoded. 
	m_engine.SetDealer((m_engine.GetDealer() + 1) % raTOTAL_PLAYERS);

	// Set the detail in the info panel
	old_deal_no = info_dtls.deal_no;
	m_info->ResetDetails();
	m_info->GetDetails(&info_dtls);
	info_dtls.dealer = m_engine.GetDealer();
	info_dtls.deal_no = old_deal_no + 1;
	for(i = 0; i < raTOTAL_TEAMS; i++)
		info_dtls.points[i] = m_game_pts[i];
	for(i = 0; i < raTOTAL_PLAYERS; i++)
		info_dtls.pnlties[i] = m_pnlties[i];
	m_info->SetDetails(&info_dtls);

	m_deal_ended = true;
	return true;
}
bool raGame::BeginBusyState()
{
	::wxBeginBusyCursor();
	return true;
}
bool raGame::EndBusyState()
{
	::wxEndBusyCursor();
	return true;
}
int raGame::CheckOppTrumps()
{
	int loc;
	wxString msg;
	int i;
	int opp_team;
	unsigned long opp_hands;
	unsigned long hands[raTOTAL_PLAYERS];
	int trump_suit;

	if(m_engine.GetStatus() == raSTATUS_TRICKS)
	{
		if(!m_engine.GetMaxBid(NULL, &loc))
		{
			wxLogError(wxString::Format(wxT("GetMaxBid failed. %s:%d"), __FILE__, __LINE__));
			return -1;
		}
		wxASSERT((loc >= 0) && (loc < raTOTAL_PLAYERS));

		m_engine.GetHands(hands);

		// Combine the hands of opponents of the max bidder
		opp_hands = 0;
		i = (loc + 1) % raTOTAL_PLAYERS;
		opp_team = raGetTeam(i);
		opp_hands |= hands[i];
		i = raGetPartner(i);
		opp_hands |= hands[i];

		trump_suit = m_engine.GetTrump();

		// If the opponents of the max bidder does not
		// a single trump card among them, abandon deal 
		// and show appropriate message
		if(!(opp_hands & raLib::m_suit_mask[trump_suit]))
		{
			msg.Empty();
			msg.Append(wxT("The trump selected for the deal is "));
			msg.Append(raLib::m_suits[trump_suit]);
			msg.Append(wxT("\n"));
			msg.Append(wxT("Team "));
			msg.Append(raLib::m_short_teams[opp_team]);
			msg.Append(wxT(" does not have any card of this suit"));
			msg.Append(wxT("\n\n"));
			msg.Append(wxT("This deal has been abandoned"));
			wxMessageBox(msg, wxT("Deal abandoned"), wxOK | wxICON_INFORMATION);

			if(!EndDeal(true))
			{
				wxLogError(wxString::Format(wxT("EndDeal failed. %s:%d"), __FILE__, __LINE__));
				return -1;
			}

			return 1;
		}
	}

	return 0;
}

bool raGame::OnCardClick(wxPoint pt)
{
	raInputTrumpselInfo trumpsel_info;
	raInputTrickInfo trick_info;
	int ret_val;
	int card;
	int loc;
	int status;
	//unsigned long hands[raTOTAL_PLAYERS];
	//unsigned long opp_hands;
	//int i;
	//wxString msg;
	//int opp_team;

	//wxMessageBox(wxString::Format("%d, %d", event.GetPosition().x, event.GetPosition().y));

	// Check if the engine is waiting for input on trump selection
	if(m_engine.IsInputPending())
	{
		switch(m_engine.GetPendingInputType())
		{
		case raINPUT_TRUMPSEL:
			if(!m_engine.GetPendingInputCriteria(NULL, &trumpsel_info))
			{
				wxLogError(wxString::Format(wxT("Failed to get pending input criteria. %s:%d"), 
					__FILE__, __LINE__));
				return false;
			}

			// Save the status of the rule engine
			status = m_engine.GetStatus();

			// Obtain the hand inside which the click was received
			loc = GetHandAtPos(pt);

			// Check whether the double-click was received inside a hand
			if(loc == raPLAYER_INVALID)
			{
				wxLogDebug(wxString::Format(wxT("Double click received, but not inside a hand. %s:%d"), 
					__FILE__, __LINE__));
				return true;
			}

			// Check whether the double-click was received at the correct hand
			if(trumpsel_info.player != loc)
			{
				wxLogDebug(wxString::Format(wxT("Double click received, but at the wrong hand. %s:%d"), 
					__FILE__, __LINE__));
				return true;
			}

			// Obtain the card at the position where the double click was received
			card = GetCardAtPos(pt);
			if(card == raCARD_INVALID)
			{
				wxLogDebug(wxString::Format(wxT("Double click received, but not on a card. %s:%d"), 
					__FILE__, __LINE__));
				return true;
			}

			wxLogDebug(wxString::Format("Card index %d", card));

			// Post the input and check for error
			if((ret_val = SetTrump(card)) != 0)
			{
				// In case of error, show appropriate message
				// TODO : Add relevant cases
				//switch(ret_val)
				//{
				//default:
				wxLogError(wxString::Format(wxT("Unexpected error value. %s:%d"), __FILE__, __LINE__));
				return false;
				//	break;
				//}
			}

			// Once the final trump is selected check if the
			// opponents of the max bidder has atleast one trump
			// among themselves. If not, game is abandoned.
			if(CheckOppTrumps() == 1)
				return true;

			// Continue the game
			while(Continue());

			break;
		case raINPUT_TRICK:
			if(!m_engine.GetPendingInputCriteria(NULL, &trick_info))
			{
				wxLogError(wxString::Format(wxT("Failed to get pending input criteria. %s:%d"), 
					__FILE__, __LINE__));
				return false;
			}
			// Obtain the hand inside which the click was received
			loc = GetHandAtPos(pt);

			// Check whether the double-click was received inside a hand
			if(loc == raPLAYER_INVALID)
			{
				wxLogDebug(wxString::Format(wxT("Double click received, but not inside a hand. %s:%d"), 
					__FILE__, __LINE__));
				return true;
			}

			// Check whether the double-click was received at the correct hand
			if(trick_info.player != loc)
			{
				wxLogDebug(wxString::Format(wxT("Double click received, but at the wrong hand. %s:%d"), 
					__FILE__, __LINE__));
				return true;
			}

			// Obtain the card at the position where the double click was received
			card = GetCardAtPos(pt);
			if(card == raCARD_INVALID)
			{
				wxLogError(wxString::Format(wxT("Double click received inside a hand, but not on a card. %s:%d"), 
					__FILE__, __LINE__));
				return true;
			}

			wxLogDebug(wxString::Format("Card index %d found at the position of double click", card));

			// Post the input and check for error
			if((ret_val = PlayCard(card)) != 0)
			{
				// In case of error, show appropriate message
				// TODO : Add relevant cases
				switch(ret_val)
				{
				case raERR_TRICK_MASK_MISMATCH:

					wxASSERT(trick_info.rules);
					// TODO : More appropriate messages

					// If the card did not match the mask 
					// show an error message depending on the rule
					// that was used to create the mask
					if(trick_info.rules & raRULE_1)
					{
						wxMessageBox(wxT("Player who set the trump cannot lead trump"));
						return true;
					}
					else if(trick_info.rules & raRULE_2)
					{
						wxMessageBox(wxT("After trump was asked, trump should be played"));
						return true;
					}
					else if(trick_info.rules & raRULE_3)
					{
						wxMessageBox(wxT("Should follow lead suit"));
						return true;
					}
					else if(trick_info.rules & raRULE_3)
					{
						wxMessageBox(wxT("After the trump was shown, the same card should be played"));
						return true;
					}

					wxLogError(wxString::Format(wxT("None of the rules matched. %s:%d"), __FILE__, __LINE__));

					wxMessageBox(wxT("Card does not match mask"));
					return true;
					break;
				default:
					wxLogError(wxString::Format(wxT("Unexpected error value. %s:%d"), __FILE__, __LINE__));
					return false;
					break;
				}
			}

			// Continue the game
			while(Continue());
			break;
		default:
			wxLogDebug(wxString::Format(wxT("Unexpected type of pending input. %s:%d"), __FILE__, __LINE__));
			break;
		}
	}
	else
		wxLogDebug(wxString::Format(wxT("Bid event received when not expected. %s:%d"), __FILE__, __LINE__));

	return true;
}



