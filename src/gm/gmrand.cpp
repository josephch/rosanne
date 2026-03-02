#include "gm/gmrand.h"

#include "SFMT.h"
static sfmt_t sfmt;
wxString gmRand::PrintState()
{
    wxString out;
	int i;

	uint32_t *psfmt32 = &sfmt.state[0].u[0];

	out.Empty();

	out.Append(wxT("\n-Random State-\n"));
	for (i = 0; i < SFMT_N32; i++)
	{
		out.Append(wxString::Format(wxT("seed%02d=%08X\n"), i, psfmt32[i]));
	}
	out.Append(wxString::Format(wxT("idx=%d\n"), sfmt.idx));
	out.Append(wxT("-Random State-"));

	return out;
}

void gmRand::GetState(gmRandState *state)
{
    int i = 0;
    uint32_t *psfmt32 = &sfmt.state[0].u[0];
    for(i = 0; i < SFMT_N32; i++)
    {
        state->state_array[i] = psfmt32[i];
        state->idx = sfmt.idx;
    }
}

void gmRand::SetState(gmRandState *state)
{
    int i = 0;
    uint32_t *psfmt32 = &sfmt.state[0].u[0];
    for(i = 0; i < SFMT_N32; i++)
    {
        psfmt32[i] = state->state_array[i];
        sfmt.idx = state->idx;
    }
}

