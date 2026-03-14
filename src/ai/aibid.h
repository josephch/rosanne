#ifndef AIBID_H_INCLUDED
#define AIBID_H_INCLUDED

#define aiBID_SAMPLE 100

bool aiBid_GetBid(unsigned long cards, int *bid, int *trump, int min, bool force_bid);
bool EstimatePoints(unsigned long *hands, int trump, int trick_no, int *eval);

#endif // AIBID_H_INCLUDED
