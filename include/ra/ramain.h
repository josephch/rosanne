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

#ifndef _RAMAIN_H_
#define _RAMAIN_H_

//#include "wx/wx.h"
#include "ra/racommon.h"
#include "wx/image.h" 
#include "wx/splitter.h"
#include "wx/grid.h"
#include "wx/socket.h"
#include "gg/ggpanel.h"
//#include "gg/ggcard.h"
//#include "ra/rainfo.h"
//#include "ra/ragame.h"
#include "ra/ra.h"
#include "ra/raupdate.h"
#include "ra/raconfig.h"
#include "ra/radlgabout.h"
#include "ra/radlgprefs.h"

enum 
{
	raID_NEW_GAME = 10000,
	raID_EXIT,
	raID_PREFERENCES,
	raID_HELP,
	raID_ABOUT
};

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
	void OnSize(wxSizeEvent& event);

private:
	wxSplitterWindow *m_split_main;//, *m_split_vert;
	raGame *m_game;
	raInfo *m_info;
	bool ShowPreferences();
	//raUpdate *m_update;
	// This class handles events
	DECLARE_EVENT_TABLE()
};

// Implements raApp& GetApp()
DECLARE_APP(raApp)
#endif
