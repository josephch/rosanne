#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "SFMT.h"
#include "SFMT-params.h"

typedef struct tagGM_RAND_STATE
{
	uint32_t state_array[SFMT_N32];
	int idx;
}gmRandState;


class gmRand
{
public:
    static wxString PrintState();
    static void GetState(gmRandState *state);
    static void SetState(gmRandState *state);
};


