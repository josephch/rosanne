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

#include "ra/raruleengine.h"

//
// Constructor
//

raRuleEngine::raRuleEngine()
{
	m_data.ok = Reset(&m_data);
	m_data.feedback = true;

	// Set the rules
	m_data.rules.rot_addn = 1;
	m_data.rules.min_bid_1 = 14;
	m_data.rules.min_bid_2 = 20;
	m_data.rules.min_bid_3 = 23;
}

//
// Destructor
//

raRuleEngine::~raRuleEngine()
{
}

//
// Public Methods
//

bool raRuleEngine::IsOk()
{
	return m_data.ok;
}
int raRuleEngine::GetStatus()
{
	return m_data.status;
}
bool raRuleEngine::Reset(raRuleEngineData *data)
{
	int i, j;
	//TODO : Reduce the number of loops

	data->status = raSTATUS_NOT_STARTED; 
	data->dealer = 0;

	// Filling m_shuffled with values for all the 32 cards
	for(i = 0; i < raTOTAL_CARDS; i++)
		data->shuffled[i] = i;

	// Neither input or output is pending at the start
	data->input_pending = false;
	data->output_pending = false;


	// Reset the hands
	for(i = 0; i < raTOTAL_PLAYERS; i++)
		data->hands[i] = 0;

	// Resetting variables related to bidding
	data->first_bid = true;
	data->curr_max_bid = 0;
	data->curr_max_bidder = raPLAYER_INVALID;
	data->last_bidder = raPLAYER_INVALID;
	for(i = 0; i < raTOTAL_BID_ROUNDS; i++)
		for(j = 0; j < raTOTAL_PLAYERS; j++)
			data->bid_hist[i][j] = false;

	// Resetting trump suit and card
	data->trump_card = raCARD_INVALID;
	data->trump_suit = raSUIT_INVALID;

	// Resetting variables related to tricks
	for(i = 0; i < raTOTAL_TEAMS; i++)
		data->pts[i] = 0;

	for(i = 0; i < raTOTAL_PLAYERS; i++)
		data->played_cards[i] = 0;

	data->trick_round = 0;
	data->should_trump = false;
	data->should_play_trump_card = false;
	data->trump_shown = false;

	// TODO : Use ResetTrick
	for(i = 0; i < raTOTAL_TRICKS; i++)
	{
		for(j = 0; j < raTOTAL_PLAYERS; j++)
			data->tricks[i].cards[j] = raCARD_INVALID;
		data->tricks[i].count = 0;
		data->tricks[i].lead_loc = raPLAYER_INVALID;
		data->tricks[i].lead_suit = raSUIT_INVALID;
		data->tricks[i].points = 0;
		data->tricks[i].trumped = false;
		data->tricks[i].winner = raPLAYER_INVALID;
	}

	return true;

}
bool raRuleEngine::Reset()
{
	return Reset(&m_data);
}
bool raRuleEngine::Shuffle()
{
	int i, j;
	int t;

	// If required set the shuffled card as per the
	// deal read from the test data input file
#ifdef raREAD_DEAL_FROM_FILE 

	if(::wxFileExists(raTEST_DATA_FILE))
	{
		wxString str_cards_read;
		wxString text_round;
		wxString key;
		wxString str_card;
		int idx;
		wxFFileInputStream in(raTEST_DATA_FILE);
		wxFileConfig fcfg(in);
		int k, l;
		unsigned long cards_read[2][raTOTAL_PLAYERS];
		unsigned long all_read = 0;
		unsigned long temp;
		int count_read = 0;

		wxLogDebug("----------------------------------------------");
		wxLogDebug("Attempting to read deal information from file");

		for(i = 0; i < 2; i++)
			for(j = 0; j < raTOTAL_PLAYERS; j++)
				cards_read[i][j] = 0;

		for(k = 1; k <= 2; k++)
		{
			text_round = wxString::Format("%s%d", raTEXT_DEAL_ROUND, k);
			if(!fcfg.Exists(text_round))
				continue;

			for(i = 0; i < raTOTAL_PLAYERS; i++)
			{
				key = wxString::Format("%s/%s", text_round, raLib::m_short_locs[i].c_str());
				if(fcfg.Exists(key))
				{
					if(!fcfg.Read(key, &str_cards_read))
					{
						wxLogError(wxString::Format(
							wxT("Read failed. %s:%d"), __FILE__, __LINE__));

					}
					else
					{
						wxLogDebug(wxString::Format(
							"Cards to be dealt to %s are %s", 
							raLib::m_short_locs[i].c_str(), str_cards_read.c_str()));

						// Get each str_card from the list of str_cards to be 
						// dealt to the location
						while(!str_cards_read.IsEmpty())
						{
							j = str_cards_read.Find(',');
							str_card = str_cards_read.Left(j);
							str_card.UpperCase();
							str_card.Trim();
							str_card.Trim(false);
							wxLogDebug(wxString::Format("Card %s", str_card.c_str()));
							idx = raLib::GetCardIndex(str_card);
							if(idx == -1)
							{
								wxLogDebug(wxString::Format(
									wxT("GetCardIndex failed. %s:%d"),
									__FILE__, __LINE__));
								break;
							}
							else
							{
								cards_read[k - 1][i] |= (1 << idx);
								count_read++;
							}

							if(j == -1)
								break;
							str_cards_read = str_cards_read.Mid(j + 1);
						}
						wxASSERT(bhLib::CountBitsSet(cards_read[k - 1][i]) <= 4);
					}
				}
			}
			wxLogDebug(wxString::Format("For round %d", k));
			// Print the cards to be dealt for each player
			for(i = 0; i < raTOTAL_PLAYERS; i++)
			{
				wxLogDebug(wxString::Format("Cards for %s - %s", 
					raLib::m_short_locs[i].c_str(),
					raLib::PrintLong(cards_read[k - 1][i]).c_str()));
			}
		}
		/*else
		{
		wxLogDebug(wxString::Format(
		wxT("Cards to be dealt for round 1 not read from %s. %s:%d"),
		raTEST_DATA_FILE, __FILE__, __LINE__));
		}*/
		for(i = 0; i < 2; i++)
			for(j = 0; j < raTOTAL_PLAYERS; j++)
				all_read |= cards_read[i][j];
		if(bhLib::CountBitsSet(all_read) != count_read)
		{
			wxLogError(wxString::Format(
				"Duplicate cards. From all read %d count read %d", 
				bhLib::CountBitsSet(all_read), count_read));
		}
		else
		{
			int * unassigned = new int[raTOTAL_CARDS - count_read];
			j = 0;
			for(i = 0; i < raTOTAL_CARDS; i++)
			{
				if(!(all_read & (1 << i)))
					unassigned[j++] = i;
			}
			wxASSERT(j == (raTOTAL_CARDS - count_read));
			raLib::ShuffleArray(unassigned, (raTOTAL_CARDS - count_read));

			bool flags[raTOTAL_CARDS];
			for(i = 0; i < raTOTAL_CARDS; i++)
			{
				flags[i] = false;
			}
			for(k = 0; k < 2; k++)
			{
				for(i = 0; i < raTOTAL_PLAYERS; i++)
				{
					j = 0;
					temp = cards_read[k][i];
					while(temp)
					{
						l = (k * (raTOTAL_CARDS / 2)) + (i * (raTOTAL_CARDS / (raTOTAL_PLAYERS * 2))) +  j;
						j++;
						m_data.shuffled[l] = bhLib::HighestBitSet(temp);
						flags[l] = true;
						temp &= ~(1 << m_data.shuffled[l]);
						//temp &= ~(1 << bhLib::HighestBitSet(temp));
					}
					wxASSERT(j == (bhLib::CountBitsSet(cards_read[k][i])));
				}
			}
			j = 0;
			for(i = 0; i < raTOTAL_CARDS; i++)
			{
				if(!flags[i])
				{
					m_data.shuffled[i] = unassigned[j++];
					flags[i] = true;
				}
			}
			for(i = 0; i < raTOTAL_CARDS; i++)
			{
				wxASSERT(flags[i]);
				//wxLogDebug(wxString::Format("%d - %s%s", 
				//	i,
				//	raLib::m_suits[raGetSuit(m_data.shuffled[i])],
				//	raLib::m_values[raGetValue(m_data.shuffled[i])]
				//	));
			}

			delete unassigned;
			wxLogDebug("-------------Succes---------------------------");
			return true;
		}
		wxLogDebug("----------------------------------------------");
	}	
#endif

	// Sorting, for full replication
	// can be removed, functionality will not be affected.
	for (i = 0; i < raTOTAL_CARDS; i++) 
		m_data.shuffled[i] = i;

	for (i = 0; i < raTOTAL_CARDS - 1; i++) {
		j = i + rand() / (RAND_MAX / (raTOTAL_CARDS - i) + 1);
		t = m_data.shuffled[j];
		m_data.shuffled[j] = m_data.shuffled[i];
		m_data.shuffled[i] = t;
	}

	return true;
}
bool raRuleEngine::Continue()
{
	int i;
	unsigned long u;
	unsigned long rules;

	// Cannot continue if input is pending
	if(m_data.input_pending)	
		return false;

	// Cannot continue if output is pending
	if(m_data.output_pending)
		return false;

	switch(m_data.status)
	{

	case raSTATUS_NOT_STARTED:
		if(m_data.feedback)
			SetOutput(raOUTPUT_STARTED);
		m_data.status++;
		return false;
		break;

	case raSTATUS_DEAL1:
		// Shuffle the cards before dealing round 1
		if(!Shuffle())
		{
			wxLogDebug(wxString::Format(
				"Unexpected error while shuffling cards. File - %s Line - %d",
				__FILE__, __LINE__));
			return false;
		}

		// Deal round 1
		for(i = 0; i < raTOTAL_CARDS / 2; i++)
			m_data.hands[i / 4] |= (1 << m_data.shuffled[i]);

		if(m_data.feedback)
		{
			m_data.out_deal_info.round = raDEAL_ROUND_1;
			//TODO : Is this memcpy correct?
			memcpy(&m_data.out_deal_info.hands, &m_data.hands, 
				sizeof(m_data.hands));

			// Set output pending
			SetOutput(raOUTPUT_DEAL);
		}
		m_data.status++;
		return false;
		break;

	case raSTATUS_BID1:
		// If no bids have been made yet, start with the player next to dealer
		if(m_data.last_bidder == raPLAYER_INVALID)
			i = raNext(m_data.dealer); 
		// Otherwise, start with the player next to the last bidder
		else
			i = raNext(m_data.last_bidder); 

		// u will hold the list of players for which 
		// it was checked as to whether the player can bid
		u = 0;
		while(u < 0x0000000F)
		{
			// If the player has not already bid
			// and the highest bid is not held by his partner
			// then a bid is expected.
			if(!m_data.bid_hist[0][i] && (raPartner(i) 
				!= m_data.curr_max_bidder))
			{	
				// Fill data in the input bid structure
				m_data.in_bid_info.player = i;
				m_data.in_bid_info.bid = 0;

				if(m_data.last_bidder == raPLAYER_INVALID)
					m_data.in_bid_info.passable = false;  
				else
					m_data.in_bid_info.passable = true;

				// If no bid has been made yet, the minimum 
				// that can be bid is the minimum for the first round.
				// Otherwise the minimum bid that can be made
				// is any bid greater than the highest bid made so far.
				if(m_data.curr_max_bid == 0)
					m_data.in_bid_info.min = m_data.rules.min_bid_1;
				else
					m_data.in_bid_info.min = m_data.curr_max_bid + 1;

				m_data.in_bid_info.round = 0;

				SetInput(raINPUT_BID);

				return false;
			}
			u |= (1 << i);
			i = raNext(i);
		}
		// Before moving to round 2 of the auction,
		// reset values
		m_data.last_bidder = raPLAYER_INVALID;

		m_data.status++;
		return false;
		break;

	case raSTATUS_BID2:
		// If no bids have been made yet in this round,
		// start with the player next to dealer
		if(m_data.last_bidder == raPLAYER_INVALID)
			i = raNext(m_data.dealer); 
		// Otherwise, start with the player next to the last bidder
		else
			i = raNext(m_data.last_bidder);

		// u will hold the list of players for which 
		// it was checked as to whether the player can bid
		u = 0;
		while(u < 0x0000000F)
		{
			// If the player has not already bid
			// and the highest bid is not by the player
			// then a bid is expected.
			if(!m_data.bid_hist[1][i] && (i != m_data.curr_max_bidder))
			{	
				// Fill data in the input bid structure
				m_data.in_bid_info.player = i;
				m_data.in_bid_info.bid = 0;

				m_data.in_bid_info.passable = true;  

				// The minimum bid that can be made is the 
				// maximum of the current highest bid and the minimum allowed
				// bid for the round.
				m_data.in_bid_info.min = 
					raMax(m_data.rules.min_bid_2, (m_data.curr_max_bid + 1));
				m_data.in_bid_info.round = 1;

				SetInput(raINPUT_BID);

				return false;
			}
			u |= (1 << i);
			i = raNext(i);
		}
		// Before moving to round 3 of the auction,
		// reset values
		m_data.last_bidder = raPLAYER_INVALID;

		m_data.status++;
		return false;
		break;

	case raSTATUS_BID3:
		// If no bids have been made yet in this round,
		// start with the player next to dealer
		if(m_data.last_bidder == raPLAYER_INVALID)
			i = raNext(m_data.dealer); 
		// Otherwise, start with the player next to the last bidder
		else
			i = raNext(m_data.last_bidder);

		// u will hold the list of players for which 
		// it was checked as to whether the player can bid
		u = 0;
		while(u < 0x0000000F)
		{
			// If the player has not already bid
			// then a bid is expected.
			if(!m_data.bid_hist[2][i])
			{	
				// Fill data in the input bid structure
				m_data.in_bid_info.player = i;
				m_data.in_bid_info.bid = 0;

				m_data.in_bid_info.passable = true;  

				// The minimum bid that can be made is the 
				// maximum of the current highest bid and the minimum allowed
				// bid for the round.
				m_data.in_bid_info.min = 
					raMax(m_data.rules.min_bid_3, (m_data.curr_max_bid + 1));
				m_data.in_bid_info.round = 2;

				SetInput(raINPUT_BID);

				return false;
			}
			u |= (1 << i);
			i = raNext(i);
		}
		// Before moving to round 3 of the auction,
		// reset values
		m_data.last_bidder = raPLAYER_INVALID;

		m_data.status++;
		return false;
		break;

	case raSTATUS_TRUMPSEL1:
	case raSTATUS_TRUMPSEL2:
		if(m_data.trump_card == raCARD_INVALID)
		{
			// Fill data in the input trumpsel info structure
			m_data.in_trumpsel_info.card = raCARD_INVALID;
			m_data.in_trumpsel_info.player = m_data.curr_max_bidder;
			SetInput(raINPUT_TRUMPSEL);
		}

		// Set the player to lead the first trick
		m_data.tricks[0].lead_loc = raNext(m_data.dealer);

		m_data.status++;
		break;

	case raSTATUS_DEAL2:
		// Deal round 1
		for(i = 0; i < raTOTAL_CARDS / 2; i++)
			m_data.hands[i / 4] |= 
				(1 << m_data.shuffled[i + (raTOTAL_CARDS / 2)]);

		if(m_data.feedback)
		{
			m_data.out_deal_info.round = raDEAL_ROUND_2;
			// TODO : Is this memcpy correct?
			memcpy(&m_data.out_deal_info.hands, 
				&m_data.hands, sizeof(m_data.hands));

			// Set output pending
			SetOutput(raOUTPUT_DEAL);
		}
		m_data.status++;
		return false;
		break;
	case raSTATUS_TRICKS:
		if(m_data.trick_round < raTOTAL_TRICKS)
		{

			m_data.in_trick_info.ask_trump = false;
			// Player can ask for trump to be shown if,
			// 1. Trump is not shown
			// 2. The trick has a valid lead suit
			// 3. Player does not have any lead suit
			m_data.in_trick_info.can_ask_trump = false;
			if(
				!m_data.trump_shown && 
				(m_data.tricks[m_data.trick_round].lead_suit != raSUIT_INVALID)
				)
			{
				if(
					!(m_data.hands[raTrickNext] & 
					raLib::m_suit_mask[m_data.tricks[m_data.trick_round].lead_suit])
					)
				{
					m_data.in_trick_info.can_ask_trump = true;
				}
			}

			// Player can also ask for trump if
			// 1. He is the max bidder
			// 2. And trump is not shown
			// 3. and he does not have any card in his hand

			if(
				(raTrickNext == m_data.curr_max_bidder) &&
				!m_data.trump_shown &&
				!m_data.hands[raTrickNext]
				)
				m_data.in_trick_info.can_ask_trump = true;

			m_data.in_trick_info.card = raCARD_INVALID;
			
			// Obtain the mask and the rules applicable 
			// and set the same
			rules = 0;
			m_data.in_trick_info.mask = GenerateMask(&rules);
			m_data.in_trick_info.rules = rules;
			m_data.in_trick_info.player = raTrickNext;

			// If the player has no card which matches 
			// the mask, player can play any card
			if(!(m_data.hands[m_data.in_trick_info.player] & m_data.in_trick_info.mask))
				m_data.in_trick_info.mask = 0xFFFFFFFF;

			SetInput(raINPUT_TRICK);
			return false;
		}
		m_data.status++;
		break;
	case raSTATUS_FINISHED:
		SetDealEndOutput();
		return true;
		break;
	}

	return false;
}


bool raRuleEngine::GetOutput(int *output_type, void *output)
{
	// If output is not pending, return false
	if(!m_data.output_pending)
		return false;

	if(output_type)
		*output_type = m_data.output_type;
	switch(m_data.output_type)
	{
	case raOUTPUT_STARTED:
		break;
	case raOUTPUT_DEAL:
		memcpy(output, &m_data.out_deal_info, sizeof(raOutputDealInfo));
		break;
	case raOUTPUT_DEAL_END:
		memcpy(output, &m_data.out_deal_end_info, sizeof(raOutputDealEndInfo));
		break;
	default:
		wxLogDebug(wxString::Format("Inside default in switch. File - %s Line - %d", __FILE__, __LINE__));
		break;
	}

	m_data.output_pending = false;
	return true;
}
bool raRuleEngine::IsOutputPending()
{
	return m_data.output_pending;
}
int raRuleEngine::GetPendingOutputType()
{
	// If no output is pending,
	// return invalid type
	if(!m_data.output_pending)
		return raOUTPUT_INVALID;

	return m_data.output_type;
}

bool raRuleEngine::IsInputPending()
{
	return m_data.input_pending;
}
int raRuleEngine::GetPendingInputType()
{
	// If no input is pending,
	// return invalid type
	if(!m_data.input_pending)
		return raINPUT_INVALID;

	return m_data.input_type;
}

bool raRuleEngine::GetPendingInputCriteria(int *input_type, void *input)
{
	// If input is not pending, return false
	if(!m_data.input_pending)
		return false;

	if(input_type)
		*input_type = m_data.input_type;

	switch(m_data.input_type)
	{
	case raINPUT_BID:
		memcpy(input, &m_data.in_bid_info, sizeof(raInputBidInfo));
		break;
	case raINPUT_TRUMPSEL:
		memcpy(input, &m_data.in_trumpsel_info, sizeof(raInputTrumpselInfo));
		break;
	case raINPUT_TRICK:
		memcpy(input, &m_data.in_trick_info, sizeof(raInputTrickInfo));
		break;
	default:
		wxLogDebug(wxString::Format("Inside default in switch. File - %s Line - %d", __FILE__, __LINE__));
		break;
	}

	return true;
}
int raRuleEngine::PostInputMessage(int input_type, void *input)
{
	raInputBidInfo *in_bid_info, *exist_bid_info;
	raInputTrumpselInfo *in_trumpsel_info, *exist_trumpsel_info;
	raInputTrickInfo *in_trick_info, *exist_trick_info;
	// Check whether input is pending
	if(!m_data.input_pending)
		return false;

	// Check whether the input types is correct
	if(input_type != m_data.input_type)
		return false;

	switch(m_data.input_type)
	{
	case raINPUT_BID:
		in_bid_info = (raInputBidInfo *)input;
		exist_bid_info = &m_data.in_bid_info;

		// Check whether the player making the bid is correct
		if(exist_bid_info->player != in_bid_info->player)
			return raERR_BID_BY_WRONG_PLAYER;

		// If passed, check whether the bid is indeed passable
		if(in_bid_info->bid == raBID_PASS)
		{
			if(!exist_bid_info->passable)
				return raERR_CANNOT_PASS;

			// If pass is a valid bid, accept the bid
			m_data.last_bidder = exist_bid_info->player;
			m_data.bid_hist[exist_bid_info->round][exist_bid_info->player] = true;
			break;
		}

		// Check whether the bid is less than the minimum allowed
		if(in_bid_info->bid < exist_bid_info->min)
			return raERR_BID_LESS_THAN_MIN;

		// If there is an existing trump, invalidate the same
		// and add the card back to the max bidder
		if(m_data.trump_card != raCARD_INVALID)
		{
			m_data.hands[m_data.curr_max_bidder] |= (1 << m_data.trump_card);
			m_data.trump_card = raCARD_INVALID;
			m_data.trump_suit = raSUIT_INVALID;
		}

		// If the bid is valid, accept the same
		m_data.first_bid = false;
		m_data.curr_max_bid = in_bid_info->bid;
		m_data.curr_max_bidder = exist_bid_info->player;
		m_data.last_bidder = exist_bid_info->player;
		m_data.bid_hist[exist_bid_info->round][exist_bid_info->player] = true;


		break;
	case raINPUT_TRUMPSEL:
		in_trumpsel_info = (raInputTrumpselInfo *)input;
		exist_trumpsel_info = &m_data.in_trumpsel_info;

		// Verify that the player is correct
		if(in_trumpsel_info->player != exist_trumpsel_info->player)
			return raERR_TRUMPSEL_BY_WRONG_PLAYER;

		// Verify that the trump card has a valid value
		wxASSERT(in_trumpsel_info->card >= 0);
		wxASSERT(in_trumpsel_info->card < raTOTAL_CARDS);

		// Verify that the card exists in the highest bidders hand
		if(!(m_data.hands[exist_trumpsel_info->player] & (1 << in_trumpsel_info->card)))
			return raERR_TRUMPSEL_NONEXIST_CARD;

		// Set the trump card
		m_data.trump_suit = in_trumpsel_info->card / raTOTAL_VALUES;
		m_data.trump_card = in_trumpsel_info->card;

		// TODO : Do this earlier, when the bid is made
		// Remove the trump card from the highest bidders hand
		m_data.hands[exist_trumpsel_info->player] &= ~(1 << m_data.trump_card);

		// As soon as a trump is selected, the status can be incremented
		//m_data.status++;

		break;
	case raINPUT_TRICK:
		in_trick_info = (raInputTrickInfo *)input;
		exist_trick_info = &m_data.in_trick_info;

		// Verify that the player is correct
		if(in_trick_info->player != exist_trick_info->player)
			return raERR_TRICK_BY_WRONG_PLAYER;

		// Is the player asking for trump to be shown?
		if(in_trick_info->ask_trump)
		{
			if(!exist_trick_info->can_ask_trump)
				return raERR_TRICK_INVALID_TRUMP_REQ;

			m_data.trump_shown = true;
			m_data.should_trump = true;

			// Add the trump card to max bidder's hand
			m_data.hands[m_data.curr_max_bidder] |= (1 << m_data.trump_card);

			if(exist_trick_info->player == m_data.curr_max_bidder)
			{
				m_data.should_play_trump_card = true;
				m_data.in_trick_info.mask = (1 << m_data.trump_card);
			}
			else
			{
				// Check if the player who asked for trump has 
				// at least a single trump. If that is the case, 
				// then he should play trump. Set mask accordingly
				if(m_data.hands[m_data.in_trick_info.player] & raLib::m_suit_mask[m_data.trump_suit])
				{
					m_data.in_trick_info.mask &= raLib::m_suit_mask[m_data.trump_suit];
				}
			}

			break;
		}

		// If the player has played a card
		wxASSERT(in_trick_info->card >= 0);
		wxASSERT(in_trick_info->card < raTOTAL_CARDS);

		// If the player has at least one card which matches the mask
		// then one such card should be played
		if(
			(m_data.hands[exist_trick_info->player] & exist_trick_info->mask) &&
			!(exist_trick_info->mask & (1 << in_trick_info->card))
			)
			return raERR_TRICK_MASK_MISMATCH;

		// Check whether the card played actually exists in the players hand
		if(!(m_data.hands[exist_trick_info->player] & (1 << in_trick_info->card)))
			return raERR_TRICK_CARD_NOT_IN_HAND;

		// If the first card to be played in the round
		if(!m_data.tricks[m_data.trick_round].count)
		{
			m_data.tricks[m_data.trick_round].lead_loc = exist_trick_info->player;
			m_data.tricks[m_data.trick_round].lead_suit = raGetSuit(in_trick_info->card);
			m_data.tricks[m_data.trick_round].winner = exist_trick_info->player;

			// If the trump has been shown and the first card 
			// to be played is a trump then the trick is already trumped
			if(m_data.trump_shown && (raGetSuit(in_trick_info->card) == m_data.trump_suit))
				m_data.tricks[m_data.trick_round].trumped = true;
		}
		else
		{

			// If the card played is a trump
			if(raGetSuit(in_trick_info->card) == m_data.trump_suit)
			{
				// and if the trick is already trumped
				if(m_data.tricks[m_data.trick_round].trumped)
				{
					// check for over trumping
					if(raGetValue(in_trick_info->card) > raGetValue(raWinnerCard))
					{
						m_data.tricks[m_data.trick_round].winner = exist_trick_info->player;
					}
				}
				// If the trick is not trumped yet
				// and the trump has been shown, 
				else if(m_data.trump_shown)
				{
					// The trick is being trumped
					m_data.tricks[m_data.trick_round].trumped = true;

					// If the lead suit is trump
					// then check whether we have a new winner
					if(m_data.tricks[m_data.trick_round].lead_suit == m_data.trump_suit)
					{
						if(raGetValue(in_trick_info->card) > raGetValue(raWinnerCard))
						{
							m_data.tricks[m_data.trick_round].winner = exist_trick_info->player;
						}
					}
					// If the lead suit is not trump
					// then we have a new winner
					else
					{
						m_data.tricks[m_data.trick_round].winner = exist_trick_info->player;
					}
				}
				// If the trick is not trumped yet
				// and if the trump has not been shown 
				// but if trump is the lead suit :)
				// check if we have a new trump
				else if(
					(m_data.trump_suit == m_data.tricks[m_data.trick_round].lead_suit) &&
					(raGetValue(in_trick_info->card) > raGetValue(raWinnerCard))
					)
				{
					m_data.tricks[m_data.trick_round].winner = exist_trick_info->player;
				}
			}
			// If the card being played is not a trump
			else
			{
				// If the trick is not trumped already,
				// and if the lead suit has been followed,
				// check whether we have a new winner
				if(
					(raGetSuit(in_trick_info->card) == m_data.tricks[m_data.trick_round].lead_suit) &&
					(raGetValue(in_trick_info->card) > raGetValue(raWinnerCard)) &&
					!m_data.tricks[m_data.trick_round].trumped
					)
				{
					m_data.tricks[m_data.trick_round].winner = exist_trick_info->player;
				}
			}
		}

		// Common changes in all cases

		// Set the card
		m_data.tricks[m_data.trick_round].cards[exist_trick_info->player] = in_trick_info->card;
		// Remove the card from the hand of the player who played it
		m_data.hands[exist_trick_info->player] &= ~(1 << in_trick_info->card);
		// Add the card to the list of cards played by the player
		m_data.played_cards[exist_trick_info->player] |= (1 << in_trick_info->card);
		// Increment the number of cards played in this round
		m_data.tricks[m_data.trick_round].count++;
		// Increment the total points
		m_data.tricks[m_data.trick_round].points += raLib::m_points[raGetValue(in_trick_info->card)];

		m_data.should_play_trump_card = false;
		m_data.should_trump = false;

		// To Remove
		//wxLogDebug(wxString::Format("%s played card %s%s", 
		//	raLib::m_long_locs[in_trick_info->player].c_str(), 
		//	raLib::m_suits[raGetSuit(in_trick_info->card)].c_str(), 
		//	raLib::m_values[raGetValue(in_trick_info->card)].c_str()));

		// If all 4 cards have been played, move to the next round
		if(m_data.tricks[m_data.trick_round].count == raTOTAL_PLAYERS)
		{
			m_data.trick_round++;
			m_data.tricks[m_data.trick_round].lead_loc = m_data.tricks[m_data.trick_round - 1].winner;
			// To Remove
			//wxLogDebug(wxString::Format("Trick %d completed. Winner - %s.", 
			//	m_data.trick_round, raLib::m_long_locs[m_data.tricks[m_data.trick_round - 1].winner].c_str() ));

			// Add points to the total of the winning team
			m_data.pts[m_data.tricks[m_data.trick_round - 1].winner % raTOTAL_TEAMS] += m_data.tricks[m_data.trick_round - 1].points;

			//TODO : For the trick ended, post and output message
			//out_trick_info = new raOutputTrickInfo;
			//memcpy(out_trick_info->points, m_data.pts, sizeof(m_data.pts));
			//memcpy(&out_trick_info->trick, &m_data.tricks[m_data.trick_round - 1], sizeof(raTrick));
		}

		break;
	}

	m_data.input_pending = false;

	return 0;
}

bool raRuleEngine::GetFeedback()
{
	return m_data.feedback;
}
void raRuleEngine::SetFeedback(bool feedback)
{
	m_data.feedback = feedback;
	if(!m_data.feedback)
		m_data.output_pending = false;
}

void raRuleEngine::GetRules(raRules *rules)
{
	memcpy(rules, &m_data.rules, sizeof(raRules));
}
void raRuleEngine::SetRules(raRules *rules)
{
	memcpy(&m_data.rules, rules, sizeof(raRules));
}
void raRuleEngine::GetHands(unsigned long *hands)
{
	memcpy(hands, m_data.hands, sizeof(m_data.hands));
}
void raRuleEngine::GetCardsPlayed(unsigned long *cards)
{
	memcpy(cards, m_data.played_cards, sizeof(m_data.played_cards));
}
void raRuleEngine::GetTrick(int trick_round, raTrick *trick)
{
	wxASSERT((trick_round >= 0) && (trick_round < raTOTAL_TRICKS));
	memcpy(trick, &m_data.tricks[trick_round], sizeof(raTrick));
}
void raRuleEngine::GetTrick(raTrick *trick) 
{
	GetTrick(m_data.trick_round, trick);
}

int raRuleEngine::GetTrickRound()
{
	return m_data.trick_round;
}

int raRuleEngine::GetPoints(int team)
{
	wxASSERT((team >= 0) && (team < raTOTAL_TEAMS));
	return m_data.pts[team];
}
void raRuleEngine::GetPoints(int *pts)
{
	wxASSERT(pts);
	memcpy(pts, m_data.pts, sizeof(m_data.pts));
}

int raRuleEngine::GetTrump()
{
	return m_data.trump_suit;
}
int raRuleEngine::GetTrumpCard()
{
	return m_data.trump_card;
}
int raRuleEngine::GetDealer()
{
	wxASSERT((m_data.dealer >= 0) && (m_data.dealer < raTOTAL_PLAYERS));
	return m_data.dealer;
}
void raRuleEngine::SetDealer(int dealer)
{
#ifdef raREAD_DEALER_FROM_FILE
	long dealer_read;
#endif

	wxASSERT((dealer >= 0) && (dealer < raTOTAL_PLAYERS));
	m_data.dealer = dealer;

	// If neeeded, for testing purposes, read the dealer location
	// from the testing data file
#ifdef raREAD_DEALER_FROM_FILE
	if(::wxFileExists(raTEST_DATA_FILE))
	{
		wxFFileInputStream in(raTEST_DATA_FILE);
		wxFileConfig fcfg(in);
		if(fcfg.Exists(raTEXT_DEALER))
		{
			wxLogDebug(wxString::Format(
				wxT("Reading dealer from %s. %s:%d"),
				raTEST_DATA_FILE, __FILE__, __LINE__));

			dealer_read = -1;
			if(!fcfg.Read(raTEXT_DEALER, &dealer_read))
			{
				wxLogError(wxString::Format(
					wxT("Read failed. %s:%d"), __FILE__, __LINE__));

			}
			else
			{
				m_data.dealer = (int)dealer_read;
				wxASSERT((m_data.dealer >= 0) && (m_data.dealer < raTOTAL_PLAYERS));
			}
		}
		else
		{
			wxLogError(wxString::Format(
				wxT("Could not find dealer in %s. %s:%d"),
				raTEST_DATA_FILE, __FILE__, __LINE__));
		}
	}
#endif

}

void raRuleEngine::ResetTrick(raTrick *trick)
{
	int j;
	for(j = 0; j < raTOTAL_PLAYERS; j++)
		trick->cards[j] = raCARD_INVALID;
	trick->count = 0;
	trick->lead_loc = raPLAYER_INVALID;
	trick->lead_suit = raSUIT_INVALID;
	trick->points = 0;
	trick->trumped = false;
	trick->winner = raPLAYER_INVALID;
}
bool raRuleEngine::GetData(raRuleEngineData *data)
{
	memcpy(data, &m_data, sizeof(raRuleEngineData));
	return true;
}
bool raRuleEngine::SetData(raRuleEngineData *data, bool check)
{
	// TODO : Add error checks and remove wxASSERT
	wxASSERT(!check);
	memcpy(&m_data, data, sizeof(raRuleEngineData));
	return true;
}

bool raRuleEngine::GetMaxBid(int *bid, int *loc)
{
	if(bid)
		*bid = m_data.curr_max_bid;
	if(loc)
		*loc = m_data.curr_max_bidder;
	return true;
}
wxString raRuleEngine::GetLoggable()
{
	wxString out;//, temp;
	int i, j;
	out.Append("\n");
	if(m_data.trump_shown)
	{
		out.Append(wxString::Format("Trump - %s(%s)\n", 
			raLib::m_suits[raGetSuit(m_data.trump_card)].c_str(),
			raLib::m_values[raGetValue(m_data.trump_card)].c_str()
			));
	}
	for(i = 0; i < m_data.trick_round; i++)
	{
		out.Append(wxString::Format("Trick %d - ", i));

		for(j = 0; j < raTOTAL_PLAYERS; j++)
		{

		if(m_data.tricks[i].lead_loc == j)
			out.Append("+");
		if(m_data.tricks[i].winner == j)
			out.Append("*");
		out.Append(wxString::Format("%s%s ", 
			raLib::m_suits[raGetSuit(m_data.tricks[i].cards[j])].c_str(),
			raLib::m_values[raGetValue(m_data.tricks[i].cards[j])].c_str()
			));
		}
		out.Append("\n");
	}
	out.Append("\n");
	i = m_data.trick_round;
	for(j = 0; j < raTOTAL_PLAYERS; j++)
	{
		if(m_data.tricks[i].cards[j] == raCARD_INVALID)
			continue;
		out.Append(wxString::Format("%s - ", raLib::m_short_locs[j].c_str()));
		if(m_data.tricks[i].lead_loc == j)
			out.Append("+");
		if(m_data.tricks[i].winner == j)
			out.Append("*");
		out.Append(wxString::Format("%s%s ", 
			raLib::m_suits[raGetSuit(m_data.tricks[i].cards[j])].c_str(),
			raLib::m_values[raGetValue(m_data.tricks[i].cards[j])].c_str()
			));
		out.Append("\n");
	}
	out.Append("\n");
	out.Append(raLib::PrintHands(m_data.hands));
	return out;
}
bool raRuleEngine::IsTrumpShown()
{
	return m_data.trump_shown;
}
int raRuleEngine::GetTrickNextToPlay()
{
	if(m_data.status != raSTATUS_TRICKS)
		return raPLAYER_INVALID;
	if(m_data.tricks[m_data.trick_round].count == raTOTAL_PLAYERS)
		return raPLAYER_INVALID;

	return raTrickNext;
}


//
// Private methods
//

void raRuleEngine::SetOutput(int output_type)
{
	wxASSERT(!m_data.output_pending);
	wxASSERT(!m_data.input_pending);
	m_data.output_type = output_type;
	m_data.output_pending = true;
}
void raRuleEngine::SetInput(int input_type)
{
	wxASSERT(!m_data.output_pending);
	wxASSERT(!m_data.input_pending);
	m_data.input_type = input_type;
	m_data.input_pending = true;
}
unsigned long raRuleEngine::GenerateMask(unsigned long *rules)
{
	unsigned long mask = 0xFFFFFFFF;
	unsigned long temp = 0;

	// Rule 1 :
	// If the first card to be played in the round
	// and if trump is not shown, the player who set the trump
	// cannot lead a trump.
	if(
		!m_data.tricks[m_data.trick_round].count &&
		!m_data.trump_shown &&
		(raTrickNext == m_data.curr_max_bidder))
	{
		wxASSERT(m_data.trump_suit != raSUIT_INVALID);
		mask = ~(raLib::m_suit_mask[m_data.trump_suit]);
		temp |= raRULE_1;
	}
	// Rule 4 :
	// If the max bidder asked for trump to be shown
	// he/she must play the very same card
	else if(m_data.should_play_trump_card)
	{
		wxASSERT(m_data.trump_card != raCARD_INVALID);
		mask  = 1 << m_data.trump_card;
		temp |= raRULE_4;
	}
	// Rule 2 :
	// If trump was asked to be shown, then trump must be played
	else if(m_data.should_trump)
	{
		wxASSERT(m_data.trump_suit != raSUIT_INVALID);
		mask = raLib::m_suit_mask[m_data.trump_suit];
		temp |= raRULE_2;
	}
	// Rule 3 :
	// Should follow suit
	else if(m_data.tricks[m_data.trick_round].count)
	{
		wxASSERT(m_data.tricks[m_data.trick_round].lead_suit != raSUIT_INVALID);
		mask = raLib::m_suit_mask[m_data.tricks[m_data.trick_round].lead_suit];
		temp |= raRULE_3;
	}
	wxASSERT(mask);

	// Set the rules which were considered
	if(rules)
		*rules = temp;
	return mask;
}
bool raRuleEngine::SetDealEndOutput()
{
	// Check whether there is a winner?
	if(m_data.pts[m_data.curr_max_bidder % 2] >= m_data.curr_max_bid)
	{
		m_data.out_deal_end_info.winner = m_data.curr_max_bidder % 2;
	}
	// TODO : Remove hard coding of 28
	else if(m_data.pts[(m_data.curr_max_bidder + 1) % 2] > (28 - m_data.curr_max_bid))
	{
		m_data.out_deal_end_info.winner = (m_data.curr_max_bidder + 1) % 2;
	}
	else
		return false;
	if(m_data.feedback)
		SetOutput(raOUTPUT_DEAL_END);
	return true;
}
