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

#ifndef _RALIB_H_
#define _RALIB_H_

#include "ra/racommon.h"

#define SPACES20 _("                    ")

#define raDEBUG_DONT_SHOW_ALL_CARDS 1

typedef struct ra_COLOURS
{
	wxColour cap_nrm_edg_col;
	wxColour cap_nrm_bak_col;
	wxColour cap_nrm_txt_col;

	wxColour cap_hgh_edg_col;
	wxColour cap_hgh_bak_col;
	wxColour cap_hgh_txt_col;

	wxColour btn_nrm_edg_col;
	wxColour btn_nrm_bak_col;
	wxColour btn_nrm_txt_col;

	wxColour btn_hgh_edg_col;
	wxColour btn_hgh_bak_col;
	wxColour btn_hgh_txt_col;

	wxColour btn_dsb_edg_col;
	wxColour btn_dsb_bak_col;
	wxColour btn_dsb_txt_col;

	wxColour pnl_nrm_edg_col;
	wxColour pnl_nrm_bak_col;
	wxColour pnl_nrm_txt_col;

	wxColour pnl_hgh_edg_col;
	wxColour pnl_hgh_bak_col;
	wxColour pnl_hgh_txt_col;

	wxColour pnl_dsb_edg_col;
	wxColour pnl_dsb_bak_col;
	wxColour pnl_dsb_txt_col;

} raColours;

class raLib
{
public:
	static int m_value_trans[];
	static void ShuffleArray(int *array, unsigned long n);
	static wxString PrintLong(unsigned long cards);
	static wxString PrintHands(unsigned long *hands);
	static wxString m_suits[];
	static wxString m_values[];
	static wxString m_short_locs[];
	static wxString m_long_locs[];
	static unsigned long m_suit_mask[];
	static unsigned long m_suit_rs[];
	static int m_points[];
	static int m_total_points[];
	static wxString m_short_teams[];
	static int GetCardIndex(wxString text);
};

#define raGET_CARD_INDEX(crd) ((crd->GetSuit() << 3) + raLib::m_value_trans[crd->GetValue()])

#define raMin(X, Y)  ((X) < (Y) ? (X) : (Y))
#define raMax(X, Y)  ((X) > (Y) ? (X) : (Y))
#define raTotalPoints(X) ( \
raLib::m_total_points[(raLib::m_suit_mask[0] & X) >> raLib::m_suit_rs[0]] + \
raLib::m_total_points[(raLib::m_suit_mask[1] & X) >> raLib::m_suit_rs[1]] + \
raLib::m_total_points[(raLib::m_suit_mask[2] & X) >> raLib::m_suit_rs[2]] + \
raLib::m_total_points[(raLib::m_suit_mask[3] & X) >> raLib::m_suit_rs[3]]  \
)

#endif

