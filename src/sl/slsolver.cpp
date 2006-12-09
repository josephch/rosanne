#include "sl/slsolver.h"


slSolver::slSolver()
{
	return;
}
slSolver::~slSolver()
{
	return;
}

// 
// Public methods
//

bool slSolver::GetRandomSolution(slSolution *solution)
{
	int i, j;
	int hands_count = 0, suits_count = 0;
	int cum_rel_prob[slTOTAL_SUITS];
	int weight_cache[slTOTAL_SUITS];
	int weight_new[slTOTAL_SUITS];
	bool first_time = true;
	int temp;
#ifdef slLOG_DEBUG_GETRANDSOLN
	wxString out;
#endif

	if(!solution)
	{
		wxLogError(wxString::Format(wxT("Input variable solution is null. %s:%d"), 
			__FILE__, __LINE__));
		return false;
	}

	// Copy saved problem to working
	memcpy(&m_working, &m_saved, sizeof(slData));

	//wxLogDebug("Before solving");

	// For each hand
	for(i = 0; i < slTOTAL_HANDS; i++)
	{
		first_time = true;
		// While there is at least one soft space to be filled
		while(m_working.hand_sum_of_softspaces[i])
		{
			// Get the current list of weights for the hand
			for(j = 0; j < slTOTAL_SUITS; j++)	
			{
				weight_new[j] = m_working.cells[i][j].soft_space;
			}

			// If it the first pass for the hand
			// create cache of weights identically to the 
			// calculated weights
			if(first_time)
			{
				memcpy(weight_cache, weight_new, sizeof(weight_new));
				first_time = false;
			}
			else
			{
#ifdef slLOG_DEBUG_GETRANDSOLN
				out.Empty();
				out.Append("\nWeight New\n");
				for(j = 0; j < slTOTAL_SUITS; j++)	
				{
					out.Append(wxString::Format("%d ", weight_new[j]));
				}
				out.Append("\n");
				wxLogDebug(out);

				out.Empty();
				out.Append("\nWeight cached\n");
				for(j = 0; j < slTOTAL_SUITS; j++)	
				{
					out.Append(wxString::Format("%d ", weight_cache[j]));
				}
				out.Append("\n");
				wxLogDebug(out);
#endif
				for(j = 0; j < slTOTAL_SUITS; j++)	
				{
					if(!!weight_new[j] != !!weight_cache[j])
					{
						memcpy(weight_cache, weight_new, sizeof(weight_new));
						break;
					}
				}
			}

			temp = 0;

			// Calculate the cumulative relative probability
#ifdef slLOG_DEBUG_GETRANDSOLN
			out.Empty();
#endif
			for(j = 0; j < slTOTAL_SUITS; j++)	
			{
				temp += weight_cache[j] * //m_working.cells[i][j].soft_space * 
					(m_working.suit_total_length[j] - m_working.suit_sum_of_mins[j]);
				cum_rel_prob[j] = temp;
#ifdef slLOG_DEBUG_GETRANDSOLN
				out.Append(wxString::Format("%d ", temp));
#endif
			}
#ifdef slLOG_DEBUG_GETRANDSOLN
			wxLogDebug(out);
#endif

			// Select a random suit for the hand
			// and add a card to the same
			temp = rand() % temp;
			for(j = 0; j < slTOTAL_SUITS; j++)	
			{
				if(temp < cum_rel_prob[j])
				{
#ifdef slLOG_DEBUG_GETRANDSOLN
					wxLogDebug(wxString::Format("Temp - %d Adding card to hand suit %d %d", temp, i, j));
#endif
					SetCell(&m_working, i, j, (m_working.cells[i][j].min + 1));
					break;
				}
			}
		}
	}

#ifdef slLOG_DEBUG_GETRANDSOLN
	wxLogDebug("After solving");
	wxLogDebug(slSolver::PrintData(&m_working));
#endif

	for(i = 0; i < slTOTAL_HANDS; i++)
		for(j = 0; j < slTOTAL_SUITS; j++)
			solution->suit_length[i][j] = m_working.cells[i][j].min;

	return true;
}

bool slSolver::SetProblem(slProblem *problem)
{
	int i, j;
	int u, v;

	wxASSERT(problem);

	// Set the data memory to zeros
	memset(&m_saved, 0, sizeof(slData));

	// Create a copy of the problem
	memcpy(&m_problem, problem, sizeof(slProblem));

	for(i = 0; i < slTOTAL_HANDS; i++)
	{
		for(j = 0; j < slTOTAL_SUITS; j++)
		{
			m_saved.cells[i][j].min = 0;
			m_saved.cells[i][j].max = slLENGTH_MAX;
			m_saved.cells[i][j].soft_space = slLENGTH_MAX;
		}
	}

	for(i = 0; i < slTOTAL_HANDS; i++)
	{
		//m_saved.hand_max_of_mins[i] = 0;
		m_saved.hand_sum_of_maxs[i] = slLENGTH_MAX * slTOTAL_HANDS;
		m_saved.hand_sum_of_mins[i] = 0;
		m_saved.hand_sum_of_softspaces[i] = slLENGTH_MAX * slTOTAL_HANDS;
	}

	for(i = 0; i < slTOTAL_SUITS; i++)
	{
		//m_saved.suit_max_of_mins[i] = 0;
		m_saved.suit_sum_of_maxs[i] = slLENGTH_MAX * slTOTAL_SUITS;
		m_saved.suit_sum_of_mins[i] = 0;
		m_saved.suit_available[i] = slLENGTH_MAX;
	}

	// Set suit length
	memcpy(m_saved.hand_total_length, 
		problem->hand_total_length, sizeof(m_saved.hand_total_length));
	memcpy(m_saved.suit_total_length, 
		problem->suit_total_length, sizeof(m_saved.suit_total_length));

	u = 0;
	for(i = 0; i < slTOTAL_HANDS; i++)
	{
		u += m_saved.hand_total_length[i];
	}

	v = 0;
	for(i = 0; i < slTOTAL_SUITS; i++)
	{
		v += m_saved.suit_total_length[i];
	}
	wxASSERT(u == v);
	
#ifdef slLOG_DEBUG_SETPROBLEM
	wxLogDebug("Before set cells");
	wxLogDebug(slSolver::PrintData(&m_saved));
#endif

	// Set individual cells
	for(i = 0; i < slTOTAL_HANDS; i++)
	{
		for(j = 0; j < slTOTAL_SUITS; j++)
		{
			if(!SetCell(&m_saved, i, j, m_problem.cells[i][j].min, m_problem.cells[i][j].max))
			{
				wxLogError(wxString::Format(wxT("SetCell() failed. %s:%d"), 
					__FILE__, __LINE__));
				return false;
			}
		}
	}

#ifdef slLOG_DEBUG_SETPROBLEM
	wxLogDebug("After set cells");
	wxLogDebug(slSolver::PrintData(&m_saved));
#endif

	return true;
}

bool slSolver::ResetProblem(slProblem *problem)
{
	int i, j;

	if(!problem)
	{
		wxLogError(wxString::Format(wxT("Input variable problem is null. %s:%d"), 
			__FILE__, __LINE__));
		return false;
	}

	for(i = 0; i < slTOTAL_HANDS; i++)
	{
		for(j = 0; j < slTOTAL_SUITS; j++)
		{
			problem->cells[i][j].min = 0;
			problem->cells[i][j].max = slLENGTH_MAX;
		}
	}

	for(i = 0; i < slTOTAL_HANDS; i++)
	{
		problem->hand_total_length[i] = slLENGTH_MAX;
	}

	for(i = 0; i < slTOTAL_SUITS; i++)
	{
		problem->suit_total_length[i] = slLENGTH_MAX;
	}

	return true;
}

wxString slSolver::PrintProblem(slProblem *problem)
{
	wxString out;
	int i, j;

	out.Empty();
	if(!problem)
	{
		wxLogError(wxString::Format(wxT("Input variable problem is null. %s:%d"), 
			__FILE__, __LINE__));
		return out;
	}

	out.Append("\n");
	for(i = 0; i < slTOTAL_SUITS; i++)
	{
		for(j = 0; j < slTOTAL_HANDS; j++)
		{
			out.Append(wxString::Format("%d/%d ", 
				problem->cells[j][i].min, problem->cells[j][i].max));
		}
		out.Append(wxString::Format("| %d\n", problem->suit_total_length[i]));
	}

	out.Append("-----------------\n");
	for(i = 0; i < slTOTAL_HANDS; i++)
	{
		out.Append(wxString::Format("  %d ", 
			problem->hand_total_length[i]));
	}

	return out;
}

wxString slSolver::PrintData(slData *data)
{
	wxString out;
	int i, j;

	out.Empty();
	if(!data)
	{
		wxLogError(wxString::Format(wxT("Input variable data is null. %s:%d"), 
			__FILE__, __LINE__));
		return out;
	}

	out.Append("\n");
	out.Append("       ");
	for(i = 0; i < slTOTAL_HANDS; i++)
		out.Append(wxString::Format("Hand %d|", i));
	out.Append("MaxLen|SumMax|SumMin");
	out.Append("\n");
	out.Append("------+------+------+------+------+------+------+------\n");

	for(i = 0; i < slTOTAL_SUITS; i++)
	{
		out.Append(wxString::Format("Suit %d|", i));
		for(j = 0; j < slTOTAL_HANDS; j++)
		{
			out.Append(wxString::Format("%d/%d(%d)|", 
				data->cells[j][i].min, data->cells[j][i].max, 
				data->cells[j][i].soft_space));
		}
		out.Append(wxString::Format("%6d|%6d|%6d\n", 
			data->suit_total_length[i],
			data->suit_sum_of_maxs[i],
			data->suit_sum_of_mins[i]
			));
		out.Append("------+------+------+------+------+------+------+------\n");
	}

	//out.Append("       ");
	//out.Append("----------------------------+--\n");
	//out.Append("------+------+------+------+------+------+------+------\n");
	//out.Append("         ");

	out.Append("Maxlen|");
	for(i = 0; i < slTOTAL_HANDS; i++)
	{
		out.Append(wxString::Format("%6d|", 
			data->hand_total_length[i]));
	}
	out.Append("\n");
	out.Append("------+------+------+------+------+\n");

	out.Append("SumMax|");
	for(i = 0; i < slTOTAL_HANDS; i++)
	{
		out.Append(wxString::Format("%6d|", 
			data->hand_sum_of_maxs[i]));
	}
	out.Append("\n");
	out.Append("------+------+------+------+------+\n");

	out.Append("SumMin|");
	for(i = 0; i < slTOTAL_HANDS; i++)
	{
		out.Append(wxString::Format("%6d|", 
			data->hand_sum_of_mins[i]));
	}
	out.Append("\n");

	out.Append("------+------+------+------+------+\n");

	out.Append("SumSft|");
	for(i = 0; i < slTOTAL_HANDS; i++)
	{
		out.Append(wxString::Format("%6d|", 
			data->hand_sum_of_softspaces[i]));
	}
	out.Append("\n");

	return out;
}

wxString slSolver::PrintSolution(slSolution *solution)
{
	int i, j;
	wxString out;

	wxASSERT(solution);

	out.Empty();
	out.Append("\n");
	for(i = 0; i < slTOTAL_HANDS; i++)
	{
		for(j = 0; j < slTOTAL_SUITS; j++)
		{
			out.Append(wxString::Format("|%d", solution->suit_length[i][j]));
		}
		out.Append("|\n");
	}
	return out;
}

//
// Private methods
//
bool slSolver::SetCell(slData *data, int i, int j, int min, int max)
{
	//TODO : Change wxASSERTs to ifs wherever appropriate

	// Pre-conditions
	if(!data)
	{
		wxLogError(wxString::Format(wxT("Null pointer. %s:%d"), 
			__FILE__, __LINE__));
		return false;
	}

	if((i < 0) || (i >= slTOTAL_HANDS))
	{
		wxLogError(wxString::Format(wxT("Invalid hand position. %s:%d"), 
			__FILE__, __LINE__));
		return false;
	}

	if((j < 0) || (j >= slTOTAL_SUITS))
	{
		wxLogError(wxString::Format(wxT("Invalid suit position. %s:%d"), 
			__FILE__, __LINE__));
		return false;
	}


	//wxASSERT((min >= 0) && (min <= slLENGTH_MAX));

	// Maximum must either be invalid
	//   or
	// 1. Maximum must be greater than or equal to min and
	// 2. Maximum must be less than the maximum length 
	//    possible for the suit and
	// 3. Maximum must be less than the maximum length 
	//    possible for the hand
	//wxASSERT((max == slLENGTH_INVALID) || ((max >= min) && (max <= data->suit_total_length[j]) && (max <= data->hand_total_length[i])));
	if(max != slLENGTH_INVALID)
	{
		if(max < min)
		{
			wxLogError(wxString::Format(wxT("Max less than min. %s:%d"), 
				__FILE__, __LINE__));
			return false;
		}
		//wxASSERT(max <= data->hand_total_length[i]);
		//wxASSERT(max <= data->suit_total_length[j]);
	}

	// Minimum must be greater than zero
	if(min < 0)
	{
		wxLogError(wxString::Format(wxT("Minimum less than zero. %s:%d"), 
			__FILE__, __LINE__));
		return false;
	}

	//wxASSERT(min <= data->hand_total_length[i]);
	//wxASSERT(min <= data->suit_total_length[j]);

	// Minimum value must not be such that the minimum 
	// length for the suit or the hand is not overrun

	if((min + m_hand_sum_of_min[i]) > data->hand_total_length[i])
	{
		wxLogError(wxString::Format(wxT("Minimum value will cause overrun of hand length. %s:%d"), 
			__FILE__, __LINE__));
		return false;
	}
	if((min + m_suit_sum_of_min[j]) > data->suit_total_length[j])
	{
		wxLogError(wxString::Format(wxT("Minimum value will cause overrun of suit length. %s:%d"), 
			__FILE__, __LINE__));
		return false;
	}

	// Since minimum value is going to change, reset the sum of mins

	data->hand_sum_of_mins[i] -= (data->cells[i][j].min - min);
	data->suit_sum_of_mins[j] -= (data->cells[i][j].min - min);

	wxASSERT(data->hand_sum_of_mins[i] >= 0);
	wxASSERT(data->suit_sum_of_mins[j] >= 0);
	wxASSERT(data->hand_sum_of_mins[i] <= data->hand_total_length[i]);
	wxASSERT(data->suit_sum_of_mins[j] <= data->suit_total_length[j]);

	// Set the maximum and minim value

	data->cells[i][j].min = min;
	if(max != slLENGTH_INVALID)
	{
		data->hand_sum_of_maxs[i] -= (data->cells[i][j].max - max);
		data->suit_sum_of_maxs[j] -= (data->cells[i][j].max - max);
		data->cells[i][j].max = max;
	}
	data->hand_sum_of_softspaces[i] -= 
		(data->cells[i][j].soft_space - 
		(data->cells[i][j].max - data->cells[i][j].min));
	data->cells[i][j].soft_space = 
		data->cells[i][j].max - data->cells[i][j].min;

	// Calculate the maximum of cell minimum values
	// for both suit and hand and check for overrun

	//data->hand_max_of_mins[i] = slMax(min, data->hand_max_of_mins[i]);
	//data->suit_max_of_mins[j] = slMax(min, data->suit_max_of_mins[j]);

	//wxASSERT((data->hand_max_of_mins[i] >= 0) && (data->hand_max_of_mins[i] <= data->hand_total_length[i]));
	//wxASSERT((data->suit_max_of_mins[i] >= 0) && (data->suit_max_of_mins[i] <= data->suit_total_length[j]));

	// Recalculate maximum values for affected cells
	RecalcAffectedCellsMax(data, i, j);
	RecalcAllCellMin(data);

	// Post-conditions

	return true;
}

bool slSolver::RecalcAffectedCellsMax(slData *data, int hand, int suit)
{
	int i;

	// Pre conditions
	wxASSERT(data);
	wxASSERT(hand <= slTOTAL_HANDS);
	wxASSERT(suit <= slTOTAL_SUITS);

	for(i = 0; i < slTOTAL_HANDS; i++)
	{
		RecalcCellMax(data, i, suit);
	}
	for(i = 0; i < slTOTAL_SUITS; i++)
	{
		RecalcCellMax(data, hand, i);
	}
	return true;
}
bool slSolver::RecalcCellMax(slData *data, int hand, int suit)
{
	int x, y, z;

	// Pre conditions
	wxASSERT(data);
	wxASSERT(hand <= slTOTAL_HANDS);
	wxASSERT(suit <= slTOTAL_SUITS);

	// Adjust the maximum for a cell
	// inorder to prevent overrun

#ifdef slLOG_DEBUG_RECALCCELL_MAX
	wxLogDebug(wxString::Format("Recalculating cell maximum for %d %d", hand, suit));
	wxLogDebug("Data before");
	wxLogDebug(slSolver::PrintData(data));
#endif

	x = data->hand_total_length[hand] - data->hand_sum_of_mins[hand];
	y = data->suit_total_length[suit] - data->suit_sum_of_mins[suit];
#ifdef slLOG_DEBUG_RECALCCELL_MAX
	wxLogDebug(wxString::Format("x y -  %d %d", x, y));
#endif
	z = data->cells[hand][suit].min + slMin(x, y);
#ifdef slLOG_DEBUG_RECALCCELL_MAX
	wxLogDebug(wxString::Format("z -  %d", z));
#endif
	z = slMin(data->cells[hand][suit].max, z);
#ifdef slLOG_DEBUG_RECALCCELL_MAX
	wxLogDebug(wxString::Format("z -  %d", z));


	wxLogDebug(wxString::Format("data->hand_sum_of_maxs[hand] -  %d", data->hand_sum_of_maxs[hand]));
	wxLogDebug(wxString::Format("data->suit_sum_of_maxs[suit] -  %d", data->suit_sum_of_maxs[suit]));
#endif
	data->hand_sum_of_maxs[hand] = data->hand_sum_of_maxs[hand] - data->cells[hand][suit].max + z;
	data->suit_sum_of_maxs[suit] = data->suit_sum_of_maxs[suit] - data->cells[hand][suit].max + z;
#ifdef slLOG_DEBUG_RECALCCELL_MAX
	wxLogDebug(wxString::Format("data->hand_sum_of_maxs[hand] -  %d", data->hand_sum_of_maxs[hand]));
	wxLogDebug(wxString::Format("data->suit_sum_of_maxs[suit] -  %d", data->suit_sum_of_maxs[suit]));
#endif

	data->cells[hand][suit].max = z;
	data->hand_sum_of_softspaces[hand] -= 
		(data->cells[hand][suit].soft_space - 
		(data->cells[hand][suit].max - data->cells[hand][suit].min));
	data->cells[hand][suit].soft_space =  
		data->cells[hand][suit].max -  data->cells[hand][suit].min;

#ifdef slLOG_DEBUG_RECALCCELL_MAX
	wxLogDebug(wxString::Format("z -  %d", z));
	wxLogDebug(wxString::Format("Hand total length %d - %d", hand, data->hand_total_length[hand]));
	wxLogDebug(wxString::Format("Suit total length %d - %d", suit, data->suit_total_length[suit]));

	wxLogDebug("Data after");
	wxLogDebug(slSolver::PrintData(data));
#endif

	// Post conditions
	wxASSERT(data->cells[hand][suit].soft_space >= 0);
	wxASSERT(z >= data->cells[hand][suit].min);
	wxASSERT(z <= data->hand_total_length[hand]);
	wxASSERT(z <= data->suit_total_length[suit]);

	return true;

}

bool slSolver::RecalcAllCellMin(slData *data)
{
	int i, j;

	// Recalculate the minimum for each cells
	for(i = 0; i < slTOTAL_HANDS; i++)
	{
		for(j = 0; j < slTOTAL_SUITS; j++)
		{
			RecalcCellMin(data, i, j);//, data->hand_sum_of_maxs[i], data->suit_sum_of_maxs[j]);
		}
	}

	return true;
}
bool slSolver::RecalcCellMin(slData *data, int hand, int suit)//, int hand_sum_of_max, int suit_sum_of_max)

{
	int x, y, z;
	int i;

	// Pre conditions
	wxASSERT(data);
	wxASSERT(hand <= slTOTAL_HANDS);
	wxASSERT(suit <= slTOTAL_SUITS);

#ifdef slLOG_DEBUG_RECALCCELL_MIN
	wxLogDebug(wxString::Format("Recalculating cell minimum for %d %d", hand, suit));
	wxLogDebug("Data before");
	wxLogDebug(slSolver::PrintData(data));
#endif

	// Adjust the minimum for a cell
	// inorder to prevent overrun or underrun

	x = data->hand_total_length[hand] - data->hand_sum_of_maxs[hand] + data->cells[hand][suit].max;
	y = data->suit_total_length[suit] - data->suit_sum_of_maxs[suit] + data->cells[hand][suit].max;
	z = slMax(x, y);
	z = slMax(data->cells[hand][suit].min, z);

	wxASSERT(z >= 0);
	wxASSERT(z <= data->hand_total_length[hand]);
	wxASSERT(z <= data->suit_total_length[suit]);

	// Since minimum value is going to change, reset the sum of mins
	//data->hand_sum_of_mins[hand] -= (data->cells[hand][suit].min - z);
	//data->suit_sum_of_mins[suit] -= (data->cells[hand][suit].min - z);

	//wxASSERT(data->hand_sum_of_mins[hand] >= 0);
	//wxASSERT(data->suit_sum_of_mins[suit] >= 0);

	//data->cells[hand][suit].min = z;
	if(data->cells[hand][suit].min != z)
	{
		// Since minimum value is going to change, reset the sum of mins

		data->hand_sum_of_mins[hand] -= (data->cells[hand][suit].min - z);
		data->suit_sum_of_mins[suit] -= (data->cells[hand][suit].min - z);

		wxASSERT(data->hand_sum_of_mins[hand] >= 0);
		wxASSERT(data->suit_sum_of_mins[suit] >= 0);

		data->cells[hand][suit].min = z;

		wxASSERT(data->cells[hand][suit].min <= data->cells[hand][suit].max);

		for(i = 0; i < slTOTAL_HANDS; i++)
		{
			if(!RecalcCellMax(data, i, suit))
			{
				wxLogError(wxString::Format("RecalcCellMax() failed. %s:%d", 
					__FILE__, __LINE__));
				return false;
			}
		}


		for(i = 0; i < slTOTAL_SUITS; i++)
		{
			if(!RecalcCellMax(data, hand, i))
			{
				wxLogError(wxString::Format("RecalcCellMax() failed. %s:%d", 
					__FILE__, __LINE__));
				return false;
			}
		}
	}

#ifdef slLOG_DEBUG_RECALCCELL_MIN
	wxLogDebug(wxString::Format("z -  %d", z));
	wxLogDebug(wxString::Format("Hand total length %d - %d", hand, data->hand_total_length[hand]));
	wxLogDebug(wxString::Format("Suit total length %d - %d", suit, data->suit_total_length[suit]));

	wxLogDebug("Data after");
	wxLogDebug(slSolver::PrintData(data));
#endif

	return true;
}



