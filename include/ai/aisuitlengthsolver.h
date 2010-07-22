// Rosanne : Trump card game popularly known as Twenty Eight (28)
// Copyright (C) 2006-2010 Vipin Cherian
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#ifndef _AI_SUITLENGTHSOLVER_H_
#define _AI_SUITLENGTHSOLVER_H_
//#include <assert.h>
//#include <memory.h>
//#include <stdio.h>
#include "wx/wx.h"


#define slMin(X, Y)  ((X) < (Y) ? (X) : (Y))
#define slMax(X, Y)  ((X) > (Y) ? (X) : (Y))

#define slLENGTH_MAX 8
#define slTOTAL_HANDS 4
#define slTOTAL_SUITS 4
#define slLENGTH_INVALID -1

/*
#define slLOG_DEBUG_SETPROBLEM 0
#define slLOG_DEBUG_SETCELL 0
#define slLOG_DEBUG_RECALCCELL_MAX 0
#define slLOG_DEBUG_RECALCCELL_MIN 0
#define slLOG_DEBUG_GETRANDSOLN 0
*/
//#include "gsl/gsl_rng.h"
//extern gsl_rng *g_rng;

typedef struct slCELL
{
	int min;
	int max;
	int soft_space;
}slCell;

typedef struct slCELL_INPUT
{
	int min;
	int max;
}slCellInput;

typedef struct slPROBLEM
{
	slCellInput cells[slTOTAL_HANDS][slTOTAL_SUITS];
	int suit_total_length[slTOTAL_SUITS];
	int hand_total_length[slTOTAL_HANDS];
}slProblem;

typedef struct slSOLUTION
{
	int suit_length[slTOTAL_HANDS][slTOTAL_SUITS];
}slSolution;

typedef struct slDATA
{
	slCell cells[slTOTAL_HANDS][slTOTAL_SUITS];
	int suit_total_length[slTOTAL_SUITS];
	int hand_total_length[slTOTAL_HANDS];
	int suit_sum_of_mins[slTOTAL_SUITS];
	int hand_sum_of_mins[slTOTAL_HANDS];
	int suit_sum_of_maxs[slTOTAL_SUITS];
	int hand_sum_of_maxs[slTOTAL_HANDS];
	// TODO : remove max of mins if it is not used
	//int suit_max_of_mins[slTOTAL_SUITS];
	//int hand_max_of_mins[slTOTAL_HANDS];
	int hand_sum_of_softspaces[slTOTAL_HANDS];
	int suit_available[slTOTAL_SUITS];
	//bool cell_set;
}slData;

class aiSuitLengthSolver
{
private:
	slProblem m_problem;
	slData m_saved, m_working;
	int m_suit_sum_of_min[slTOTAL_SUITS];
	int m_hand_sum_of_min[slTOTAL_HANDS];
	// Disallow copy constructor/assignment operators
	aiSuitLengthSolver(const aiSuitLengthSolver &);
    aiSuitLengthSolver & operator=(const aiSuitLengthSolver &);
	bool SetCell(slData *data, int i, int j, int min, int max = slLENGTH_INVALID);
	bool RecalcAffectedCellsMax(slData *data, int hand, int suit);
	bool RecalcCellMax(slData *data, int hand, int suit);
	bool RecalcAllCellMin(slData *data);
	bool RecalcCellMin(slData *data, int hand, int suit);
public:
	aiSuitLengthSolver();
	~aiSuitLengthSolver();
	bool SetProblem(slProblem *problem);
	bool GetRandomSolution(slSolution *solution);
	static bool ResetProblem(slProblem *problem);
	static wxString PrintProblem(slProblem *problem);
	static wxString PrintData(slData *data);
	static wxString PrintSolution(slSolution *solution);
};



#endif
