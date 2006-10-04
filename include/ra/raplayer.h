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

#ifndef _RAPLAYER_H_
#define _RAPLAYER_H_

#include "ra/racommon.h"
#include "ra/raaiagent.h"

enum {
	raPLAYER_TYPE_INVALID = -1,
	raPLAYER_TYPE_HUMAN,
	raPLAYER_TYPE_AI
};

class raPlayer
{
private:
	raAIAgent m_agent;
	int m_loc;
	int m_type;
	int m_trump;
public:
	raPlayer();
	~raPlayer();
	void SetLocation(int loc);
	int GetLocation();
	int GetType();
	void SetType(int type);
	void SetRuleEngineData(raRuleEngineData *data);
	bool GetBid(int *bid, int *trump, int min, bool force_bid);
	int GetTrump();
	int GetPlay();
	bool PostPlayUpdate(raRuleEngineData *data, int card);
	bool Reset();
};

#endif
