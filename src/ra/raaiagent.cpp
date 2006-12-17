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

#include "ra/raaiagent.h"

//const int raAIAgent::s_depths[] = {2, 3, 5, 7, 8, 8, 8, 8};
const int raAIAgent::s_depths[] = {2, 3, 4, 6, 7, 8, 8, 8};

raAIAgent::raAIAgent()
{
	// TODO : Remove hardcoding
	m_loc = 0;
	Reset();
}
raAIAgent::~raAIAgent()
{
}
//
// Public method/s
//
void raAIAgent::SetLocation(int loc)
{
	m_loc = loc;
}
int raAIAgent::GetLocation()
{
	return m_loc;
}

bool raAIAgent::GetBid(unsigned long cards, int *bid, int *trump, int min, bool force_bid)
{
	int i, k;
	int initial;
	int *undealt;
	int eval[2];
	int trump_count;		// Counter, used to loop through different trump options
	int sample;				// Counter, used to loop though different samples
	int data[4][29];
	unsigned long hands[4];
	int temp_trump;

	//raAIGameState state;

	//data = new int[4][29];
	/*for(i = 0; i < 4; i++)
	{
	data[i] = new int[raBID_SAMPLE];
	memset(data[i], 0, raBID_SAMPLE * sizeof(int));
	}*/
	memset(data, 0, 4 * 29 * sizeof(int));

#ifdef raAI_LOG_GETBID
	wxLogDebug(raLib::PrintLong(cards));
#endif

	initial = (int)bhLib::CountBitsSet(cards);
	wxASSERT(initial <= 8);
#ifdef raAI_LOG_GETBID
	wxLogDebug(wxString::Format("initial is %d", initial));
#endif
	undealt = new int[32 - initial];

	//
	//Dealing the rest of the cards
	//

	for(sample = 0; sample < raBID_SAMPLE; sample++)
	{
		// Get the rest of the cards into undealt
		k = 0;
		for(i = 0; i < 32; i++)
			if(!(cards & (1 << i)))
				undealt[k++] = i;

		//Shuffle the undealt
		raLib::ShuffleArray(undealt, 32 - initial);

		//Initialize the hands
		memset(hands, 0, sizeof(hands));

		// Simplifying the problem, assume the current player is South
		// Add the cards already dealt to South
		hands[0] = cards;


		// Deal the undealt cards
		k = 0;
		for(i = /*8 - */initial; i < 32; i++)
			hands[i / 8] |= 1 << undealt[k++];

#ifdef raAI_LOG_GETBID
		wxLogDebug("#############################");
		wxLogDebug(raLib::PrintHands(hands));
#endif


		// Calculate the estimated points
		// considering each suit as trump

		for(trump_count = 0; trump_count < 4; trump_count++)
		{
			// Estimation done only if there is atleast one card
			// belonging to the suit, in the initial hand
			if(cards & raLib::m_suit_mask[trump_count])
			{
				temp_trump = trump_count;
				eval[0] = 0;
				eval[1] = 0;
				EstimatePoints(hands, temp_trump, 0, eval);

				// Distribute half of the unallocated points equally
				// This is pure guess work
				eval[0] += (28 - eval[0] - eval[1]) / 4;

#ifdef raAI_LOG_GETBID
				if((eval[0] > 28) || eval[0] < 0)
				{
					wxLogDebug(wxString::Format("RRRooor %d %d", eval[0], eval[1]));
					wxLogDebug(wxString::Format("%08X %08X %08X %08X", hands[0], hands[1], hands[2], hands[3]));
					wxLogDebug(raLib::m_suits[trump_count].c_str());
				}
				wxLogDebug(wxString::Format("%s : %d - %d", raLib::m_suits[trump_count].c_str(), eval[0], eval[1]));
#endif
				data[trump_count][eval[0]]++;
			}
		}
#ifdef raAI_LOG_GETBID
		//wxLogDebug("#############################");
#endif
	}
#ifdef raAI_LOG_GETBID
	wxLogDebug("#############################");
	for(i = 0; i < 4; i++)
	{
		wxLogDebug(_("Trump - ") + raLib::m_suits[i]);
		for(k = 0; k < 29; k++)
		{
			wxLogDebug(wxString::Format("Bid - %d : %d", k, data[i][k]));
		}
	}
	wxLogDebug("#############################");
#endif


	// Analyzing cumulative success percentages and
	// figuring out which is the best bid
	*bid = 0;
	*trump = -1;
	for(i = 0; i < 4; i++)
	{
		sample = 0;
		for(k = 28; k >= 0; k--)
		{
			sample += data[i][k];
			// TODO : 70 might be an aggressive value fix it accordingly
			if((((double)(sample) / raBID_SAMPLE) >= 0.70) && (k > *bid))
			{
				*bid = k;
				*trump = i;
			}
		}
	}

#ifdef raAI_LOG_GETBID
	wxLogDebug(wxString::Format("Optimal bid is %d %d", *bid, *trump));
#endif

	// Cleanup data and undealt
	/*for(i = 0; i < 4; i++)
	delete data[i];
	delete data;*/

	delete undealt;

	// If the suggested bid is less than
	// the minimum bid suggested, return pass
	// or minimum bid appropriately
	if(*bid < min)
	{
		if(force_bid)
			*bid = min;
		else
		{
			*bid = 0;
			*trump = -1;
		}
	}

	return true;
}
bool raAIAgent::GetBid(int *bid, int *trump, int min, bool force_bid)
{
	int ret_trump;
	int ret_bid;
	unsigned long hands[4];
	unsigned long cards;
	int max_bidder;
	int trump_card;

	wxASSERT(bid);
	wxASSERT(trump);
	// TODO : Correct this check
	// Check if the current Rule Engine state is auction
	if(m_engine.GetStatus() == raSTATUS_NOT_STARTED)
	{
		return false;
	}

	m_engine.GetHands(hands);
#ifdef raAI_LOG_GETBID
	wxLogDebug(wxString::Format("Estimated bid for %d", m_loc));
	wxLogDebug(raLib::PrintLong(hands[m_loc]));
#endif

	cards = hands[m_loc];
	wxASSERT(cards);

	max_bidder = raPLAYER_INVALID;
	m_engine.GetMaxBid(NULL, &max_bidder);
	wxASSERT((max_bidder >= raPLAYER_INVALID) && (max_bidder < raTOTAL_PLAYERS));

	if(max_bidder == m_loc)
	{
		trump_card = m_engine.GetTrumpCard();
		wxASSERT((trump_card >= 0) && (trump_card < raTOTAL_CARDS));
		cards |= (1 << trump_card);
	}

	wxASSERT(bhLib::CountBitsSet(cards) <= 8);

	if(!GetBid(cards, &ret_bid, &ret_trump, min, force_bid))
	{
		wxLogDebug(wxString::Format("GetBid failed. File - %s Line - %d", __FILE__, __LINE__));
		return false;
	}
	wxASSERT((ret_bid == raBID_ALL) || (ret_bid == raBID_PASS) || (ret_bid >= min));
	 *bid = ret_bid;
	 *trump = ret_trump;

	return true;
}
bool raAIAgent::SetRuleEngineData(raRuleEngineData *data)
{
	m_engine.SetData(data, false);
	return true;
}

int raAIAgent::GetTrump()
{
	int bid;
	int trump;
	unsigned long hands[raTOTAL_PLAYERS];
	int ret_val = raCARD_INVALID;

	m_engine.GetHands(hands);
	//wxLogDebug(wxString::Format("Estimated bid for %d", m_loc));
	//wxLogDebug(raLib::PrintLong(hands[m_loc]));

	GetBid(&bid, &trump, 14, true);

	return GetTrump(hands[m_loc], trump);
}

// Function returns the card to be played(index)
// -1, for show trump
// -2 or other negative values in case of error

int raAIAgent::GetTrump(unsigned long hand, int suit)
{
	int i;
	unsigned long trump_cards;
	int ret_val = raCARD_INVALID;

	wxASSERT(hand);
	wxASSERT((suit > raSUIT_INVALID) && (suit <= raTOTAL_SUITS));

	trump_cards = (hand & raLib::m_suit_mask[suit]) >> raLib::m_suit_rs[suit];

	// If there are two cards with points
	// select smallest as the trump
	if((bhLib::CountBitsSet(trump_cards & 0x0000000F0)) >= 2)
	{
		for(i = 4; i < 8; i++)
			if(trump_cards & (1 << i))
			{
				ret_val = (suit * 8) + i;
				break;
			}
	}
	// Else, if you have at least on card
	// with no points select the highest as the trump
	else if(bhLib::CountBitsSet(trump_cards & 0x0000000F) > 0)
	{
		ret_val = (suit * 8) + bhLib::HighestBitSet(trump_cards & 0x0000000F);
	}
	// Else, select the highest card as trump
	else
	{
		ret_val = (suit * 8) + bhLib::HighestBitSet(trump_cards);
	}

	// Make sure that the trump card selected exists in the hand
	wxASSERT(hand & (1 << ret_val));
	return ret_val;
}

int raAIAgent::GetPlay(unsigned long mask)
{
	raRuleEngineData data, work_data, bkp_data;
	raRuleEngine rule_engine;
	//slProblem problem;
	//slSolution solution;
	unsigned long **deal_hands = NULL;
	int i, j, k;
	raAIEval evals[raTOTAL_VALUES];
	//raAIEval evals_trump[raTOTAL_SUITS][raTOTAL_VALUES];
	raAIEval eval_trump, avg_eval_trump;

	raAIMove moves[raTOTAL_VALUES];
	raAIMove moves_trump[raTOTAL_VALUES];
	int move_count, move_trump_count;
	int depth = -1;
	int eval;

	double best_eval;
	double best_eval_trump;
	double temp_eval;
	int best_play;
	//int best_play_trump;

#ifdef raAI_LOG_GET_PLAY
	wxLogDebug("**********Entering GetPlay()*****************");
	wxLogDebug(wxString::Format("Location - %s", raLib::m_long_locs[m_loc].c_str()));
#endif

	if(!m_engine.GetData(&data))
	{
		wxLogError(wxString::Format(wxT("GetData() failed. %s:%d"), 
			__FILE__, __LINE__));
		return -2;
	}
#ifdef raAI_LOG_GET_PLAY
	wxLogDebug("Rule engine data :");
	wxLogDebug(m_engine.GetLoggable());
	wxLogDebug("Trump candidates :");
	for(i = 0; i < raTOTAL_SUITS; i++)
	{
		if(m_trump_cards & (1 << i))
			wxLogDebug(raLib::m_suits[i].c_str());
	}
#endif

	wxASSERT((m_trump_cards >= 0) && (m_trump_cards <= 15));
	//if((data.trump_shown) || (bhLib::CountBitsSet(m_trump_cards) == 1))
	//	trump_known = true;

	// Create the array to hold the random deals
	deal_hands = new unsigned long *[30];
	if(!deal_hands)
	{
		wxLogError(wxString::Format(wxT("Memory allocation failed. %s:%d"), 
			__FILE__, __LINE__));
		return -2;
	}
	memset(deal_hands, 0, sizeof(deal_hands));
	for(i = 0; i < 30; i++)
	{
		deal_hands[i] = new unsigned long[raTOTAL_PLAYERS];
		if(!deal_hands[i])
		{
			wxLogError(wxString::Format(wxT("Memory allocation failed. %s:%d"), 
				__FILE__, __LINE__));
			return -2;
		}
		memset(deal_hands[i], 0, sizeof(unsigned long));
	}

	//
	// Generate deals and moves, play each and find the best move
	//

	memset(&evals, 0, sizeof(evals));
	//memset(&evals_trump, 0, sizeof(evals_trump));
	memcpy(&work_data, &data, sizeof(raRuleEngineData));
	memcpy(&bkp_data, &work_data, sizeof(raRuleEngineData));

	rule_engine.SetData(&work_data, false);

	depth = s_depths[m_engine.GetTrickRound()];
#ifdef raAI_LOG_GET_PLAY
	wxLogDebug(wxString::Format("Depth of search - %d", depth));
#endif
	
	avg_eval_trump.count = 0;
	avg_eval_trump.eval = 0;
	avg_eval_trump.valid = false;


	// If trump is shown or if self is the max bidder
	// then trump is known. 
	if(data.trump_shown || (m_loc == data.curr_max_bidder))
	{
#ifdef raAI_LOG_GET_PLAY
		if(data.trump_shown)
			wxLogDebug("Trump known as trump is shown");
		else
			wxLogDebug("Trump known as m_loc is the max bidder");
#endif
		// Generate possible moves which do not ask for trump
		if(!GenerateMoves(&rule_engine, moves, &move_count, raAI_GENMV_NOTRUMP))
		{
			wxLogError(wxString::Format(wxT("GenerateMoves() failed. %s:%d"), 
				__FILE__, __LINE__));
			return -2;
		}
		wxASSERT(move_count > 0);

#ifdef raAI_LOG_GET_PLAY
		wxLogDebug("Moves generated (without asking for trump) :");
		wxLogDebug(PrintMoves(moves, move_count));
#endif

		// Generate moves which ask for trump
		if(!GenerateMoves(&rule_engine, moves_trump, &move_trump_count, raAI_GENMV_TRUMP))
		{
			wxLogError(wxString::Format(wxT("GenerateMoves() failed. %s:%d"), 
				__FILE__, __LINE__));
			return -2;
		}
		wxASSERT(move_trump_count >= 0);

#ifdef raAI_LOG_GET_PLAY
		wxLogDebug("Moves generated (asking for trump) :");
		wxLogDebug(PrintMoves(moves_trump, move_trump_count));
#endif

		// Generate random deals
		if(!GenerateDeals(&data, deal_hands, 30))
		{
			wxLogError(wxString::Format(wxT("GenerateDeals() failed. %s:%d"), 
				__FILE__, __LINE__));
			return -2;
		}
		// For each random deal
		for(i = 0; i < 30; i++)
		{
			memcpy(work_data.hands, deal_hands[i], sizeof(work_data.hands));
#ifdef raAI_LOG_GET_PLAY
			wxLogDebug(wxString::Format("Random deal no : %d", i));
#endif
			for(j = 0; j < move_count; j++)
			{
				rule_engine.SetData(&work_data, false);

#ifdef raAI_LOG_GET_PLAY
				wxLogDebug("----------------------------------------------");
				wxLogDebug(wxString::Format("Random deal no : %d", i));
				wxLogDebug(wxString::Format("Attempting move no : %d", j));
				wxLogDebug(PrintMoves(&moves[j], 1));
				wxLogDebug("Rule engine data dump :");
				wxLogDebug(rule_engine.GetLoggable());
#endif
				if(!MakeMoveAndEval(&rule_engine, &moves[j], depth, &eval))
				{
					wxLogError(wxString::Format(wxT("MakeMoveAndEval() failed. %s:%d"), 
						__FILE__, __LINE__));
					return -2;
				}
				evals[j].eval += eval;
				evals[j].count++;
				evals[j].valid = true;
#ifdef raAI_LOG_GET_PLAY
				wxLogDebug(wxString::Format("Eval - %d", eval));
				wxLogDebug(wxString::Format("evals[%d].eval = %d", j, evals[j].eval));
				wxLogDebug(wxString::Format("evals[%d].count = %d", j, evals[j].count));
#endif
			}

			eval_trump.count = 0;
			eval_trump.eval = raAI_NEG_INFTY;
			eval_trump.valid = false;

			for(j = 0; j < move_trump_count; j++)
			{
				rule_engine.SetData(&work_data, false);

#ifdef raAI_LOG_GET_PLAY
				wxLogDebug("----------------------------------------------");
				wxLogDebug(wxString::Format("Random deal no : %d", i));
				wxLogDebug(wxString::Format("Attempting move no : %d", j));
				wxLogDebug(PrintMoves(&moves_trump[j], 1));
				wxLogDebug("Rule engine data dump :");
				wxLogDebug(rule_engine.GetLoggable());
#endif
				if(!MakeMoveAndEval(&rule_engine, &moves_trump[j], depth, &eval))
				{
					wxLogError(wxString::Format(wxT("MakeMoveAndEval() failed. %s:%d"), 
						__FILE__, __LINE__));
					return -2;
				}
				//evals_trump[0][j].eval += eval;
				//evals_trump[0][j].count++;
				//evals_trump[0][j].valid = true;
				if(eval > eval_trump.eval)
				{
					eval_trump.eval = eval;
					eval_trump.valid = true;
				}
#ifdef raAI_LOG_GET_PLAY
				wxLogDebug(wxString::Format("Eval - %d", eval));
				wxLogDebug(wxString::Format("eval_trump.eval = %d", eval_trump.eval));
#endif
			}

			if(j > 0)
			{
				wxASSERT(eval_trump.valid);
				avg_eval_trump.count++;
				avg_eval_trump.eval += eval_trump.eval;
#ifdef raAI_LOG_GET_PLAY
				wxLogDebug(wxString::Format("avg_eval_trump.count - %d",avg_eval_trump.count));
				wxLogDebug(wxString::Format("avg_eval_trump.eval - %d",avg_eval_trump.eval));
#endif
			}
		}
		best_eval = (double)raAI_NEG_INFTY;
		best_eval_trump = (double)raAI_NEG_INFTY;
		best_play = raCARD_INVALID;
		//best_play_trump = raCARD_INVALID;

#ifdef raAI_LOG_GET_PLAY
		wxLogDebug("Average evals for each move :");
#endif

		// Find out the best play without asking for the trump
		for(j = 0; j < move_count; j++)
		{
			if(evals[j].valid)
			{
				temp_eval = (double)evals[j].eval / (double)evals[j].count;

#ifdef raAI_LOG_GET_PLAY
				wxLogDebug(wxString::Format("%s - %5.2f", PrintMoves(&moves[j], 1).c_str(), temp_eval));
#endif

				if(temp_eval > best_eval)
				{
					best_eval = temp_eval;
					best_play = moves[j].card;
				}
			}
		}
		wxASSERT(best_play != raCARD_INVALID);

#ifdef raAI_LOG_GET_PLAY
		wxLogDebug(wxString::Format("Best play (only considering cases where trump is not asked) - %s%s", 
			raLib::m_suits[raGetSuit(best_play)], 
			raLib::m_values[raGetValue(best_play)]));
#endif

		// Find out the best play after asking for the trump
		/*for(j = 0; j < move_trump_count; j++)
		{
			if(evals_trump[0][j].valid)
			{
				temp_eval = (double)evals_trump[0][j].eval / (double)evals_trump[0][j].count;
				if(temp_eval > best_eval_trump)
				{
					best_eval_trump = temp_eval;
					best_play_trump = moves_trump[j].card;
				}
			}
		}*/

		// Compare the options - 
		// Ask for trump or play without asking for trump
		if(avg_eval_trump.count > 0)
		{
			best_eval_trump = (double)avg_eval_trump.eval / (double)avg_eval_trump.count;
#ifdef raAI_LOG_GET_PLAY
			wxLogDebug(wxString::Format("Best_eval_trump - %5.2f", best_eval_trump));
#endif
			//wxASSERT(best_play_trump != raCARD_INVALID);
			if(best_eval_trump > best_eval)
			{
#ifdef raAI_LOG_GET_PLAY
				wxLogDebug(wxString::Format("Best_eval - %5.2f", best_eval));
				wxLogDebug("Best move is to ask for trump");
#endif
				best_play = -1;//best_play_trump;
			}
		}

	}
	// If trump is not shown and if self is not the max bidder
	// then trump is not known. Consider each suit as
	// a possible trump
	else
	{
#ifdef raAI_LOG_GET_PLAY
		wxLogDebug("Trump is not known");
#endif
		// Generate possible moves which do not ask for trump
		if(!GenerateMoves(&rule_engine, moves, &move_count, raAI_GENMV_NOTRUMP))
		{
			wxLogError(wxString::Format(wxT("GenerateMoves() failed. %s:%d"), 
				__FILE__, __LINE__));
			return -2;
		}
		wxASSERT(move_count > 0);

		wxASSERT(m_trump_cards);

#ifdef raAI_LOG_GET_PLAY
		wxLogDebug("Moves generated (without asking for trump) :");
		wxLogDebug(PrintMoves(moves, move_count));
#endif
		for(k = 0; k < raTOTAL_SUITS; k++)
		{
			// If the trump is not possible ignore 
			if(!(m_trump_cards & (1 << k)))
				continue;

			work_data.trump_suit = k;
			rule_engine.SetData(&work_data, false);

			// Generate moves which ask for trump
			if(!GenerateMoves(&rule_engine, moves_trump, &move_trump_count, raAI_GENMV_TRUMP))
			{
				wxLogError(wxString::Format(wxT("GenerateMoves() failed. %s:%d"), 
					__FILE__, __LINE__));
				return -2;
			}
			wxASSERT(move_trump_count >= 0);

#ifdef raAI_LOG_GET_PLAY
			wxLogDebug(wxString::Format("Trump - %s", raLib::m_suits[k].c_str()));
			wxLogDebug("Moves generated (asking for trump) :");
			wxLogDebug(PrintMoves(moves_trump, move_trump_count));
#endif

			// Generate random deals
			if(!GenerateDeals(&work_data, deal_hands, 30, k))
			{
				wxLogError(wxString::Format(wxT("GenerateDeals() failed. %s:%d"), 
					__FILE__, __LINE__));
				return -2;
			}
			// For each random deal
			for(i = 0; i < 30; i++)
			{
				memcpy(work_data.hands, deal_hands[i], sizeof(work_data.hands));
#ifdef raAI_LOG_GET_PLAY
				wxLogDebug(wxString::Format("Random deal no : %d", i));
#endif

				// Set the trump card 
				work_data.trump_card = GetTrump(work_data.hands[work_data.curr_max_bidder], work_data.trump_suit);

				wxASSERT((work_data.trump_card > raCARD_INVALID) && (work_data.trump_card < raTOTAL_CARDS));
				wxASSERT(work_data.hands[work_data.curr_max_bidder] & (1 << work_data.trump_card));

				// Remove the trump card from the max bidder's hand
				work_data.hands[work_data.curr_max_bidder] &= ~(1 << work_data.trump_card);

				for(j = 0; j < move_count; j++)
				{
					rule_engine.SetData(&work_data, false);
#ifdef raAI_LOG_GET_PLAY
					wxLogDebug("----------------------------------------------");
					wxLogDebug(wxString::Format("Random deal no : %d", i));
					wxLogDebug(wxString::Format("Trump : %s", raLib::m_suits[k].c_str()));
					wxLogDebug(wxString::Format("Attempting move no : %d", j));
					wxLogDebug(PrintMoves(&moves[j], 1));
					wxLogDebug("Rule engine data dump :");
					wxLogDebug(rule_engine.GetLoggable());
#endif
					if(!MakeMoveAndEval(&rule_engine, &moves[j], depth, &eval))
					{
						wxLogError(wxString::Format(wxT("MakeMoveAndEval() failed. %s:%d"), 
							__FILE__, __LINE__));
						return -2;
					}
					//wxLogDebug(wxString::Format("Eval for %s%s - %d (Trump - %s)",
					//	raLib::m_suits[raGetSuit(moves[j].card)].c_str(), 
					//	raLib::m_values[raGetValue(moves[j].card)].c_str(), 
					//	eval, raLib::m_suits[k].c_str()));
					evals[j].eval += eval;
					evals[j].count++;
					evals[j].valid = true;
#ifdef raAI_LOG_GET_PLAY
					wxLogDebug(wxString::Format("Eval - %d", eval));
					wxLogDebug(wxString::Format("evals[%d].eval = %d", j, evals[j].eval));
					wxLogDebug(wxString::Format("evals[%d].count = %d", j, evals[j].count));
#endif
				}

				rule_engine.SetData(&work_data, false);
				// Generate moves which ask for trump
				//wxLogDebug("Printing GetLoggable");
				//wxLogDebug(rule_engine.GetLoggable());
				if(!GenerateMoves(&rule_engine, moves_trump, &move_trump_count, raAI_GENMV_TRUMP))
				{
					wxLogError(wxString::Format(wxT("GenerateMoves() failed. %s:%d"), 
						__FILE__, __LINE__));
					return -2;
				}
				//wxLogDebug(PrintMoves(moves_trump, move_trump_count));
				wxASSERT(move_trump_count >= 0);

				eval_trump.count = 0;
				eval_trump.eval = raAI_NEG_INFTY;
				eval_trump.valid = false;

				for(j = 0; j < move_trump_count; j++)
				{
					rule_engine.SetData(&work_data, false);
#ifdef raAI_LOG_GET_PLAY
					wxLogDebug("----------------------------------------------");
					wxLogDebug(wxString::Format("Random deal no : %d", i));
					wxLogDebug(wxString::Format("Trump : %s", raLib::m_suits[k].c_str()));
					wxLogDebug(wxString::Format("Attempting move no : %d", j));
					wxLogDebug(PrintMoves(&moves_trump[j], 1));
					wxLogDebug("Rule engine data dump :");
					wxLogDebug(rule_engine.GetLoggable());
#endif
					if(!MakeMoveAndEval(&rule_engine, &moves_trump[j], depth, &eval))
					{
						wxLogError(wxString::Format(wxT("MakeMoveAndEval() failed. %s:%d"), 
							__FILE__, __LINE__));
						return -2;
					}
					if(eval > eval_trump.eval)
					{
						eval_trump.eval = eval;
						eval_trump.valid = true;
					}
					//evals_trump[k][j].eval += eval;
					//evals_trump[k][j].count++;
					//evals_trump[k][j].valid = true;
#ifdef raAI_LOG_GET_PLAY
					wxLogDebug(wxString::Format("Eval - %d", eval));
					wxLogDebug(wxString::Format("eval_trump.eval = %d", eval_trump.eval));
#endif
				}
				if(j > 0)
				{
					wxASSERT(eval_trump.valid);
					avg_eval_trump.count++;
					avg_eval_trump.eval += eval_trump.eval;
#ifdef raAI_LOG_GET_PLAY
					wxLogDebug(wxString::Format("avg_eval_trump.count - %d",avg_eval_trump.count));
					wxLogDebug(wxString::Format("avg_eval_trump.eval - %d",avg_eval_trump.eval));
#endif
				}
			}
		}

		best_eval = (double)raAI_NEG_INFTY;
		best_eval_trump = (double)raAI_NEG_INFTY;
		best_play = raCARD_INVALID;
		//best_play_trump = raCARD_INVALID;
#ifdef raAI_LOG_GET_PLAY
		wxLogDebug("Average evals for each move :");
#endif

		// Find out the best play without asking for the trump
		for(j = 0; j < move_count; j++)
		{
			if(evals[j].valid)
			{
				temp_eval = (double)evals[j].eval / (double)evals[j].count;
#ifdef raAI_LOG_GET_PLAY
				wxLogDebug(wxString::Format("%s - %5.2f", PrintMoves(&moves[j], 1).c_str(), temp_eval));
#endif
				if(temp_eval > best_eval)
				{
					best_eval = temp_eval;
					best_play = moves[j].card;
				}
			}
		}
		wxASSERT(best_play != raCARD_INVALID);
#ifdef raAI_LOG_GET_PLAY
		wxLogDebug(wxString::Format("Best play (only considering cases where trump is not asked) - %s%s", 
			raLib::m_suits[raGetSuit(best_play)], 
			raLib::m_values[raGetValue(best_play)]));
#endif

		// Compare the options - 
		// Ask for trump or play without asking for trump
		//wxLogDebug("Best evals %f, %f", best_eval, best_eval_trump);
		if(avg_eval_trump.count > 0)
		{
			// Find out the best play after asking for the trump
			best_eval_trump = (double)avg_eval_trump.eval / (double)avg_eval_trump.count;
#ifdef raAI_LOG_GET_PLAY
			wxLogDebug(wxString::Format("Best_eval_trump - %5.2f", best_eval_trump));
#endif
			if(best_eval_trump > best_eval)
			{
#ifdef raAI_LOG_GET_PLAY
				wxLogDebug(wxString::Format("Best_eval - %5.2f", best_eval));
				wxLogDebug("Best move is to ask for trump");
#endif
				best_play = -1;//best_play_trump;
			}
		}

	}

	// Free the memory allocated to hold the random deals
	for(i = 0; i < 30; i++)
	{
		delete[] deal_hands[i];
		deal_hands[i] = NULL;
	}
	delete[] deal_hands;
	deal_hands = NULL;

	wxASSERT((best_play == -1) || ((best_play > raCARD_INVALID) && (best_play < raTOTAL_CARDS)));

#ifdef raAI_LOG_GET_PLAY
	wxLogDebug("**********Exiting GetPlay()*****************");
#endif

	return best_play;
}
bool raAIAgent::GenerateSLProblem(raRuleEngineData *data, slProblem *problem, int trump)
{
	unsigned long cards_played = 0;
	int i, j;
	int sum_hands = 0, sum_suts = 0;

#ifdef raAI_LOG_GENERATESLPROBLEM
	wxLogDebug("Inside GenerateSLProblem");
	wxLogDebug(wxString::Format("m_loc - %s", raLib::m_short_locs[m_loc].c_str()));
#endif

	wxASSERT(data);

	if((!data->trump_shown) && (data->curr_max_bidder != m_loc))
		wxASSERT(trump != raSUIT_INVALID);

	if((data->trump_shown) || (data->curr_max_bidder == m_loc))
		trump = data->trump_suit;

	// TODO : Implement the case where the opponents of the max bidder
	// should have at least one trump

	// Set the hand lengths
	for(i = 0; i < raTOTAL_PLAYERS; i++)
	{
		problem->hand_total_length[i] = 8 - (bhLib::CountBitsSet(data->played_cards[i]));
		sum_hands += problem->hand_total_length[i]; 
		cards_played |= data->played_cards[i];
	}

	// Set suit lengths
	for(i = 0; i < raTOTAL_SUITS; i++)
	{
		problem->suit_total_length[i] = 8 - bhLib::CountBitsSet(cards_played & raLib::m_suit_mask[i]);
		sum_suts += problem->suit_total_length[i];
	}

	// If the trump is not shown, max bidder should have at least one trump
	if(!data->trump_shown)
		problem->cells[data->curr_max_bidder][trump].min = 1;

	// If the trump is shown, but if the max bidder is yet to play the 
	// card that was set as the trump then the minimum suit length is 1
	if(data->trump_shown)
	{
		if(!(data->played_cards[data->curr_max_bidder] & (1 << data->trump_card)))
		{
			problem->cells[data->curr_max_bidder][trump].min = 1;
		}
	}

	// Set the cases where the suit length is null
	for(i = 0; i < raTOTAL_PLAYERS; i++)
	{
		for(j = 0; j < raTOTAL_SUITS; j++)
		{
			problem->cells[i][j].max =
				raMin(problem->hand_total_length[i], problem->suit_total_length[j]);
			if(m_nulls[i] & (1 << j))
			{
				problem->cells[i][j].min = 0;
				problem->cells[i][j].max = 0;
			}
		}
	}

	// Set the cells for self
	for(i = 0; i < raTOTAL_SUITS; i++)
	{
		problem->cells[m_loc][i].min = bhLib::CountBitsSet(data->hands[m_loc] & raLib::m_suit_mask[i]);
		problem->cells[m_loc][i].max = problem->cells[m_loc][i].min;
	}
	// If self is the max bidder and if trump is not shown, 
	// add one to the suit length to accommodate the card kept as trump
	if((!data->trump_shown) && (m_loc == data->curr_max_bidder))
	{
		problem->cells[m_loc][trump].min++;
		problem->cells[m_loc][trump].max++;
	}

	wxASSERT(sum_suts == sum_hands);

#ifdef raAI_LOG_GENERATESLPROBLEM
	wxString out;
	wxLogDebug(slSolver::PrintProblem(problem));
	for(i = 0; i < raTOTAL_PLAYERS; i++)
	{
		out.Empty();
		out.Append(wxString::Format("%s - ", raLib::m_short_locs[i].c_str()));
		for(j = 0; j < raTOTAL_SUITS; j++)
		{
			out.Append(wxString::Format("%d/%d ", 
				problem->cells[i][j].min,
				problem->cells[i][j].max
				));
		}
		wxLogDebug(out);
	}
	wxLogDebug("Exiting GenerateSLProblem");
#endif

	return true;
}
//bool raAIAgent::GenerateSLSolution(slProblem *problem, slSolution *solution)
//{
//	slSolver solver;
//
//	wxASSERT(problem);
//	wxASSERT(solution);
//
//	solver.SetProblem(problem);
//	solver.GetRandomSolution(solution);
//	wxLogDebug(slSolver::PrintSolution(solution));
//
//	return true;
//}
bool raAIAgent::GenerateDeals(raRuleEngineData *data, unsigned long **deals, int count, int trump)
{
	int i, j, k, l;
	slProblem problem;
	slSolution solution;
	slSolver solver;
	int to_deal[raTOTAL_SUITS][raTOTAL_VALUES];
	int to_deal_count[raTOTAL_SUITS];
	unsigned long cards_played = 0;
	unsigned long temp;
	unsigned long known_alloc[raTOTAL_PLAYERS];
	

	wxASSERT(data);
	//wxASSERT(deals);

	//
	// Allocate the known cards straightaway
	//
	memset(known_alloc, 0, sizeof(known_alloc));

	// Cards beloning to self are known. Allocate those first
	known_alloc[m_loc] = data->hands[m_loc];

	// If self is the max bidder and if the trump is not shown
	// Add the card that is set as the trump to hand
	if((!data->trump_shown) && (m_loc == data->curr_max_bidder))
		known_alloc[m_loc] |= (1 << data->trump_card);

	// If trump is shown and the card that was set as trump is not
	// played yet, add the same to the max bidders hand
	if(data->trump_shown && (!(data->played_cards[data->curr_max_bidder] & (1 << data->trump_card))))
		known_alloc[data->curr_max_bidder] |= (1 << data->trump_card);

	// Consider the allocated cards as played
	for(i = 0; i < raTOTAL_PLAYERS; i++)
	{
		cards_played |= known_alloc[i];
	}

	// Get the list of cards to be dealt for each suit
	for(i = 0; i < raTOTAL_SUITS; i++)
	{
		cards_played |= data->played_cards[i];
		to_deal_count[i] = 0;
	}

	// For each suit, create an array of the cards to be dealt
	for(i = 0; i < raTOTAL_SUITS; i++)
	{
		temp = (~cards_played) & raLib::m_suit_mask[i];
		j = 0;
		while(temp)
		{
			to_deal[i][j] = bhLib::HighestBitSet(temp);
			temp &= ~(1 << to_deal[i][j]);
			j++;
		}
		to_deal_count[i] = j;
	}

	// Generate the problem
	memset(&problem, 0, sizeof(problem));
	if(!GenerateSLProblem(data, &problem, trump))
	{
		wxLogError(wxString::Format(wxT("GetData() failed. %s:%d"), 
			__FILE__, __LINE__));
		return false;
	}

	// 
	// Generate random deals
	//

	// Set the problem
	solver.SetProblem(&problem);
	for(i = 0; i < count; i++)
	{
		// Get a random solution
		memset(&solution, 0, sizeof(solution));
		solver.GetRandomSolution(&solution);
		memcpy(deals[i], known_alloc, sizeof(known_alloc));

		// For each array shuffle the cards to be dealt
		for(j = 0; j < raTOTAL_SUITS; j++)
		{
			raLib::ShuffleArray(to_deal[j], to_deal_count[j]);
		}
		//wxLogDebug(slSolver::PrintSolution(&solution));

		// For each player deal the undealt cards
		for(k = 0; k < raTOTAL_SUITS; k++)
		{
			l = 0;
			for(j = 0; j < raTOTAL_PLAYERS; j++)
			{
				//wxLogDebug(wxString::Format("Compare with solution %d %d",
				//	(int)bhLib::CountBitsSet(deals[i][j] & raLib::m_suit_mask[k]),
				//	solution.suit_length[j][k]));
				wxASSERT((int)bhLib::CountBitsSet(deals[i][j] & raLib::m_suit_mask[k]) <= solution.suit_length[j][k]);
				while((int)bhLib::CountBitsSet(deals[i][j] & raLib::m_suit_mask[k]) < solution.suit_length[j][k])
				{
					wxASSERT(l < to_deal_count[k]);
					deals[i][j] |= (1 << to_deal[k][l]);
					l++;
				}
			}
		}
		//wxLogDebug(raLib::PrintHands(deals[i]));

	}


	return true;
}
wxString raAIAgent::PrintMoves(raAIMove *moves, int move_count)
{
	wxString out;
	int i;
	wxASSERT(move_count >= 0);
	out.Append(wxString::Format("%d moves - ", move_count));
	for(i = 0; i < move_count; i++)
	{
		if(moves[i].ask_trump)
		{
			out.Append(wxString::Format("?%s%s,",
				raLib::m_suits[raGetSuit(moves[i].card)].c_str(),
				raLib::m_values[raGetValue(moves[i].card)].c_str()
				));
		}
		else
		{
			out.Append(wxString::Format("%s%s,",
				raLib::m_suits[raGetSuit(moves[i].card)].c_str(),
				raLib::m_values[raGetValue(moves[i].card)].c_str()
				));
		}
	}
	return out;
}

bool raAIAgent::PostPlayUpdate(raRuleEngineData *data, int card)
{
	int cards_left = 0;
	unsigned long cards_played = 0;
	int i;
	int suit;

	wxASSERT(card != raCARD_INVALID);
	wxASSERT(data->status == raSTATUS_TRICKS);

	// If the input card is valid
	if(card != raCARD_INVALID)
	{

		suit = raGetSuit(card);

		// If trump is not shown and if the player is the max bidder and
		// if it is the first card in the trick then the suit is not trump, 
		// provided the max bidder has a choice of another suit
		if(
			!data->trump_shown &&
			(data->in_trick_info.player == data->curr_max_bidder) //&&
			//(data->tricks[data->trick_round].count == 0)
			)
		{
			if(data->tricks[data->trick_round].count == 0 )
			{
				// Get the count of cards belonging to the suit
				// already played
				for(i = 0; i < raTOTAL_PLAYERS; i++)
				{
					cards_left += bhLib::CountBitsSet(data->played_cards[i] & raLib::m_suit_mask[suit]);
				}
				// Add to that the number of cards held by the AI player
				// belonging to the suit
				cards_left += bhLib::CountBitsSet(data->hands[m_loc] & raLib::m_suit_mask[suit]);
				// Consider the card being played
				cards_left++;

				// Check whether the number of cards left in the suit
				// is less than the number of cards left in max bidders hand
				// after the play
				// (8 - data->trick_round) > (8 - cards_left)
				if(data->trick_round < cards_left)
				{
					//wxLogDebug(wxString::Format("%s is not the trump", raLib::m_suits[suit].c_str()));
					m_trump_cards &= ~(1 << suit);
				}
				else
				{
					m_notrump_suspects |= (1 << suit);
				}
			}

			// Check each of the suspects, if the max bidder has played a card
			// which is not any of the suspects, the suspicion is valid
			if(m_notrump_suspects)
			{
				for(i = 0; i < raTOTAL_SUITS; i++)
				{
					//if((m_notrump_suspects & (1 << suit)) && (i != suit))
					if((m_notrump_suspects & (1 << i)) && (i != suit))
					{
						// Remove the suit from the list of possible trump suits
						m_trump_cards &= ~(1 << i);
						// Remove the suit from the list of suspects
						m_notrump_suspects &= ~(1 << i);
					}
				}
			}
		}

		wxASSERT(m_trump_cards);

		// Check the cases where a player is unable to follow suit
		if(
			(data->tricks[data->trick_round].count > 0) &&
			(data->tricks[data->trick_round].lead_suit != suit)
			)
		{
			// If the current player is the max bidder
			// we can safely assume that either the suit is the trump
			// and the max bidder has only one card of the suit left and 
			// that is kept as the trump. Otherwise the suit length is zero.
			// This can be confirmed only after the trump is shown
			if(
				(data->in_trick_info.player == data->curr_max_bidder) &&
				(!data->trump_shown)
				)
			{
				m_mb_null_susp |= (1 << data->tricks[data->trick_round].lead_suit);
			}
			// If the player playing the card is not the max bidder
			// we can safely assume that the suit length for the player
			// is zero
			else
			{
				m_nulls[data->in_trick_info.player] |= (1 << data->tricks[data->trick_round].lead_suit);
			}
		}
	}

	// If trump is shown and if any of the null suspects is not 
	// the trump set the suit length in max bidders hand to zero
	// for those
	if((data->trump_shown) && (m_mb_null_susp))
	{
		for(i = 0; i < raTOTAL_SUITS; i++)	
		{
			if((m_mb_null_susp & (1 << i)) && (i != data->trump_suit))
			{
				m_nulls[data->curr_max_bidder] |= (1 << i);
				// Remove from the suspect list
				m_mb_null_susp &= ~(1 << i);
			}
		}
	}

	// If self is not the max bidder and trump is not shown,
	// For any suit if the sum of the cards played(including the
	// the cards played in the current trick) and cards held by self
	// is 8, then the suit is not the trump.
	// This is because max bidder cannot have any card of the suit
	if((m_loc != data->curr_max_bidder) && (!data->trump_shown))
	{
		cards_played = 0;

		// Add the card that is being played first
		if(card != raCARD_INVALID)
		{
			cards_played |= (1 << card);
		}

		// Add the cards played so far (previous tricks and the current one)
		for(i = 0; i < raTOTAL_PLAYERS; i++)	
		{
			cards_played |= data->played_cards[i];
			if(data->tricks[data->trick_round].cards[i] != raCARD_INVALID)
				cards_played |= (1 << data->tricks[data->trick_round].cards[i]);
		}

		//wxLogDebug("Here");
		//wxLogDebug(raLib::PrintHands(data->played_cards));
		for(i = 0; i < raTOTAL_SUITS; i++)	
		{
			//wxLogDebug(wxString::Format("Debug count - %d", 
			//	bhLib::CountBitsSet((cards_played | data->hands[m_loc]) & 
			//	raLib::m_suit_mask[i])));
			if(bhLib::CountBitsSet((cards_played | data->hands[m_loc]) & 
				raLib::m_suit_mask[i])>= raTOTAL_VALUES)
			{
				m_trump_cards &= ~(1 << i);
			}
		}
	}

	/*
	wxLogDebug("---------------------------------");
	wxLogDebug(wxString::Format("Logged from %s AI", raLib::m_short_locs[m_loc].c_str()));
	for(i = 0; i < raTOTAL_SUITS; i++)
	{
		if(m_trump_cards & (1 << i))
		{
			wxLogDebug(wxString::Format("%s can be trump", raLib::m_suits[i].c_str()));
		}
		else
		{
			wxLogDebug(wxString::Format("%s cannot be trump", raLib::m_suits[i].c_str()));
		}
	}
	int j;
	for(j = 0; j < raTOTAL_PLAYERS; j++)
	{
		for(i = 0; i < raTOTAL_SUITS; i++)
		{
			if(m_nulls[j] & (1 << i))
			{
				wxLogDebug(wxString::Format("%s does not have %s", 
					raLib::m_short_locs[j].c_str(),
					raLib::m_suits[i].c_str()));
			}
		}
	}
	wxLogDebug("---------------------------------");
	*/

	return true;
}
bool raAIAgent::CheckAssumptions(raRuleEngineData *data)
{
	int i;	

#ifdef raAI_LOG_CHECKASSUMPTIONS
#endif
	for(i = 0; i < raTOTAL_SUITS; i++)
	{
		if(!(m_trump_cards & (1 << i)))
		{
			if(data->trump_suit == i)
			{
				wxLogDebug("Dummy");
			}
			wxASSERT(data->trump_suit != i);
		}
	}
	int j;
	for(j = 0; j < raTOTAL_PLAYERS; j++)
	{
		for(i = 0; i < raTOTAL_SUITS; i++)
		{
			if(m_nulls[j] & (1 << i))
			{
				wxASSERT(!(data->hands[j] & raLib::m_suit_mask[i]));
				//wxLogDebug(wxString::Format("%s does not have %s", 
				//	raLib::m_short_locs[j].c_str(),
				//	raLib::m_suits[i].c_str()));
			}
		}
	}

	return true;
}

bool raAIAgent::Reset()
{
	int i;

	m_engine.Reset();
	m_trump_cards = 0x0000000F;
	for(i = 0; i < raTOTAL_PLAYERS; i++)
		m_nulls[i] = 0;
	m_notrump_suspects = 0;
	m_mb_null_susp = 0;
	return true;
}

bool raAIAgent::SetClockwise(bool flag)
{
	raRuleEngineData data;
	m_engine.GetData(&data);
	if(flag)
		data.rules.rot_addn = 1;
	else
		data.rules.rot_addn = 3;
	m_engine.SetData(&data, false);

	return true;
}
bool raAIAgent::GetClockwise()
{
	raRuleEngineData data;
	m_engine.GetData(&data);
	switch(data.rules.rot_addn)
	{
	case 1:
		return true;
		break;
	case 3:
		return false;
		break;
	default:
		return false;
		break;
	}

	return false;
}
bool raAIAgent::AbandonGame(bool *flag)
{
	wxASSERT(flag);
	// TODO : Add more intelligent logic
	*flag = true;
	return true;
}


//
// Private method/s
//
bool raAIAgent::EstimateTricks(unsigned long *p_hands, int trump, int *eval)
{
	int i, j, k; // Multi-purpose counters
	unsigned long combined[2];
	unsigned long suit[2];
	int stronger, tricks[2];
	int suit_count[4];
	int trump_adv;

	// Initialization
	tricks[0] = 0;
	tricks[1] = 0;


	// Combine each teams hands
	combined[0] = p_hands[0] | p_hands[2];
	combined[1] = p_hands[1] | p_hands[3];

#ifdef raAI_LOG_ESTIMATE_TRICKS
	wxLogDebug("----------------------------------------------------");

	wxLogDebug(raLib::PrintHands(p_hands));
	wxLogDebug(_("Trump is ") + raLib::m_suits[trump]);
	wxLogDebug("Combined hands :");
	wxLogDebug(_("N/S - ") + raLib::PrintLong(combined[0]));
	wxLogDebug(_("E/W - ") + raLib::PrintLong(combined[1]));
#endif

	// Loop though each of the suits
	for(i = 0; i < 4; i++)
	{
		suit[0] = (combined[0] & raLib::m_suit_mask[i]) >> raLib::m_suit_rs[i];
		suit[1] = (combined[1] & raLib::m_suit_mask[i]) >> raLib::m_suit_rs[i];
#ifdef raAI_LOG_ESTIMATE_TRICKS
		wxLogDebug("-----------------------");
		wxLogDebug(_("Evaluating suit - ") + raLib::m_suits[i]);
#endif
		if(suit[0] > suit[1])
			j = 0;
		else
			j = 1;

#ifdef raAI_LOG_ESTIMATE_TRICKS
		wxLogDebug(_("Stronger team is - ") + raLib::m_short_teams[j]);
#endif

		// Number of cards with the stronger team which are stronger than
		// the strongest card in the weakest team

		stronger = bhLib::CountBitsSet(
			suit[j] & (0xFFFFFFFF << bhLib::HighestBitSet(suit[!j])));
#ifdef raAI_LOG_ESTIMATE_TRICKS
		wxLogDebug(wxString::Format("Number of stronger cards - %d", stronger));
#endif

		suit_count[!j] =
			bhLib::CountBitsSet(p_hands[!j] & raLib::m_suit_mask[i]);
		suit_count[(!j) + 2] =
			bhLib::CountBitsSet(p_hands[(!j) + 2] & raLib::m_suit_mask[i]);
#ifdef raAI_LOG_ESTIMATE_TRICKS
		wxLogDebug(_("Individual counts of cards with opposition ") + raLib::m_short_teams[!j]);
		wxLogDebug(wxString::Format("%s - %d", raLib::m_long_locs[!j].c_str(), suit_count[!j]));
		wxLogDebug(wxString::Format("%s - %d", raLib::m_long_locs[(!j) + 2].c_str(), suit_count[(!j) + 2]));
#endif

		// If the suit is not the trump suit,
		//   take the minimum of stronger cards and the number of cards
		//   with each of the weaker locations. This is to accommodate
		//   chances of getting trumped
		// If the suit is the trump suit,
		//   the number of tricks expected is the sum of
		//   the number of stronger cards
		//   and the difference of maximum number of cards

		if(i == trump)
		{
			suit_count[j] =
				bhLib::CountBitsSet(p_hands[j] & raLib::m_suit_mask[i]);
			suit_count[j + 2] =
				bhLib::CountBitsSet(p_hands[j + 2] & raLib::m_suit_mask[i]);

#ifdef raAI_LOG_ESTIMATE_TRICKS
			wxLogDebug(_("Tricks expected - ") +
				wxString::Format("%d", raMin(stronger, raMax(suit_count[j], suit_count[j + 2]))));
#endif
			tricks[j] += raMin(stronger, raMax(suit_count[j], suit_count[j + 2]));

			//
			// Calculating trump advantage
			//
			// Remove cards from each of the hands, equal to the number of
			// tricks expected. If there is any advantage in the suit length
			// after this, that is considered
			for(k = 0; k < 4; k++)
			{
				// Is this the best way to do this sort of negation?
				suit_count[k] -= stronger;
				if(suit_count[k] < 0)
					suit_count[k] = 0;
			}

			trump_adv = raMax(suit_count[j], suit_count[j + 2])
				- raMax(suit_count[!j], suit_count[(!j) + 2]);
#ifdef raAI_LOG_ESTIMATE_TRICKS
			wxLogDebug(_("Trump advantage for ") + raLib::m_short_teams[j].c_str() + _(" is ") + wxString::Format("%d", trump_adv));
#endif
			if(trump_adv > 0)
				tricks[j] += trump_adv;
			else
				tricks[!j] -= trump_adv;
		}
		else
		{
#ifdef raAI_LOG_ESTIMATE_TRICKS
			wxLogDebug(_("Tricks expected - ") +
				wxString::Format("%d", raMin(raMin(suit_count[!j], suit_count[(!j) + 2]), stronger)));
#endif
			tricks[j] += raMin(raMin(suit_count[!j], suit_count[(!j) + 2]), stronger);
		}
	}
	// If either is greater that 8 correct
	if(tricks[0] > 8)
		tricks[0] = 8;
	if(tricks[1] > 8)
		tricks[1] = 8;

#ifdef raAI_LOG_ESTIMATE_TRICKS
	wxLogDebug(wxString::Format("Total no of tricks expected %d, %d", tricks[0], tricks[1]));
	wxLogDebug("----------------------------------------------------");
#endif
	eval[0] = tricks[0];
	eval[1] = tricks[1];
	return true;
}
bool raAIAgent::EstimatePoints(unsigned long *hands, int trump, int trick_no, int *eval)
{
	int trick_count[2];
	unsigned long all_cards = 0;
	int total_pts;

	// Set both ints in eval to 0
	memset(eval, 0, 2 * sizeof(int));

	all_cards =  hands[0] | hands[1] | hands[2] | hands[3];
	total_pts = raTotalPoints(all_cards);

	EstimateTricks(hands, trump, trick_count);

	// Share points according to the tricks estimated

	eval[0] = (total_pts * trick_count[0]) / (8 - trick_no);
	eval[1] = (total_pts * trick_count[1]) / (8 - trick_no);
#ifdef  raAI_LOG_ESTIMATE_POINTS
	wxLogDebug(wxString::Format("Points expected before sharing - %d, %d", eval[0], eval[1]));
	wxLogDebug(wxString::Format("Points shared- %d", (total_pts - eval[0] - eval[1]) / 4));
#endif

	// Share the half of the rest of the points equally
	eval[0] += (total_pts - eval[0] - eval[1]) / 4;
	eval[1] += (total_pts - eval[0] - eval[1]) / 4;

#ifdef  raAI_LOG_ESTIMATE_POINTS
	wxLogDebug(wxString::Format("Points expected (final) - %d, %d", eval[0], eval[1]));
#endif

	return true;
}

bool raAIAgent::GenerateMoves(raRuleEngine *node, raAIMove *moves, int *count, int type)
{
	raInputTrickInfo trick_info;
	int i = 0;
	unsigned long cards_left;
	unsigned long hands[raTOTAL_PLAYERS];
	raRuleEngine rule_engine;
	raRuleEngineData re_data;
#ifdef raAI_LOG_GENERATEMOVES
	wxString out;
#endif

	wxASSERT(moves);
	wxASSERT(count);

	// Check whether the rule engine is expecting a
	// card to be played
	if(node->GetPendingInputType() != raINPUT_TRICK)
	{
		wxLogError(wxString::Format(wxT("Trick not expected. %s:%d"),
			__FILE__, __LINE__));
		return false;
	}

	// Get the input criteria for future use
	if(!node->GetPendingInputCriteria(NULL, &trick_info))
	{
		wxLogError(wxString::Format(wxT("GetPendingInputCriteria failed. %s:%d"),
			__FILE__, __LINE__));
		return false;
	}

	// Depending on the type passed, generate 
	// moves which do not ask for the trump

	if(type & raAI_GENMV_NOTRUMP)
	{
		// Get the cards in the hand and generate all possible card plays
		// which can be played without asking for trump
		node->GetHands(hands);
		cards_left = hands[trick_info.player];
		cards_left &= trick_info.mask;

		while(cards_left)
		{
			moves[i].ask_trump = false;
			moves[i].card = bhLib::HighestBitSet(cards_left);
			cards_left &= ~(1 << moves[i].card);
			i++;
		}
	}
	// Depending on the type passed, generate moves
	// that ask for the trump
	if(type & raAI_GENMV_TRUMP)
	{
		// If trump can be asked, generate all possible card plays
		// than can be made after asking for the trump

		if(trick_info.can_ask_trump)
		{
			node->GetData(&re_data);
			rule_engine.SetData(&re_data, false);

			trick_info.ask_trump = true;
			if(rule_engine.PostInputMessage(raINPUT_TRICK, &trick_info))
			{
				wxLogError(wxString::Format(wxT("PostInputMessage failed. %s:%d"),
					__FILE__, __LINE__));
				return false;
			}
			while(rule_engine.Continue());

			if(rule_engine.GetPendingInputType() != raINPUT_TRICK)
			{
				wxLogError(wxString::Format(wxT("Trick not expected. %s:%d"),
					__FILE__, __LINE__));
				return false;
			}

			// Get the input criteria
			if(!rule_engine.GetPendingInputCriteria(NULL, &trick_info))
			{
				wxLogError(wxString::Format(wxT("GetPendingInputCriteria failed. %s:%d"),
					__FILE__, __LINE__));
				return false;
			}

			rule_engine.GetHands(hands);
			cards_left = hands[trick_info.player];
			cards_left &= trick_info.mask;
			//wxLogDebug(wxString::Format("Trump - %s", raLib::m_suits[rule_engine.GetTrump()].c_str()));
			//wxLogDebug(wxString::Format("Mask is %0X", trick_info.mask));

			while(cards_left)
			{
				moves[i].ask_trump = true;
				moves[i].card = bhLib::HighestBitSet(cards_left);
				cards_left &= ~(1 << moves[i].card);
				i++;
			}

		}
	}

	wxASSERT(i <= raAI_MAX_MOVES);
	*count = i;

	// TODO : Use PrintMoves()
#ifdef raAI_LOG_GENERATEMOVES
	out.Append(wxString::Format("%d moves generated for - ", *count));
	for(i = 0; i < *count; i++)
	{
		if(moves[i].ask_trump)
		{
			out.Append(wxString::Format("?%s%s,",
				raLib::m_suits[raGetSuit(moves[i].card)].c_str(),
				raLib::m_values[raGetValue(moves[i].card)].c_str()
				));
		}
		else
		{
			out.Append(wxString::Format("%s%s,",
				raLib::m_suits[raGetSuit(moves[i].card)].c_str(),
				raLib::m_values[raGetValue(moves[i].card)].c_str()
				));
		}
	}
	wxLogDebug(out);
#endif

	return true;
}

/*
* Alpha-Beta search...
* Pseudo code -
evaluate (node, alpha, beta)
	if node is a leaf
		return the heuristic value of node
	if node is a minimizing node
		for each child of node
			beta = min (beta, evaluate (child, alpha, beta))
			if beta <= alpha
				return alpha
			return beta
	if node is a maximizing node
		for each child of node
			alpha = max (alpha, evaluate (child, alpha, beta))
			if beta <= alpha
				return beta
			return alpha
*
*/
int raAIAgent::Evaluate(raRuleEngine *node, int alpha, int beta, int depth, bool *ret_val)
{
	int eval;
	raRuleEngineData old_node;
	int i;
	int trick_round;
	raInputTrickInfo trick_info;
	raAIMove moves[raAI_MAX_MOVES];
	int move_count;
	int ret_heur;

	wxASSERT(ret_val);
	*ret_val = true;

	trick_round = node->GetTrickRound();
	wxASSERT((trick_round >= 0) && (trick_round <= 8));

#ifdef raAI_LOG_EVALUATE
	wxLogDebug(wxString::Format(wxT("Evaluating round %d. %s:%d"),
		trick_round, __FILE__, __LINE__));
#endif

	// If node is leaf, estimate heuristic
	if((trick_round == 8) || (trick_round >= depth))
	{
		ret_heur = EstimateHeuristic(node);
#ifdef raAI_LOG_EVALUATE
		wxLogDebug("Logging at leaf");
		wxLogDebug("-------------------------------");
		wxLogDebug(node->GetLoggable());
		wxLogDebug(wxString::Format("Estimated Heuristic - %d", ret_heur));
		wxLogDebug("-------------------------------");
#endif
		return ret_heur;
	}

	// Backup the current state of the rule engine
	node->GetData(&old_node);

	if(node->GetPendingInputType() != raINPUT_TRICK)
	{
		wxLogError(wxString::Format(wxT("Unexpected input type. %s:%d"),
			__FILE__, __LINE__));
		*ret_val = false;
		return 0;
	}

	if(!node->GetPendingInputCriteria(NULL, &trick_info))
	{
		wxLogError(wxString::Format(wxT("GetPendingInputCriteria failed. %s:%d"),
			__FILE__, __LINE__));
		*ret_val = false;
		return 0;
	}

	//if node is a minimizing node
	if((trick_info.player & 1) != (m_loc & 1))
	{
		GenerateMoves(node, moves, &move_count);
		wxASSERT(move_count);
		if(move_count <= 0)
		{
			wxLogError(node->GetLoggable());
		}
		// for each child of node
		for(i = 0; i < move_count; i++)
		{
			//::wxYield();
			if(!MakeMove(node, &moves[i]))
			{
				wxLogError(wxString::Format(wxT("MakeMove failed. %s:%d"),
					__FILE__, __LINE__));
				*ret_val = false;
				return 0;
			}
			//beta = min (beta, evaluate (child, alpha, beta))
			eval =  Evaluate(node, alpha, beta, depth, ret_val);
			if(!*ret_val)
			{
				wxLogError(wxString::Format(wxT("Evaluate failed. %s:%d"),
					__FILE__, __LINE__));
				return 0;
			}
			beta = raMin(beta, eval);
			node->SetData(&old_node, false);
			//if beta <= alpha
			//	return alpha
			if(beta <= alpha)
			{
				return alpha;
			}
		}
		//return beta
		return beta;

	}
	//if node is a maximizing node
	else
	{
		GenerateMoves(node, moves, &move_count);
		wxASSERT(move_count);
		if(move_count <= 0)
		{
			wxLogError(node->GetLoggable());
		}
		// for each child of node
		for(i = 0; i < move_count; i++)
		{
			//::wxYield();
			if(!MakeMove(node, &moves[i]))
			{
				wxLogError(wxString::Format(wxT("MakeMove failed. %s:%d"),
					__FILE__, __LINE__));
				*ret_val = false;
				return 0;
			}
			//alpha = max (alpha, evaluate (child, alpha, beta))
			eval = Evaluate(node, alpha, beta, depth, ret_val);
			if(!*ret_val)
			{
				wxLogError(wxString::Format(wxT("Evaluate failed. %s:%d"),
					__FILE__, __LINE__));
				return 0;
			}
			alpha = raMax(alpha, eval);
			node->SetData(&old_node, false);
			//if beta <= alpha
			//    return beta
			if(beta <= alpha)
			{
				return beta;
			}
		}
		// return alpha
		return alpha;
	}

	return 0;
}

int raAIAgent::EstimateHeuristic(raRuleEngine *state)
{
	int pts[2];
	int estimated[2];
	int trick_no;
	int bid;
	int loc;

	//unsigned long played_cards[raTOTAL_PLAYERS];
	unsigned long hands[raTOTAL_PLAYERS];

	trick_no = state->GetTrickRound();
	state->GetMaxBid(&bid, &loc);
	state->GetPoints(pts);

	if(trick_no == 8)
	{
		// If the maximum bid was made a player from the same team
		if((loc & 1) == (m_loc & 1))
		{
			return ((pts[m_loc & 1] * (29 -bid)) -  (pts[!(m_loc & 1)] * bid));
		}
		else
		{
			return ((pts[m_loc & 1] * bid) -  (pts[!(m_loc & 1)] * (29 - bid)));
		}
	}
	else
	{
		state->GetHands(hands);
		EstimatePoints(hands, state->GetTrump(), trick_no, estimated);
		if((loc & 1) == (m_loc & 1))
		{
			return ((estimated[m_loc & 1] + pts[m_loc & 1]) * (29 - bid)) -
				((estimated[!(m_loc & 1)] + pts[!(m_loc & 1)]) * bid);
		}
		else
		{
			return ((estimated[m_loc & 1] + pts[m_loc & 1]) * bid) -
				((estimated[!(m_loc & 1)] + pts[!(m_loc & 1)]) * (29 - bid));
		}
	}
	return 0;
}

bool raAIAgent::MakeMove(raRuleEngine *node, raAIMove *move)
{
	raInputTrickInfo trick_info;

	// Obtain the current input criteria and verify

	if(node->GetPendingInputType() != raINPUT_TRICK)
	{
		wxLogError(wxString::Format(wxT("GetPendingInputCriteria failed. %s:%d"),
			__FILE__, __LINE__));
		return false;
	}

	if(!node->GetPendingInputCriteria(NULL, &trick_info))
	{
		wxLogError(wxString::Format(wxT("GetPendingInputCriteria failed. %s:%d"),
			__FILE__, __LINE__));
		return false;
	}

	// If trump needs to be asked for firstly, do that
	if(move->ask_trump)
	{
		wxASSERT(trick_info.can_ask_trump);
		trick_info.ask_trump = true;
		if(node->PostInputMessage(raINPUT_TRICK, &trick_info))
		{
			wxLogError(wxString::Format(wxT("PostInputMessage failed. %s:%d"),
				__FILE__, __LINE__));
			return false;
		}
		while(node->Continue());

		if(node->GetPendingInputType() != raINPUT_TRICK)
		{
			wxLogError(wxString::Format(wxT("GetPendingInputCriteria failed. %s:%d"),
				__FILE__, __LINE__));
			return false;
		}

		if(!node->GetPendingInputCriteria(NULL, &trick_info))
		{
			wxLogError(wxString::Format(wxT("GetPendingInputCriteria failed. %s:%d"),
				__FILE__, __LINE__));
			return false;
		}
	}

	// Make the move
	trick_info.card = move->card;

#ifdef raAI_LOG_MAKEMOVE
	wxLogDebug(wxString::Format(wxT("%s making a move %s%s. %s:%d"),
		raLib::m_short_locs[trick_info.player].c_str(),
		raLib::m_suits[raGetSuit(move->card)].c_str(),
		raLib::m_values[raGetValue(move->card)].c_str(),
		__FILE__, __LINE__));
#endif
	if(node->PostInputMessage(raINPUT_TRICK, &trick_info))
	{
		node->PostInputMessage(raINPUT_TRICK, &trick_info);
		wxLogDebug(wxString::Format("Player %s Card %s%s", 
			raLib::m_short_locs[trick_info.player].c_str(),
			raLib::m_suits[raGetSuit(trick_info.card)].c_str(),
			raLib::m_values[raGetValue(trick_info.card)].c_str()
			));
		wxLogError(wxString::Format(wxT("PostInputMessage failed. %s:%d"),
			__FILE__, __LINE__));
		return false;
	}

	// Continue the play
	while(node->Continue());

	return true;
}

bool raAIAgent::MakeMoveAndEval(raRuleEngine *node, raAIMove *move, int depth, int *eval)
{
	bool eval_ret;
	int temp;

	wxASSERT(node);
	wxASSERT(eval);
	wxASSERT(move);
	wxASSERT(depth > 0);

	if(!MakeMove(node, move))
	{
		wxLogError(wxString::Format(wxT("MakeMove failed. %s:%d"),
			__FILE__, __LINE__));
		wxLogError(node->GetLoggable());
		if(move->ask_trump)
		{
			wxLogError(wxString::Format("Move attempted ?%s%s",
				raLib::m_suits[raGetSuit(move->card)].c_str(),
				raLib::m_values[raGetValue(move->card)].c_str()
				));
		}
		else
		{
			wxLogError(wxString::Format("Move attempted %s%s",
				raLib::m_suits[raGetSuit(move->card)].c_str(),
				raLib::m_values[raGetValue(move->card)].c_str()
				));
		}
		return false;
	}

	eval_ret = false;
	temp = raAI_NEG_INFTY;
	temp = Evaluate(node, raAI_NEG_INFTY, raAI_POS_INFTY, depth, &eval_ret);
	wxASSERT(temp != raAI_NEG_INFTY);
	if(!eval_ret)
	{
		wxLogError(wxString::Format(wxT("Evaluate failed. %s:%d"),
			__FILE__, __LINE__));
		return false;
	}

	*eval = temp;

	return true;
}

