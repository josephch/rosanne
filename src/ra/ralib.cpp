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

#include "ra/ralib.h"


//int raLib::m_value_trans[] = {5, -1, -1, -1, -1, -1, 0, 1, 6, 4, 7, 2, 3};
int raLib::m_value_trans[] = {6, 7, 11, 12, 9, 0, 8, 10};
wxString raLib::m_suits[] = {"C", "D", "H", "S"};
wxString raLib::m_values[] = {"7", "8", "Q", "K", "10", "A", "9", "J"};
wxString raLib::m_short_locs[] = {"S", "W", "N", "E"};
wxString raLib::m_long_locs[] = {"South", "West", "North", "East"};
unsigned long raLib::m_suit_mask[] = {0x000000FF, 0x0000FF00, 0x00FF0000, 0xFF000000};
unsigned long raLib::m_suit_rs[] = {0, 8, 16, 24};
int raLib::m_points[] = {0, 0, 0, 0, 1, 1, 2, 3};
int raLib::m_total_points[] = {
	0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0,
1, 1, 1, 1, 1, 1, 1, 1,
1, 1, 1, 1, 1, 1, 1, 1,
1, 1, 1, 1, 1, 1, 1, 1,
1, 1, 1, 1, 1, 1, 1, 1,
2, 2, 2, 2, 2, 2, 2, 2,
2, 2, 2, 2, 2, 2, 2, 2,
2, 2, 2, 2, 2, 2, 2, 2,
2, 2, 2, 2, 2, 2, 2, 2,
3, 3, 3, 3, 3, 3, 3, 3,
3, 3, 3, 3, 3, 3, 3, 3,
3, 3, 3, 3, 3, 3, 3, 3,
3, 3, 3, 3, 3, 3, 3, 3,
4, 4, 4, 4, 4, 4, 4, 4,
4, 4, 4, 4, 4, 4, 4, 4,
3, 3, 3, 3, 3, 3, 3, 3,
3, 3, 3, 3, 3, 3, 3, 3,
4, 4, 4, 4, 4, 4, 4, 4,
4, 4, 4, 4, 4, 4, 4, 4,
4, 4, 4, 4, 4, 4, 4, 4,
4, 4, 4, 4, 4, 4, 4, 4,
5, 5, 5, 5, 5, 5, 5, 5,
5, 5, 5, 5, 5, 5, 5, 5,
5, 5, 5, 5, 5, 5, 5, 5,
5, 5, 5, 5, 5, 5, 5, 5,
6, 6, 6, 6, 6, 6, 6, 6,
6, 6, 6, 6, 6, 6, 6, 6,
6, 6, 6, 6, 6, 6, 6, 6,
6, 6, 6, 6, 6, 6, 6, 6,
7, 7, 7, 7, 7, 7, 7, 7,
7, 7, 7, 7, 7, 7, 7, 7
};
wxString raLib::m_short_teams[] = {"N/S", "E/W", "N/S", "E/W"};

void raLib::ShuffleArray(int *array, unsigned long n)
{
	
	if (n > 1) {
		unsigned long i;
		for (i = 0; i < n - 1; i++) {
			unsigned long j = i + rand() / (RAND_MAX / (n - i) + 1);
			int t = array[j];
			array[j] = array[i];
			array[i] = t;
		}
	}
}

wxString raLib::PrintLong(unsigned long cards)
{
	unsigned long i;
	wxString out, final;

	//wxLogDebug(wxString::Format("Cards = %lu", cards));

	out = _("");
	//wxLogDebug(wxString::Format("%s%s", m_suits[highest / 8], m_values[highest % 8]));
	for(i = 0; i < 32; i++)
	{
		if(cards & (1 << i))
			out = out + m_suits[i / 8] + m_values[i % 8] + _(",");
	}
	if(!out.IsEmpty())
	{
		final = out.Left(out.Length() - 1);
	}

	return final;
}
wxString raLib::PrintHands(unsigned long *hands)
{	
	wxString ret_val;
	wxString final;
	wxString out;
	unsigned long temp;
	int i, j;

	ret_val.Clear();

	// Print North first
	//wxLogMessage(SPACES20 + m_long_locs[2]);
	ret_val.Append(SPACES20 + m_long_locs[2]);
	ret_val.Append("\n");

	for (i = 0; i < 4; i++) 
	{
		temp = (hands[2] & m_suit_mask[i]) >> m_suit_rs[i];
		out = wxString::Format("%s - ", m_suits[i].c_str());
		for(j = 7; j >= 0; j--)
		{
			if(temp & (1 << j))
				out = out + m_values[j % 8] + _(",");
		}

		final = out.Left(out.Length() - 1);
		//wxLogMessage(SPACES20 + final);
		ret_val.Append(SPACES20 + final);
		ret_val.Append("\n");
	}
	
	// Print East and West in the same line :D
	//wxLogMessage(wxString::Format("%-40s%-40s", m_long_locs[1], m_long_locs[3]));
	ret_val.Append(wxString::Format("%-40s%-40s", m_long_locs[1].c_str(), m_long_locs[3].c_str()));
	ret_val.Append("\n");

	for (i = 0; i < 4; i++) 
	{
		temp = (hands[1] & m_suit_mask[i]) >> m_suit_rs[i];
		out = wxString::Format("%s - ", m_suits[i].c_str());
		//for(j = 0; j < 8; j++)
		for(j = 7; j >= 0; j--)
		{
			if(temp & (1 << j))
				out = out + m_values[j % 8] + _(",");
		}

		final = wxString::Format("%-40s", out.Left(out.Length() - 1).c_str());

		temp = (hands[3] & m_suit_mask[i]) >> m_suit_rs[i];
		out = wxString::Format("%s - ", m_suits[i].c_str());
		//for(j = 0; j < 8; j++)
		for(j = 7; j >= 0; j--)
		{
			if(temp & (1 << j))
				out = out + m_values[j % 8] + _(",");
		}

		final += wxString::Format("%-40s", out.Left(out.Length() - 1).c_str());

		//wxLogMessage(final);
		ret_val.Append(final);
		ret_val.Append("\n");
	}

	// Finally print South
	//wxLogMessage(SPACES20 + m_long_locs[0]);
	ret_val.Append(SPACES20 + m_long_locs[0]);
	ret_val.Append("\n");

	for (i = 0; i < 4; i++) 
	{
		temp = (hands[0] & m_suit_mask[i]) >> m_suit_rs[i];
		out = wxString::Format("%s - ", m_suits[i].c_str());
		//for(j = 0; j < 8; j++)
		for(j = 7; j >= 0; j--)
		{
			if(temp & (1 << j))
				out = out + m_values[j % 8] + _(",");
		}

		final = out.Left(out.Length() - 1);
		//wxLogMessage(SPACES20 + final);
		ret_val.Append(SPACES20 + final);
		ret_val.Append("\n");
	}
	return ret_val;
}

int raLib::GetCardIndex(wxString text)
{
	wxString suit, value;
	int x, y;
	suit = text.Left(1);
	//wxPrintf(suit + "\n");
	value = text.Mid(1);
	//wxPrintf(value + "\n");

	if(!suit.CmpNoCase("C"))
		x = 0;
	else if(!suit.CmpNoCase("D"))
		x = 1;
	else if(!suit.CmpNoCase("H"))
		x = 2;
	else if(!suit.CmpNoCase("S"))
		x = 3;
	else
		return -1;

	if(!value.CmpNoCase("7"))
		y = 0;
	else if(!value.CmpNoCase("8"))
		y = 1;
	else if(!value.CmpNoCase("Q"))
		y = 2;
	else if(!value.CmpNoCase("K"))
		y = 3;
	else if(!value.CmpNoCase("10"))
		y = 4;
	else if(!value.CmpNoCase("A"))
		y = 5;
	else if(!value.CmpNoCase("9"))
		y = 6;
	else if(!value.CmpNoCase("J"))
		y = 7;
	else 
		return -1;

	return (x * 8) + y;
}
bool raLib::SetStatusText(const wxString& text, int i)
{
	wxFrame *main_frame;
	wxStatusBar *status_bar;

	main_frame = NULL;
	main_frame = (wxFrame *)wxTheApp->GetTopWindow();
	if(!main_frame)
		return false;

	status_bar = NULL;
	status_bar = main_frame->GetStatusBar();
	if(!status_bar)
		return false;

	status_bar->SetStatusText(text, i);
	status_bar->Update();
	return true;
}


