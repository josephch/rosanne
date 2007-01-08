// rosanne : Twenty-Eight(28) Card Game
// Copyright (C) 2006-2007 Vipin Cherian
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

#ifndef _RARULEENGINE_H_
#define _RARULEENGINE_H_

#include "ra/racommon.h"

#if defined ( raREAD_DEALER_FROM_FILE )
#include "wx/wfstream.h"
#include "wx/fileconf.h"
#elif defined ( raREAD_DEAL_FROM_FILE )
#include "wx/wfstream.h"
#include "wx/fileconf.h"
#endif

enum
{
	raSTATUS_NOT_STARTED = 0,
	raSTATUS_DEAL1,
	raSTATUS_BID1,
	raSTATUS_BID2,
	raSTATUS_TRUMPSEL1,
	raSTATUS_DEAL2,
	raSTATUS_BID3,
	raSTATUS_TRUMPSEL2,
	raSTATUS_TRICKS,
	raSTATUS_FINISHED
};

enum{
	raOUTPUT_INVALID = -1,
	raOUTPUT_STARTED,
	raOUTPUT_DEAL,
	raOUPUT_BID1,
	raOUPUT_BID2,
	raOUTPUT_TRUMPSEL,
	raOUTPUT_TRICK,
	raOUTPUT_DEAL_END
};

enum{
	raINPUT_INVALID = -1,
	raINPUT_BID,
	raINPUT_TRUMPSEL,
	raINPUT_TRICK
};
enum{
	raERR_CANNOT_PASS = 1,
	raERR_BID_LESS_THAN_MIN,
	raERR_BID_BY_WRONG_PLAYER,
	raERR_TRUMPSEL_BY_WRONG_PLAYER,
	raERR_TRUMPSEL_NONEXIST_CARD,
	raERR_TRICK_BY_WRONG_PLAYER,
	raERR_TRICK_INVALID_TRUMP_REQ,
	raERR_TRICK_MASK_MISMATCH,
	raERR_TRICK_CARD_NOT_IN_HAND
};

#define	raRULE_1 1
#define	raRULE_2 2
#define	raRULE_3 4
#define	raRULE_4 8
// Sluffing of jacks
#define	raRULE_5 16 

#define raDEAL_ROUND_1 0
#define raDEAL_ROUND_2 1

#define raBID_ROUND_3 2

#define raFOUR_JACKS 0x80808080
#define raJACK 0x80
#define raALL_CARDS 0xFFFFFFFF

typedef struct tagRA_RULES
{
	int rot_addn;
	int min_bid_1;
	int min_bid_2;
	int min_bid_3;
	bool waive_rule_4;
	bool sluff_jacks;
}raRules, *praRules;

typedef struct tag_raTRICK{
	bool trumped;
	int cards[raTOTAL_PLAYERS];
	int lead_suit;
	int lead_loc;
	int count;
	int points;
	int winner;
	//bool wait;
} raTrick, *praTrick;

#define raNext(X) ((X + m_data.rules.rot_addn) % raTOTAL_PLAYERS)
#define raTrickNext ((m_data.tricks[m_data.trick_round].lead_loc + (m_data.tricks[m_data.trick_round].count * m_data.rules.rot_addn)) % 4)
#define raWinnerCard (m_data.tricks[m_data.trick_round].cards[m_data.tricks[m_data.trick_round].winner])

typedef struct tagRA_OUTPUT_DEAL_INFO
{
	int round;
	unsigned long hands[raTOTAL_PLAYERS];
}raOutputDealInfo;

typedef struct tagRA_OUTPUT_TRICK_INFO
{
	int points[raTOTAL_TEAMS];
	raTrick trick;
}raOutputTrickInfo;

typedef struct tagRA_OUTPUT_DEAL_END_INFO
{
	int winner;
}raOutputDealEndInfo;

typedef struct tagRA_INPUT_BID_INFO
{
	int player;
	int min;
	//int max;
	bool passable;
	int bid;
	int round;
}raInputBidInfo;

typedef struct tagRA_INPUT_TRUMPSEL_INFO
{
	int card;
	int player;
}raInputTrumpselInfo;

typedef struct tagRA_INPUT_TRICK_INFO
{
	int player;
	bool can_ask_trump;
	bool ask_trump;
	unsigned long mask;
	int card;
	unsigned long rules;
}raInputTrickInfo;

typedef struct tagRA_RULEENGINE_DATA
{
	bool ok;
	bool feedback;

	raRules rules;

	int status;
	int dealer;

	int shuffled[raTOTAL_CARDS];
	//int deal_pos;

	// Variables related to messaging
	bool output_pending;
	bool input_pending;

	int output_type;
	int input_type;

	// Related to bidding
	bool first_bid;
	int curr_max_bid;
	int curr_max_bidder;
	int last_bidder;
	bool bid_hist[raTOTAL_BID_ROUNDS][raTOTAL_PLAYERS];
	unsigned long passed_round1;

	// Related to the trump card
	int trump_suit;
	int trump_card;

	// Cards held by players
	unsigned long hands[raTOTAL_PLAYERS];

	// Related to tricks
	int pts[raTOTAL_TEAMS];
	int trick_round;
	raTrick tricks[raTOTAL_TRICKS];
	unsigned long played_cards[raTOTAL_PLAYERS];
	bool should_trump;
	bool should_play_trump_card;
	bool trump_shown;

	// Related to output messages
	raOutputDealInfo out_deal_info;
	raOutputDealEndInfo out_deal_end_info;
	// Structures to hold data related to the pending input message
	raInputBidInfo in_bid_info;
	raInputTrumpselInfo in_trumpsel_info;
	raInputTrickInfo in_trick_info;
} raRuleEngineData;

/*typedef struct tagRA_OUPUT
{
	int type;
	void *output;
} raOutput, *praOutput;*/


class raRuleEngine
{
private:
	raRuleEngineData m_data;
	void SetOutput(int output_type);
	void SetInput(int input_type);
	unsigned long GenerateMask(unsigned long *rules = NULL);
	bool SetDealEndOutput();

public:
	raRuleEngine();
	virtual ~raRuleEngine();
	bool IsOk();
	int GetStatus();
	static bool Reset(raRuleEngineData *data);
	bool Reset();
	bool Shuffle();
	bool Continue();
	bool GetOutput(int *output_type, void *output);
	bool IsOutputPending();
	int GetPendingOutputType();
	bool IsInputPending();
	int GetPendingInputType();
	bool GetPendingInputCriteria(int *input_type, void *input);
	int PostInputMessage(int input_type, void *input);
	bool GetFeedback();
	void SetFeedback(bool feedback);
	void GetRules(raRules *rules);
	void SetRules(raRules *rules);
	void GetHands(unsigned long *hands);
	void GetCardsPlayed(unsigned long *cards);
	// TODO : Try to implement one method with trick_round as default variable
	void GetTrick(int trick_round, raTrick *trick); 
	void GetTrick(raTrick *trick); 
	int GetTrickRound();
	int GetPoints(int team);
	void GetPoints(int *pts);
	int GetTrump();
	int GetTrumpCard();
	int GetDealer();
	void SetDealer(int dealer);
	static void ResetTrick(raTrick *trick);
	bool GetData(raRuleEngineData *data);
	bool SetData(raRuleEngineData *data, bool check = true);
	bool GetMaxBid(int *bid, int *loc);
	wxString GetLoggable();
	static wxString PrintRuleEngineData(raRuleEngineData *data);
	bool IsTrumpShown();
	int GetTrickNextToPlay();
	void SetMinBid(int round, int bid);
	void SetWaiveRuleFour(bool flag);
	void SetSluffJacks(bool flag);
};



#endif
