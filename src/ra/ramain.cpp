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

#include "ra/ramain.h"
#include <time.h>

// Event table for raFrame
BEGIN_EVENT_TABLE(raFrame, wxFrame)
EVT_MENU(wxID_ABOUT, raFrame::OnAbout)
EVT_MENU(wxID_EXIT,  raFrame::OnQuit)
EVT_MENU(wxID_NEW,  raFrame::OnGameNew)
END_EVENT_TABLE()

// Give wxWidgets the means to create a raApp object
IMPLEMENT_APP(raApp)

// Initialize the application
bool raApp::OnInit()
{	
	m_logfile = fopen("ra_gui.log", "w+");

	m_logger = new wxLogStderr(m_logfile);
	wxASSERT(m_logger);

	m_old_logger = wxLog::GetActiveTarget();

	wxLog::SetActiveTarget(m_logger);
	wxLogDebug("Logging opened.");

	// Randomizing the PRNG
	srand(time(NULL));

	// Create the main application window
	//wxImage::AddHandler(new wxXPMHandler);
	wxImage::AddHandler(new wxPNGHandler);

	raFrame *frame = new raFrame(RA_APP_FULL_NAME);
	frame->SetSize(
		wxSystemSettings::GetMetric(wxSYS_SCREEN_X) - 500,
		wxSystemSettings::GetMetric(wxSYS_SCREEN_Y) - 300
		);
	// Show it
	frame->Show(true);

	// Start the event loop
	return true;
}

int raApp::OnExit()
{
	wxLogDebug("Attempting to stop logger.");

	wxLog::SetActiveTarget(m_old_logger);
	delete m_logger;
	fclose(m_logfile);

	return 0;
}

void raFrame::OnAbout(wxCommandEvent& event)
{
	wxString msg;
	msg.Append(RA_APP_FULL_NAME);
	msg.Append("\n\n - Copyright, Vipin Cherian 2006");
	// TODO : Add thanks in a respectable version
	//msg.Append(wxT("\n - Special thanks to Gigi, Kamjith,"));
	//msg.Append(wxT("\n     Aravind, Gaurav, Balji, Rajeev,"));
	//msg.Append(wxT("\n     Manish, Keya, Johns."));

	wxMessageBox(msg, wxT("About"),
		wxOK | wxICON_INFORMATION, this);
}

void raFrame::OnQuit(wxCommandEvent& event)
{
	// Destroy the frame
	Close();
}
void raFrame::OnGameNew(wxCommandEvent& event)
{
	m_game->NewGame();
}

//#include "mondrian.xpm"

raFrame::raFrame(const wxString& title) : wxFrame(NULL, wxID_ANY, title)
{
	// Set the frame icon
	//SetIcon(wxIcon(mondrian_xpm));
	wxBitmap tile;

	wxMenuBar *menu_bar = NULL;
	wxMenu *game_menu = NULL;
	wxMenu *help_menu = NULL;
	wxMenu *opt_menu = NULL;

	wxMenuItem *game_new = NULL;
	wxMenuItem *game_open = NULL;
	wxMenuItem *game_save = NULL;
	wxMenuItem *game_exit = NULL;

	wxMenuItem *opt_prefs = NULL;

	wxMenuItem *help_conts = NULL;
	wxMenuItem *help_about = NULL;

	game_menu = new wxMenu;
	help_menu = new wxMenu;
	opt_menu = new wxMenu;

	game_new = new wxMenuItem(game_menu, wxID_NEW, wxT("&New"));
	game_menu->Append(game_new);
	game_open = new wxMenuItem(game_menu, wxID_OPEN, wxT("&Open"));
	game_menu->Append(game_open);
	game_save = new wxMenuItem(game_menu, wxID_SAVE, wxT("&Save"));
	game_menu->Append(game_save);
	game_exit = new wxMenuItem(game_menu, wxID_EXIT, wxT("E&xit"));
	game_menu->Append(game_exit);

	opt_prefs = new wxMenuItem(opt_menu, wxID_PREFERENCES, wxT("&Preferences"));
	opt_menu->Append(opt_prefs);

	help_conts = new wxMenuItem(help_menu, wxID_HELP_CONTENTS, wxT("&Contents"));
	help_menu->Append(help_conts);
	help_about = new wxMenuItem(help_menu, wxID_ABOUT, wxT("&About"));
	help_menu->Append(help_about);

	// Now append the freshly created menu to the menu bar...
	menu_bar = new wxMenuBar();
	menu_bar->Append(game_menu, wxT("&Game"));
	menu_bar->Append(opt_menu, wxT("&Options"));
	menu_bar->Append(help_menu, wxT("&Help"));

	// ... and attach this menu bar to the frame
	SetMenuBar(menu_bar);

	game_open->Enable(false);
	game_save->Enable(false);
	opt_prefs->Enable(false);
	help_conts->Enable(false);

	// Create the main splitter control
	m_split_main = new wxSplitterWindow(this);
	//m_split_main->SetWindowStyle(m_split_main->GetWindowStyle() & ~wxSP_3D);
	m_split_main->Refresh();
	m_split_main->SetSashGravity(0.0);
	m_info = new raInfo(m_split_main);
	//m_info->SetWindowStyle(wxSUNKEN_BORDER );

	m_game = new raGame(m_split_main);
	tile.LoadFile("tile.bmp", wxBITMAP_TYPE_BMP);
	m_game->SetTile(&tile);
	//m_game->SetWindowStyle(wxSUNKEN_BORDER);

	m_game->SetInfoPanel(m_info);
	m_info->SetGamePanel(m_game);

	m_split_main->SplitVertically(m_info, m_game, 160);

	m_game->NewGame();

	// Create a status bar
	CreateStatusBar();
	//SetStatusText(wxT("Welcome to wxWidgets!"));
}
