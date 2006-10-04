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
	wxLogDebug(wxString::Format("Estimated bid for %d", m_loc));
	wxLogDebug(raLib::PrintLong(hands[m_loc]));

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
	unsigned long trump_cards;
	unsigned long hands[raTOTAL_PLAYERS];
	int i;

	m_engine.GetHands(hands);
	wxLogDebug(wxString::Format("Estimated bid for %d", m_loc));
	wxLogDebug(raLib::PrintLong(hands[m_loc]));

	GetBid(&bid, &trump, 14, true);
#ifdef raAI_LOG_GETTRUMP
	wxLogDebug(_("raPlayer::GetTrump - Entering"));
	wxLogDebug(_("raPlayer::GetTrump - Hand : ") + raLib::PrintLong(m_cards));
	wxLogDebug(_("raPlayer::GetTrump - Trump suggested is : ") + raLib::m_suits[trump].c_str());
#endif
	trump_cards = (hands[m_loc] & raLib::m_suit_mask[trump]) >> raLib::m_suit_rs[trump];
	// If there are two cards with points
	// select smallest as the trump
	if((bhLib::CountBitsSet(trump_cards & 0x0000000F0)) >= 2)
	{
#ifdef raAI_LOG_GETTRUMP
		wxLogDebug(_("raPlayer::GetTrump - There is at least two cards with points"));
#endif
		for(i = 4; i < 8; i++)
			if(trump_cards & (1 << i))
				return (trump * 8) + i;
	}
	// Else, if you have at least on card
	// with no points select the highest as the trump
	else if(bhLib::CountBitsSet(trump_cards & 0x0000000F) > 0)
	{
#ifdef raAI_LOG_GETTRUMP
		wxLogDebug(_("raPlayer::GetTrump - There is at least one card without points"));
#endif
		return (trump * 8) + bhLib::HighestBitSet(trump_cards & 0x0000000F);
	}
	// Else, select the highest card as trump
	else
	{
#ifdef raAI_LOG_GETTRUMP
		wxLogDebug(_("raPlayer::GetTrump - There is only one card and that too with points"));
#endif
		return (trump * 8) + bhLib::HighestBitSet(trump_cards);
	}
	return raCARD_INVALID;
}

// Function returns the card to be played(index)
// -1, for show trump
// -2 or other negative values in case of error

int raAIAgent::GetPlay(unsigned long mask)
{
	raInputTrickInfo trick_info;
	// TODO : Remove hardcoding
	raAIMove moves[raAI_MAX_MOVES];
	int move_count;
	int i, j, k, l;
	raRuleEngine rule_engine;
	raRuleEngineData re_data, work_data, bkp_data;
	bool eval_ret;
	int best_eval;
	int best_move;
	int eval;
	int max_bidder;
	int depth;

	int *deck;
	unsigned long to_deal;
	int to_deal_total;
	int to_deal_count[raTOTAL_PLAYERS];
	bool is_trump_shown;

	raAIEval evals_ntrump[raTOTAL_CARDS];
	raAIEval evals_trump;


	// Remove hard coding
	int sample_counts[raTOTAL_CARDS];
	int sample_count_trump = 0;

	//wxASSERT(mask);
	// Verify the status of the rule engine.
	if(m_engine.GetPendingInputType() != raINPUT_TRICK)
	{
		wxLogError(wxString::Format(wxT("Trick not expected. %s:%d"),
			__FILE__, __LINE__));
		return -2;
	}
	// Verify that the next player expected to play
	// is the current player

	if(!m_engine.GetPendingInputCriteria(NULL, &trick_info))
	{
		wxLogError(wxString::Format(wxT("GetPendingInputCriteria failed. %s:%d"),
			__FILE__, __LINE__));
		return -2;
	}
	if(trick_info.player != m_loc)
	{
		wxLogError(wxString::Format(wxT("Player expected to play is not the current player. %s:%d"),
			__FILE__, __LINE__));
		return -2;
	}

	depth = s_depths[m_engine.GetTrickRound()];
	wxASSERT((depth > 0) && (depth <= raTOTAL_TRICKS));

	m_engine.GetData(&re_data);
	m_engine.GetMaxBid(NULL, &max_bidder);
	is_trump_shown = m_engine.IsTrumpShown();

	// Obtain the list of cards to be dealt the rest of the players
	to_deal = 0xFFFFFFFF;		// Complete deck
	to_deal &= ~(re_data.hands[m_loc]
		| re_data.played_cards[0]
		| re_data.played_cards[1]
		| re_data.played_cards[2]
		| re_data.played_cards[3]
	);
	// If the current location has set the trump, then trump card is not in
	// the players hand. Remove that from the list of cards to be dealt
	if((max_bidder == m_loc) && !is_trump_shown)
	{
		wxASSERT((re_data.trump_card >= 0) && (re_data.trump_card < raTOTAL_CARDS));
		to_deal &= ~(re_data.trump_card);
	}

	// Obtain the count of cards to be dealt
	to_deal_total = bhLib::CountBitsSet(to_deal);

	// Create a deck out the cards to be dealt
	deck = new int[to_deal_total];

	// Fill it with cards
	j = 0;
	for(i = 0; i < 32; i++)
		if(to_deal & (1 << i))
			deck[j++] = i;

	for(i = 0; i < raTOTAL_CARDS; i++)
		sample_counts[i] = 0;

	// Calculate the number of cards to be dealt to each of the players
	for(i = 0; i < raTOTAL_PLAYERS; i++)
	{
		to_deal_count[i] = 8 - bhLib::CountBitsSet(re_data.played_cards[i]);
#ifdef raAI_LOG_GET_PLAY
		wxLogDebug(wxString::Format("%d cards to be dealt to %s (%d)", 
			to_deal_count[i], raLib::m_short_locs[i].c_str(), bhLib::CountBitsSet(re_data.played_cards[i])));
#endif
	}

	for(i = 0; i < raTOTAL_SUITS; i++)
	{
		if(m_trump_cands & (1 << i))
			wxLogDebug(wxString::Format("%s can be trump", raLib::m_suits[i].c_str()));
	}

	// Play a large number of samples
	for(k = 0; k < raAI_PLAY_SAMPLES; k++)
	{
#ifdef raAI_LOG_GET_PLAY
		wxLogDebug(wxString::Format("Sample no %d", k));
#endif
		::wxYield();
		// Shuffle the deck
		raLib::ShuffleArray(deck, (unsigned long)to_deal_total);
		memcpy(&work_data, &re_data, sizeof(raRuleEngineData));

		// Deal cards
		l = 0;
		for(j = 0; j < raTOTAL_PLAYERS; j++)
		{
			if(j != m_loc)
			{
				// This can be removed later
				work_data.hands[j] = 0;
				for(i = 0; i < to_deal_count[j]; i++)
					work_data.hands[j] |= 1 << deck[l++];
			}
		}
		//wxLogDebug(raLib::PrintHands(work_data.hands));

		rule_engine.SetData(&work_data, false);
#ifdef raAI_LOG_GET_PLAY
	wxLogDebug("Random hand dealt in GetPlay()");
		wxLogDebug(rule_engine.GetLoggable());
#endif


		best_eval = raAI_NEG_INFTY;
		best_move = raCARD_INVALID;

		for(i = 0; i < raTOTAL_CARDS; i++)
		{
			evals_ntrump[i].count = 0;
			evals_ntrump[i].eval = 0;
			evals_ntrump[i].valid = false;
		}
		evals_trump.count = 0;
		evals_trump.eval = 0;
		evals_trump.valid = false;

		// If
		// 1. The trump was set by the user, or
		// 2. The trump is shown,
		// TODO : Implement point 3
		// 3. There is only one possible value of trump(Not implemented)
		// then the trump is known
		if((max_bidder == m_loc) || is_trump_shown)
		{
#ifdef raAI_LOG_GET_PLAY
			wxLogDebug("Trump is known");
#endif
			rule_engine.SetData(&work_data, false);
			if(!GenerateMoves(&rule_engine, moves, &move_count, raAI_GENMV_NOTRUMP))
			{
				wxLogError(wxString::Format(wxT("Generate moves failed. %s:%d"),
					__FILE__, __LINE__));
				return -2;
			}
#ifdef raAI_LOG_GET_PLAY
			wxLogDebug(PrintMoves(moves, move_count));
#endif
			wxASSERT(move_count >= 0);

			for(i = 0; i < move_count; i++)
			{
				rule_engine.SetData(&work_data, false);

				if(!MakeMove(&rule_engine, &moves[i]))
				{
					wxLogError(wxString::Format(wxT("MakeMove failed. %s:%d"),
						__FILE__, __LINE__));
					wxLogError(rule_engine.GetLoggable());
					wxLogError(PrintMoves(moves, move_count));
					if(moves[i].ask_trump)
					{
						wxLogError(wxString::Format("Move attempted ?%s%s",
							raLib::m_suits[raGetSuit(moves[i].card)].c_str(),
							raLib::m_values[raGetValue(moves[i].card)].c_str()
							));
					}
					else
					{
						wxLogError(wxString::Format("Move attempted %s%s",
							raLib::m_suits[raGetSuit(moves[i].card)].c_str(),
							raLib::m_values[raGetValue(moves[i].card)].c_str()
							));
					}
					return -2;
				}
				eval_ret = false;
				eval = Evaluate(&rule_engine, raAI_NEG_INFTY, raAI_POS_INFTY, depth, &eval_ret);
				if(!eval_ret)
				{
					wxLogError(wxString::Format(wxT("Evaluate failed. %s:%d"),
						__FILE__, __LINE__));
					return -2;
				}

#ifdef raAI_LOG_GET_PLAY
				wxLogDebug(wxString::Format("Eval for %s%s - %d",
					raLib::m_suits[raGetSuit(moves[i].card)].c_str(),
					raLib::m_values[raGetValue(moves[i].card)].c_str(),
					eval
					));
#endif

				evals_ntrump[moves[i].card].eval += eval;
				evals_ntrump[moves[i].card].count++;
				evals_ntrump[moves[i].card].valid = true;
			}

			rule_engine.SetData(&work_data, false);
			if(!GenerateMoves(&rule_engine, moves, &move_count, raAI_GENMV_TRUMP))
			{
				wxLogError(wxString::Format(wxT("Generate moves failed. %s:%d"),
					__FILE__, __LINE__));
				return -2;
			}
			wxASSERT(move_count >= 0);
#ifdef raAI_LOG_GET_PLAY
			wxLogDebug(PrintMoves(moves, move_count));
#endif

			best_eval = raAI_NEG_INFTY;
			best_move = raCARD_INVALID;

			for(i = 0; i < move_count; i++)
			{
				rule_engine.SetData(&work_data, false);

				if(!MakeMove(&rule_engine, &moves[i]))
				{
					wxLogError(wxString::Format(wxT("MakeMove failed. %s:%d"),
						__FILE__, __LINE__));
					wxLogError(rule_engine.GetLoggable());
					wxLogError(PrintMoves(moves, move_count));
					if(moves[i].ask_trump)
					{
						wxLogError(wxString::Format("Move attempted ?%s%s",
							raLib::m_suits[raGetSuit(moves[i].card)].c_str(),
							raLib::m_values[raGetValue(moves[i].card)].c_str()
							));
					}
					else
					{
						wxLogError(wxString::Format("Move attempted %s%s",
							raLib::m_suits[raGetSuit(moves[i].card)].c_str(),
							raLib::m_values[raGetValue(moves[i].card)].c_str()
							));
					}
					return -2;
				}
				eval_ret = false;
				eval = Evaluate(&rule_engine, raAI_NEG_INFTY, raAI_POS_INFTY, depth, &eval_ret);
				if(!eval_ret)
				{
					wxLogError(wxString::Format(wxT("Evaluate failed. %s:%d"),
						__FILE__, __LINE__));
					return -2;
				}

#ifdef raAI_LOG_GET_PLAY
				wxLogDebug(wxString::Format("Eval for ?%s%s - %d",
					raLib::m_suits[raGetSuit(moves[i].card)].c_str(),
					raLib::m_values[raGetValue(moves[i].card)].c_str(),
					eval
					));
#endif

				if(eval > best_eval)
				{
					best_move = moves[i].card;
					best_eval = eval;
				}
			}

			// If a best move - asking for trump - was identified
			// add the eval
			evals_trump.eval += best_eval;
			evals_trump.count++;
			evals_trump.valid = true;
		}
		// If trump is not known, consider each possible suit
		// and work on the cumulative evaluated heuristic
		else
		{
#ifdef raAI_LOG_GET_PLAY
			wxLogDebug("Trump is not known");
#endif
			eval = 0;

			wxASSERT(m_trump_cands);
			// For each suit
			for(j = 0; j < raTOTAL_SUITS; j++)
			{
				if(!(m_trump_cands & (1 << j)))
					continue;
				// TODO : Remove this debug statement
				//wxLogDebug(wxString::Format("Considering suit %s as trump", raLib::m_suits[j].c_str()));
				// A suit needs to be considered only if the 
				// highest bidder has at least one card of it
				if(work_data.hands[max_bidder] & raLib::m_suit_mask[j])
				{
					memcpy(&bkp_data, &work_data, sizeof(raRuleEngineData));
					// TODO : This is not the optimal logic correct this
					bkp_data.trump_card =
						bhLib::HighestBitSet(bkp_data.hands[max_bidder] & raLib::m_suit_mask[j]);
					wxASSERT((bkp_data.trump_card >= 0) && (bkp_data.trump_card < raTOTAL_CARDS));
					wxASSERT(raGetSuit(bkp_data.trump_card) == j);
					bkp_data.hands[max_bidder] &= ~(1 << bkp_data.trump_card);
					bkp_data.trump_suit = j;

					rule_engine.SetData(&bkp_data, false);

					if(!GenerateMoves(&rule_engine, moves, &move_count, raAI_GENMV_NOTRUMP))
					{
						wxLogError(wxString::Format(wxT("Generate moves failed. %s:%d"),
							__FILE__, __LINE__));
						return -2;
					}
					wxASSERT(move_count >= 0);
#ifdef raAI_LOG_GET_PLAY
					wxLogDebug(wxString::Format("Trump - %s, ", raLib::m_suits[j].c_str()) + 
						PrintMoves(moves, move_count));
#endif


					for(i = 0; i < move_count; i++)
					{
						rule_engine.SetData(&bkp_data, false);

						if(!MakeMove(&rule_engine, &moves[i]))
						{
							wxLogError(wxString::Format(wxT("MakeMove failed. %s:%d"),
								__FILE__, __LINE__));
							wxLogError(wxString::Format("Loop trump is %s", 
								raLib::m_suits[j].c_str()
								));
							wxLogError(rule_engine.GetLoggable());
							wxLogError(PrintMoves(moves, move_count));
							if(moves[i].ask_trump)
							{
								wxLogError(wxString::Format("Move attempted ?%s%s",
									raLib::m_suits[raGetSuit(moves[i].card)].c_str(),
									raLib::m_values[raGetValue(moves[i].card)].c_str()
									));
							}
							else
							{
								wxLogError(wxString::Format("Move attempted %s%s",
									raLib::m_suits[raGetSuit(moves[i].card)].c_str(),
									raLib::m_values[raGetValue(moves[i].card)].c_str()
									));
							}
							return -2;
						}
						eval_ret = false;
						eval = Evaluate(&rule_engine, raAI_NEG_INFTY, raAI_POS_INFTY, depth, &eval_ret);
						if(!eval_ret)
						{
							wxLogError(wxString::Format(wxT("Evaluate failed. %s:%d"),
								__FILE__, __LINE__));
							return -2;
						}
						evals_ntrump[moves[i].card].eval += eval;
						evals_ntrump[moves[i].card].count++;
						evals_ntrump[moves[i].card].valid = true;

#ifdef raAI_LOG_GET_PLAY
						wxLogDebug(wxString::Format("Eval for %s%s - %d",
							raLib::m_suits[raGetSuit(moves[i].card)].c_str(),
							raLib::m_values[raGetValue(moves[i].card)].c_str(),
							eval
							));
#endif
					}

					// Reset the rule engine and generate
					// the moves which will ask for the trump

					rule_engine.SetData(&bkp_data, false);

					if(!GenerateMoves(&rule_engine, moves, &move_count, raAI_GENMV_TRUMP))
					{
						wxLogError(wxString::Format(wxT("Generate moves failed. %s:%d"),
							__FILE__, __LINE__));
						return -2;
					}
					wxASSERT(move_count >= 0);
#ifdef raAI_LOG_GET_PLAY
					wxLogDebug(PrintMoves(moves, move_count));
#endif


					best_eval = raAI_NEG_INFTY;
					best_move = raCARD_INVALID;

					for(i = 0; i < move_count; i++)
					{
						rule_engine.SetData(&bkp_data, false);

						if(!MakeMove(&rule_engine, &moves[i]))
						{
							wxLogError(wxString::Format(wxT("MakeMove failed. %s:%d"),
								__FILE__, __LINE__));
							wxLogError(wxString::Format("Loop trump is %s", 
								raLib::m_suits[j].c_str()
								));
							wxLogError(rule_engine.GetLoggable());
							wxLogError(PrintMoves(moves, move_count));
							if(moves[i].ask_trump)
							{
								wxLogError(wxString::Format("Move attempted ?%s%s",
									raLib::m_suits[raGetSuit(moves[i].card)].c_str(),
									raLib::m_values[raGetValue(moves[i].card)].c_str()
									));
							}
							else
							{
								wxLogError(wxString::Format("Move attempted %s%s",
									raLib::m_suits[raGetSuit(moves[i].card)].c_str(),
									raLib::m_values[raGetValue(moves[i].card)].c_str()
									));
							}
							return -2;
						}
						eval_ret = false;
						eval = Evaluate(&rule_engine, raAI_NEG_INFTY, raAI_POS_INFTY, depth, &eval_ret);
						if(!eval_ret)
						{
							wxLogError(wxString::Format(wxT("Evaluate failed. %s:%d"),
								__FILE__, __LINE__));
							return -2;
						}

						if(eval > best_eval)
						{
							best_move = moves[i].card;
							best_eval = eval;
						}

#ifdef raAI_LOG_GET_PLAY
						wxLogDebug(wxString::Format("Eval for ?%s%s - %d",
							raLib::m_suits[raGetSuit(moves[i].card)].c_str(),
							raLib::m_values[raGetValue(moves[i].card)].c_str(),
							eval
							));
#endif
					}
					// If a best move - asking for trump - was identified
					// add the eval
					evals_trump.eval += best_eval;
					evals_trump.count++;
					evals_trump.valid = true;
				}
			}
		}
		// Reset best eval and best move
		best_eval = raAI_NEG_INFTY;
		best_move = raCARD_INVALID;

		// Obtain the best move which does not ask for a trump

		for(i = 0; i < raTOTAL_CARDS; i++)
		{
			if(evals_ntrump[i].valid)
			{
				if((evals_ntrump[i].eval / evals_ntrump[i].count) > best_eval)
				{
					best_move = i;
					best_eval = evals_ntrump[i].eval / evals_ntrump[i].count;
				}
			}
		}

		// TODO : This is a known bug and hence commented out. Uncomment.
		//wxASSERT((best_move >= 0) && (best_move < raTOTAL_CARDS));
		if(!((best_move >= 0) && (best_move < raTOTAL_CARDS)))
		{
			wxLogError(wxString::Format(wxT("Invalid best move - Known bug. %s:%d"), 
				__FILE__, __LINE__));
		}

		// Compare the best move obtained with the 
		// the best move which asks for a trump
#ifdef raAI_LOG_GET_PLAY
		wxLogDebug(wxString::Format("Best eval is %d", best_eval));
#endif

		if(evals_trump.valid && ((evals_trump.eval / evals_trump.count) > best_eval))
		{
#ifdef raAI_LOG_GET_PLAY
		wxLogDebug(wxString::Format("Eval for ask trump(?) is %d", (evals_trump.eval / evals_trump.count)));
#endif
			sample_count_trump++;	
		}
		else
		{
			for(i = 0; i < raTOTAL_CARDS; i++)
			{
				if(evals_ntrump[i].valid)
				{
#ifdef raAI_LOG_GET_PLAY
					wxLogDebug(wxString::Format("Eval for %s%s - %d",
						raLib::m_suits[raGetSuit(i)].c_str(),
						raLib::m_values[raGetValue(i)].c_str(),
						(evals_ntrump[i].eval / evals_ntrump[i].count)	
						));
#endif
					if((evals_ntrump[i].eval / evals_ntrump[i].count) >= best_eval)
					{
#ifdef raAI_LOG_GET_PLAY
						wxLogDebug(wxString::Format("%s%s is a best move",
							raLib::m_suits[raGetSuit(i)].c_str(),
							raLib::m_values[raGetValue(i)].c_str(),
							(evals_ntrump[i].eval / evals_ntrump[i].count)	
							));
#endif
						sample_counts[i]++;
					}
				}
			}
		}

	}

	delete deck;

#ifdef raAI_LOG_GET_PLAY
	wxLogDebug("Sample counts!");
	for(i = 0; i < raTOTAL_CARDS; i++)
	{
		if(sample_counts[i] > 0)
		{
			wxLogDebug(wxString::Format("%s%s - %d",
				raLib::m_suits[raGetSuit(i)].c_str(),
				raLib::m_values[raGetValue(i)].c_str(),
				sample_counts[i]
				));
		}
	}
	wxLogDebug(wxString::Format("Ask trump - %d", sample_count_trump));
#endif

	best_move = -2;
	best_eval = -1;
	for(i = 0; i < raTOTAL_CARDS; i++)
	{
		if(sample_counts[i] > best_eval)	
		{
			best_move = i;
			best_eval = sample_counts[i];
		}
	}

	if(sample_count_trump > best_eval)
	{
		best_move = -1;
		best_eval = sample_count_trump;
	}

	wxASSERT(best_eval >= 0);
	wxASSERT(((best_move >= 0) && (best_move < raTOTAL_CARDS)) || (best_move == -1));

	/*if(moves[best_move].ask_trump)
	{
		
#ifdef raAI_LOG_GET_PLAY
		wxLogDebug("Best move is ask for trump");
#endif
		return -1;
	}
	else
	{
#ifdef raAI_LOG_GET_PLAY
		wxLogDebug(wxString::Format("Best move is %s%s", 
			raLib::m_suits[raGetSuit(moves[best_move].card)].c_str(),
			raLib::m_values[raGetValue(moves[best_move].card)].c_str()
			));
#endif
		return moves[best_move].card;
	}*/

	return best_move;
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
	wxASSERT(data->status == raSTATUS_TRICKS);
	// If trump is not shown and 
	// if the player is the max bidder and
	// if it is the first card in the trick
	// then the suit is not trump
	if(
		!data->trump_shown &&
		(data->in_trick_info.player == data->curr_max_bidder) &&
		(data->tricks[data->trick_round].count == 0)
		)
	{
		m_trump_cands &= ~(1 << raGetSuit(card));
	}
	wxASSERT(m_trump_cands);
	return true;
}

bool raAIAgent::Reset()
{
	m_engine.Reset();
	m_trump_cands = 0x0000000F;
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
