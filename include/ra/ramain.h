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

#ifndef _RAMAIN_H_
#define _RAMAIN_H_

//#include "wx/wx.h"
#include "wx/image.h"
#include "wx/splitter.h"
#include "wx/grid.h"
#include "wx/socket.h"

#include "ra/racommon.h"
#include "ra/ragamepanel.h"
#include "ra/rainfo.h"
#include "ra/raupdate.h"
#include "ra/raconfig.h"
#include "ra/radlgabout.h"
#include "ra/radlgprefs.h"
#include "ra/radlgrules.h"


enum
{
	raID_NEW_GAME = 10000,
	raID_EXIT,
	raID_PREFERENCES,
	raID_RULES,
	raID_BID_HISTORY,
	raID_LAST_TRICK,
	raID_HELP,
	raID_ABOUT
};

#define raSBAR_FIELDS 2

#define raLOG_FILE "ra_gui.log"
#define raGUI_XRS wxT("gui.xrs")

class raFrame;

// Declare the application class
class raApp : public wxApp
{
private:
	FILE *m_logfile;
	wxLogStderr *m_logger;
	wxLog *m_old_logger;
	raUpdate *m_update;
	raFrame *m_frame;
public:
	// Called on application startup
	virtual bool OnInit();
	virtual int OnRun();
	virtual int OnExit();
};

// Declare our main frame class
class raFrame : public wxFrame
{
public:
	// Constructor
	raFrame(const wxString& title);

	// Event handlers
	void OnQuit(wxCommandEvent& event);
	void OnAbout(wxCommandEvent& event);
	void OnGameNew(wxCommandEvent& event);
	void OnClose(wxCloseEvent& event);
	void OnPreferences(wxCommandEvent& event);
	void OnRules(wxCommandEvent& event);
	void OnAuction(wxCommandEvent& event);
	void OnLastTrick(wxCommandEvent& event);
	void OnSize(wxSizeEvent& event);
	void OnUpdate(raUpdateEvent& event);

private:
	wxSplitterWindow *m_split_main;//, *m_split_vert;
	raGamePanel *m_game;
	raInfo *m_info;
	bool ShowPreferences();
	bool ShowRules();
	//raUpdate *m_update;
	// This class handles events
	DECLARE_EVENT_TABLE()
};

// Implements raApp& GetApp()
DECLARE_APP(raApp)
#endif
