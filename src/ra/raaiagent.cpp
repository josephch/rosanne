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

int raAIAgent::GetPlayOld(unsigned long mask)
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

	GetPlay(0);

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
		if(m_trump_cards & (1 << i))
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

			wxASSERT(m_trump_cards);
			// For each suit
			for(j = 0; j < raTOTAL_SUITS; j++)
			{
				if(!(m_trump_cards & (1 << j)))
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
int raAIAgent::GetPlay(unsigned long mask)
{
	raRuleEngineData data;
	bool trump_known = false;
	slProblem problem;
	slSolution solution;
	unsigned long **deal_hands = NULL;
	int i;

	if(!m_engine.GetData(&data))
	{
		wxLogError(wxString::Format(wxT("GetData() failed. %s:%d"), 
			__FILE__, __LINE__));
		return -2;
	}

	wxASSERT((m_trump_cards >= 0) && (m_trump_cards <= 15));
	if((data.trump_shown) || (bhLib::CountBitsSet(m_trump_cards) == 1))
		trump_known = true;

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

	if(data.trump_shown)
	{
		GenerateDeals(&data, deal_hands, 30);
		//memset(&problem, 0, sizeof(problem));
		//GenerateSLProblem(&data, &problem);
		//GenerateSLSolution(&problem, &solution);
	}
	else
	{
		for(i = 0; i < raTOTAL_SUITS; i++)
		{
			// If the trump is possible
			if(m_trump_cards & (1 << i))
				GenerateDeals(&data, deal_hands, 30, i);
			//memset(&problem, 0, sizeof(problem));
			//GenerateSLProblem(&data, &problem, i);
			//GenerateSLSolution(&problem, &solution);
		}
	}

	// Free the memory allocated to hold the random deals
	for(i = 0; i < 30; i++)
	{
		delete[] deal_hands[i];
		deal_hands[i] = NULL;
	}
	delete [] deal_hands;
	deal_hands = NULL;

	return 0;
}
bool raAIAgent::GenerateSLProblem(raRuleEngineData *data, slProblem *problem, int trump)
{
	unsigned long cards_played = 0;
	int i, j;
	int sum_hands = 0, sum_suts = 0;

	wxLogDebug("Inside GenerateSLProblem");
	wxLogDebug(wxString::Format("m_loc - %s", raLib::m_short_locs[m_loc].c_str()));

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
			wxLogDebug("Suspect");
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

	return true;
}
bool raAIAgent::GenerateSLSolution(slProblem *problem, slSolution *solution)
{
	slSolver solver;

	wxASSERT(problem);
	wxASSERT(solution);

	solver.SetProblem(problem);
	solver.GetRandomSolution(solution);
	wxLogDebug(slSolver::PrintSolution(solution));

	return true;
}
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
		wxLogDebug(raLib::PrintHands(deals[i]));

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
					wxLogDebug(wxString::Format("%s is not the trump", raLib::m_suits[suit].c_str()));
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
	// For any suit if the sum of the cards played and cards held by self
	// is 8, then the suit is not the trump.
	// This is because max bidder cannot have any card of the suit
	if((m_loc != data->curr_max_bidder) && (!data->trump_shown))
	{
		cards_played = 0;
		for(i = 0; i < raTOTAL_PLAYERS; i++)	
		{
			cards_played |= data->played_cards[i];
		}

		for(i = 0; i < raTOTAL_SUITS; i++)	
		{
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

	wxLogDebug(wxString::Format("Checking assumptions for %s AI", raLib::m_short_locs[m_loc].c_str()));
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
				wxLogDebug(wxString::Format("%s does not have %s", 
					raLib::m_short_locs[j].c_str(),
					raLib::m_suits[i].c_str()));
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
