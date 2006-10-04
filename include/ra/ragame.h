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

#ifndef _RAGAME_H
#define _RAGAME_H

#include "ra/racommon.h"
#include "ra/raruleengine.h"
#include "ra/raaiagent.h"
#include "ra/rainfo.h"
#include "ra/raevents.h"
#include "ra/rabid.h"
#include "ra/raplayer.h"
#include "gg/ggpanel.h"
#include "gg/ggcard.h"

//#define raGAME_PLAY_TILL_END 0

#define raTOTAL_CARD_BACKS 2
#define raMAX_CARDS_PER_HAND 8
#define raCARD_VERT_RELIEF (10)
#define raCARD_HORZ_RELIEF (GG_CARD_WIDTH / 4)
#define raCARD_PANEL_RELIEF 20
#define raGAME_HIDE_AI_HANDS 1

enum{
	raHAND_VERTICAL = 0,
	raHAND_HORIZONTAL
};

// TODO : If raHandCard is no longer used, remove
typedef struct tagRA_HAND_CARD
{
	//int suit;
	//int value;
	int index;
	int x;
	int y;
	int width;
	int height;
}raHandCard, *praHandCard;

typedef struct tagRA_HAND
{
	unsigned long cards;
	int count;
	//raHandCard card_info[raMAX_CARDS_PER_HAND];
	int card_indexes[raMAX_CARDS_PER_HAND];
}raHand, *praHand;

class raGame: public ggPanel
{
private:
	wxBitmap *m_tile;
	DECLARE_EVENT_TABLE()

	wxBitmap *m_card_faces[raTOTAL_CARDS];
	wxBitmap *m_card_backs[raTOTAL_CARD_BACKS];
	raRuleEngine m_engine; 
	raInfo *m_info;
	raBid *m_bid;

	// Related to the display of hands
	raHand m_hands[raTOTAL_PLAYERS];
	wxRect m_hand_rects[raTOTAL_PLAYERS];
	wxRect m_hand_card_rects[raTOTAL_PLAYERS][raMAX_CARDS_PER_HAND];
	int m_hand_rot;

	// Related to the display of cards played in a trick
	//int m_trick_cards[raTOTAL_PLAYERS];
	raTrick m_trick;
	wxRect m_trick_card_rects[raTOTAL_PLAYERS];
	bool m_wait_trick;

	// To check whether the deal is still being played
	bool m_deal_ended;

	// Players
	raPlayer m_players[raTOTAL_PLAYERS];

	// Game points and penalties
	int m_game_pts[raTOTAL_TEAMS];
	int m_pnlties[raTOTAL_PLAYERS];

	void OnSize(wxSizeEvent& event);
	bool RedrawBack();
	bool DrawHand(int loc, int x, int y, int orientation = raHAND_HORIZONTAL);
	bool DrawTrick();
	void OnInfo(raInfoEvent& event);
	void OnBid(raBidEvent& event);
	void OnLeftDClick(wxMouseEvent &event);
	void OnLeftUp(wxMouseEvent &event);
	int GetCardAtPos(wxPoint pt, int loc = raPLAYER_INVALID);
	int GetHandAtPos(wxPoint pt);
	bool Continue();
	bool ResetDeal();
	bool ResetGame();
	bool UpdateHands(unsigned long *hands);
	int PlayCard(int card, int loc = raPLAYER_INVALID);
	int SetTrump(int card, int loc = raPLAYER_INVALID);
	int ShowTrump(int loc = raPLAYER_INVALID);
	int MakeBid(int bid, int loc = raPLAYER_INVALID);
	bool UpdateDrawAndRefresh();
	//bool UpdateTrick(raTrick *trick);
	bool HideInfo(raRuleEngineData *data, int player);
	bool HasDealEnded(int *winner = NULL);
	bool EndDeal();
public:
	raGame(const wxWindow* parent);
	virtual ~raGame();
	bool SetTile(wxBitmap *tile);
	bool SetInfoPanel(raInfo *info_panel);
	bool NewGame(int dealer = raPLAYER_INVALID);
};
#endif
