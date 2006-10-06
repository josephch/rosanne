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


#define RA_APP_MAJOR_VER 1
#define RA_APP_MINOR_VER 0
#define RA_APP_REL_TYPE "a"
#define RA_APP_REL_TYPE_VER 2 
#define RA_APP_NAME "rosanne"

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

#endif

