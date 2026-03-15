#include "aibid.h"
#include "gm/gmutil.h"

int main()
{
	unsigned long cards = 0x40118000;
	wxLogDebug(gmUtil::PrintLong(cards));
	int bid;
	int trump;
	int min = 14;
	bool force_bid = false;
	bool ret = aiBid_GetBid(cards, &bid, &trump, min, force_bid);
	wxLogDebug("bid %d trump %d", bid, trump);
	assert(ret);
	return 0;
}
