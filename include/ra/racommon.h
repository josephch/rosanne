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

#ifndef _RACOMMON_H_
#define _RACOMMON_H_

#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "bh/bh.h"
#include "ra/ralib.h"

// Including images
//#include "main_icon_16.xpm"
//#include "new_game.xpm"
//#include "exit.xpm"
//#include "options.xpm"
//#include "help.xpm"

#define RA_APP_MAJOR_VER 1
#define RA_APP_MINOR_VER 0
#define RA_APP_REL_TYPE "a"
#define RA_APP_REL_TYPE_VER 2 
#define RA_APP_NAME "rosanne"
#define RA_APP_AUTHOR "Vipin Cherian"

#define RA_APP_FULL_VER (wxString::Format ("%d.%d%s%d", \
	RA_APP_MAJOR_VER, RA_APP_MINOR_VER, \
	RA_APP_REL_TYPE, RA_APP_REL_TYPE_VER))

#define RA_APP_FULL_NAME (wxString::Format("%s %s", \
	RA_APP_NAME, RA_APP_FULL_VER.c_str()))

#define ra_APP_URL "http://rosanne.sourceforge.net"

#define raTOTAL_CARDS 32
#define raTOTAL_PLAYERS 4
#define raTOTAL_BID_ROUNDS 3
#define raPLAYER_INVALID -1
#define raSUIT_INVALID -1
#define raCARD_INVALID -1
#define raPartner(X) ((X + 2) % raTOTAL_PLAYERS)
#define raBID_INVALID -2
#define raBID_PASS 0
#define raBID_ALL -1
#define raTOTAL_SUITS 4
#define raTOTAL_VALUES 8
#define raTOTAL_TRICKS 8
#define raTOTAL_TEAMS 2 

#define raGetSuit(X) (X / raTOTAL_VALUES)
#define raGetValue(X) (X % raTOTAL_VALUES)
#define raGetTeam(X) (X % raTOTAL_TEAMS)
#define raGetOpponent(X) ((X + 1) % raTOTAL_TEAMS)
#define raGetPartner(X) ((X + 2) % raTOTAL_PLAYERS)
#define raGetOpponentOne(X) ((X + 1) % raTOTAL_PLAYERS)
#define raGetOpponentTwo(X) ((X + 2) % raTOTAL_PLAYERS)

// Colours
#define raCLR_HEAD_DARK (wxColour(0, 92, 133))
#define raCLR_HEAD_LITE (wxColour(136, 219, 255))

#define raCLR_BLUE_DARK wxColour(129, 203, 255)
#define raCLR_BLUE_LITE wxColour(168, 219, 255)
#define raCLR_PURP_DARK wxColour(188, 174, 255)
#define raCLR_PURP_LITE wxColour(212, 204, 255)

#define raCLR_BUBB_FILL wxColour(173, 255, 135)

//#define raCLR_INSTR wxColour(144, 0, 255)
#define raCLR_INSTR wxColour(255, 0, 144)
//#define raCLR_INSTR (*wxRED)

#define raARROW_WIDTH 16
#define raBUBB_ARROW_WIDTH 12
//#define raBUBB_EDGE_WIDTH 5
//#define raBUBB_CORNER_WIDTH 5
#define raBUBB_UNIT_MIN 5
#define raBUBB_MIN_WIDTH 120
#define raBUBB_MIN_HEIGHT 40
#define raBUBB_ARROW_OVERLAP 3
#define raBUBB_ARROW_PROTUN (raBUBB_ARROW_WIDTH - raBUBB_ARROW_OVERLAP)

// For testing purposes
#define raREAD_SEED_FROM_FILE 0
//#define raREAD_DEALER_FROM_FILE 0
//#define raREAD_DEAL_FROM_FILE 0
#define raTEST_DATA_FILE "ra_test_data.ini"
#define raTEXT_SEED "rand/seed"
#define raTEXT_DEALER "deal/dealer"
#define raTEXT_DEAL_ROUND "deal_round"

enum
{
	raSBARPOS_GEN = 0,
	raSBARPOS_CLOCK
};

#define raStatusBar ((wxFrame *)(wxTheApp->GetTopWindow()))->GetStatusBar()

#define raTEXT_CLOCKWISE wxT("Clockwise")
#define raTEXT_ANTICLOCKWISE wxT("Anti-lockwise")

#endif

