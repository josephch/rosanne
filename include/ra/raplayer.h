// Rosanne : Twenty Eight (28) Card Game
// Copyright (C) 2006-2009 Vipin Cherian
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
#include "ai/aiagent.h"

enum {
	raPLAYER_TYPE_INVALID = -1,
	raPLAYER_TYPE_HUMAN,
	raPLAYER_TYPE_AI
};

class raPlayer
{
private:
	aiAgent m_agent;
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
	void SetRuleEngineData(gmEngineData *data);
	bool GetBid(int *bid, int *trump, int min, bool force_bid);
	int GetTrump();
	int GetPlay();
	bool PostPlayUpdate(gmEngineData *data, int card);
	bool CheckAssumptions(gmEngineData *data);
	bool Reset();
	void SetRules(pgmRules rules = NULL);
	bool SetClockwise(bool flag);
	bool GetClockwise();
	bool AbandonGame(bool *flag);
};

#endif
