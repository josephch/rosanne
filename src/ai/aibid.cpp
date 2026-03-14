#include "aibid.h"
#include "gm/gmutil.h"

//#define raAI_LOG_GETBID


static inline bool EstimateTricks(unsigned long *p_hands, int trump, int *eval)
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

	wxLogDebug(gmUtil::PrintHands(p_hands));
	wxLogDebug(_("Trump is ") + gmUtil::m_suits[trump]);
	wxLogDebug("Combined hands :");
	wxLogDebug(_("N/S - ") + gmUtil::PrintLong(combined[0]));
	wxLogDebug(_("E/W - ") + gmUtil::PrintLong(combined[1]));
#endif

	// Loop though each of the suits
	for(i = 0; i < 4; i++)
	{
		suit[0] = (combined[0] & gmUtil::m_suit_mask[i]) >> gmUtil::m_suit_rs[i];
		suit[1] = (combined[1] & gmUtil::m_suit_mask[i]) >> gmUtil::m_suit_rs[i];
#ifdef raAI_LOG_ESTIMATE_TRICKS
		wxLogDebug("-----------------------");
		wxLogDebug(_("Evaluating suit - ") + gmUtil::m_suits[i]);
#endif
		if(suit[0] > suit[1])
			j = 0;
		else
			j = 1;

#ifdef raAI_LOG_ESTIMATE_TRICKS
		wxLogDebug(_("Stronger team is - ") + gmUtil::m_short_teams[j]);
#endif

		// Number of cards with the stronger team which are stronger than
		// the strongest card in the weakest team

		stronger = gmUtil::CountBitsSet(
			suit[j] & (0xFFFFFFFF << gmUtil::HighestBitSet(suit[!j])));
#ifdef raAI_LOG_ESTIMATE_TRICKS
		wxLogDebug(wxString::Format("Number of stronger cards - %d", stronger));
#endif

		suit_count[!j] =
			gmUtil::CountBitsSet(p_hands[!j] & gmUtil::m_suit_mask[i]);
		suit_count[(!j) + 2] =
			gmUtil::CountBitsSet(p_hands[(!j) + 2] & gmUtil::m_suit_mask[i]);
#ifdef raAI_LOG_ESTIMATE_TRICKS
		wxLogDebug(_("Individual counts of cards with opposition ") + gmUtil::m_short_teams[!j]);
		wxLogDebug(wxString::Format("%s - %d", gmUtil::m_long_locs[!j].c_str(), suit_count[!j]));
		wxLogDebug(wxString::Format("%s - %d", gmUtil::m_long_locs[(!j) + 2].c_str(), suit_count[(!j) + 2]));
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
				gmUtil::CountBitsSet(p_hands[j] & gmUtil::m_suit_mask[i]);
			suit_count[j + 2] =
				gmUtil::CountBitsSet(p_hands[j + 2] & gmUtil::m_suit_mask[i]);

#ifdef raAI_LOG_ESTIMATE_TRICKS
			wxLogDebug(_("Tricks expected - ") +
				wxString::Format("%d", std::min(stronger, std::max(suit_count[j], suit_count[j + 2]))));
#endif
			tricks[j] += std::min(stronger, std::max(suit_count[j], suit_count[j + 2]));

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

			trump_adv = std::max(suit_count[j], suit_count[j + 2])
				- std::max(suit_count[!j], suit_count[(!j) + 2]);
#ifdef raAI_LOG_ESTIMATE_TRICKS
			wxLogDebug(_("Trump advantage for ") + gmUtil::m_short_teams[j].c_str() + _(" is ") + wxString::Format("%d", trump_adv));
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
				wxString::Format("%d", std::min(std::min(suit_count[!j], suit_count[(!j) + 2]), stronger)));
#endif
			tricks[j] += std::min(std::min(suit_count[!j], suit_count[(!j) + 2]), stronger);
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

bool EstimatePoints(unsigned long *hands, int trump, int trick_no, int *eval)
{
	int trick_count[2];
	unsigned long all_cards = 0;
	int total_pts;

	// Set both ints in eval to 0
	memset(eval, 0, 2 * sizeof(int));

	all_cards =  hands[0] | hands[1] | hands[2] | hands[3];
	total_pts = gmTotalPoints(all_cards);

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


bool aiBid_GetBid(unsigned long cards, int *bid, int *trump, int min, bool force_bid)
{
	int i, k;
	int initial;
	int undealt[32];
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
	data[i] = new int[aiBID_SAMPLE];
	memset(data[i], 0, aiBID_SAMPLE * sizeof(int));
	}*/
	memset(data, 0, 4 * 29 * sizeof(int));

#ifdef raAI_LOG_GETBID
	wxLogDebug(gmUtil::PrintLong(cards));
#endif

	initial = (int)gmUtil::CountBitsSet(cards);
	wxASSERT(initial <= 8);
#ifdef raAI_LOG_GETBID
	wxLogDebug(wxString::Format("initial is %d", initial));
#endif
	//
	//Dealing the rest of the cards
	//

	for(sample = 0; sample < aiBID_SAMPLE; sample++)
	{
		// Get the rest of the cards into undealt
		k = 0;
		for(i = 0; i < 32; i++)
			if(!(cards & (1u << i)))
				undealt[k++] = i;

		//Shuffle the undealt
		gmUtil::ShuffleArray(undealt, 32 - initial);

		//Initialize the hands
		memset(hands, 0, sizeof(hands));

		// Simplifying the problem, assume the current player is South
		// Add the cards already dealt to South
		hands[0] = cards;


		// Deal the undealt cards
		k = 0;
		for(i = /*8 - */initial; i < 32; i++)
			hands[i / 8] |= 1u << undealt[k++];

#ifdef raAI_LOG_GETBID
		wxLogDebug("#############################");
		wxLogDebug(gmUtil::PrintHands(hands));
#endif


		// Calculate the estimated points
		// considering each suit as trump

		for(trump_count = 0; trump_count < 4; trump_count++)
		{
			// Estimation done only if there is atleast one card
			// belonging to the suit, in the initial hand
			if(cards & gmUtil::m_suit_mask[trump_count])
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
					wxLogDebug(gmUtil::m_suits[trump_count].c_str());
				}
				wxLogDebug(wxString::Format("%s : %d - %d", gmUtil::m_suits[trump_count].c_str(), eval[0], eval[1]));
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
		wxLogDebug(_("Trump - ") + gmUtil::m_suits[i]);
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
			if((((double)(sample) / aiBID_SAMPLE) >= 0.67) && (k > *bid))
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
