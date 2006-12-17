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

#ifndef _RAAIAGENT_H_
#define _RAAIAGENT_H_

#include "ra/racommon.h"
#include "ra/raruleengine.h"
#include "sl/slsolver.h"

//#define AI_LOG 1
//#define raAI_LOG_AT_LEAF 1
//#define raAI_LOG_ESTIMATE_TRICKS 1
//#define raAI_LOG_ESTIMATE_POINTS 1
//#define raAI_LOG_GETBID 1
//#define raAI_LOG_GETTRUMP 1
//#define raAI_LOG_GET_PLAY_OLD 1
//#define raAI_LOG_GET_PLAY 1
//#define raAI_LOG_GENERATEMOVES 1
//#define raAI_LOG_EVALUATE 1
//#define raAI_LOG_MAKEMOVE 1
//#define raAI_LOG_GENERATESLPROBLEM
//#define raAI_LOG_CHECKASSUMPTIONS

#define raBID_SAMPLE 100
#define raAI_PLAY_SAMPLES 30
#define raAI_MAX_MOVES 20
#define raAI_GENMV_NOTRUMP 1
#define raAI_GENMV_TRUMP 2
#define raAI_GENMV_ALL 3 

#define raAI_POS_INFTY +10000
#define raAI_NEG_INFTY -10000

typedef struct RA_AI_MOVE
{
	int card;
	bool ask_trump;
}raAIMove;

typedef struct RA_AI_EVAL
{
	int eval;
	int count;
	bool valid;
} raAIEval;

class raAIAgent
{
private:
	raRuleEngine m_engine;
	int m_loc;
	unsigned long m_trump_cards;
	unsigned long m_notrump_suspects;
	unsigned long m_nulls[raTOTAL_PLAYERS];
	unsigned long m_mb_null_susp;
	bool EstimateTricks(unsigned long *p_hands, int trump, int *eval);
	bool EstimatePoints(unsigned long *hands, int trump, int trick_count, int *eval);
	bool GenerateMoves(raRuleEngine *node, raAIMove *moves, int *count, int type = raAI_GENMV_ALL);
	int Evaluate(raRuleEngine *node, int alpha, int beta, int depth, bool *ret_val);
	int raAIAgent::EstimateHeuristic(raRuleEngine *state);
	bool MakeMove(raRuleEngine *node, raAIMove *move);
	bool MakeMoveAndEval(raRuleEngine *node, raAIMove *move, int depth, int *eval);
	const static int s_depths[];
public:
	raAIAgent();
	~raAIAgent();
	void SetLocation(int loc);
	int GetLocation();
	bool GetBid(unsigned long cards, int *bid, int *trump, int min, bool force_bid);
	bool GetBid(int *bid, int *trump, int min, bool force_bid);
	bool SetRuleEngineData(raRuleEngineData *data);
	int GetTrump();
	static int GetTrump(unsigned long hand, int suit);
	int GetPlay(unsigned long mask);
	bool GenerateSLProblem(raRuleEngineData *data, slProblem *problem, int trump = raSUIT_INVALID);
	//bool GenerateSLSolution(slProblem *problem, slSolution *solution);
	bool GenerateDeals(raRuleEngineData *data, unsigned long **deals, int count, int trump = raSUIT_INVALID);
	static wxString PrintMoves(raAIMove *moves, int move_count);
	bool PostPlayUpdate(raRuleEngineData *data, int card = raCARD_INVALID);
	bool CheckAssumptions(raRuleEngineData *data);
	bool Reset();
	bool SetClockwise(bool flag);
	bool GetClockwise();
	bool AbandonGame(bool *flag);

};

#endif
