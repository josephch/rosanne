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

#include "ra/ramain.h"
#include <time.h>
#include "main_icon_16.xpm"
#include "new_game.xpm"
#include "exit.xpm"
#include "options.xpm"
#include "rules.xpm"
#include "help.xpm"
#include "about.xpm"
#include "tile.xpm"
#include "bid_history.xpm"
#include "last_trick.xpm"

// Event table for raFrame
BEGIN_EVENT_TABLE(raFrame, wxFrame)
	EVT_MENU(raID_ABOUT, raFrame::OnAbout)
	EVT_MENU(raID_EXIT,  raFrame::OnQuit)
	EVT_MENU(raID_NEW_GAME,  raFrame::OnGameNew)
	EVT_MENU(raID_PREFERENCES,  raFrame::OnPreferences)
	EVT_MENU(raID_RULES,  raFrame::OnRules)
	EVT_MENU(raID_BID_HISTORY,  raFrame::OnAuction)
	EVT_MENU(raID_LAST_TRICK,  raFrame::OnLastTrick)
	EVT_CLOSE(raFrame::OnClose)
	EVT_SIZE(raFrame::OnSize)
END_EVENT_TABLE()

// Give wxWidgets the means to create a raApp object
IMPLEMENT_APP(raApp)

// Initialize the application
bool raApp::OnInit()
{	
	raConfig *config;
	raConfData conf_data;

	m_logfile = fopen("ra_gui.log", "w+");

	m_logger = new wxLogStderr(m_logfile);
	wxASSERT(m_logger);

	m_old_logger = wxLog::GetActiveTarget();

	wxLog::SetActiveTarget(m_logger);
	wxLogDebug("Logging opened.");

	// Obtain the configuration data
	config = raConfig::GetInstance();
	config->GetData(&conf_data);
		

	// Randomizing the PRNG
	srand(time(NULL));

	//wxImage::AddHandler(new wxXPMHandler);
	//wxImage::AddHandler(new wxPNGHandler);
	//For usage of sockets or derived classes such as wxFTP in a secondary thread
	wxSocketBase::Initialize();

	// Create the main application window
	m_frame = new raFrame(RA_APP_FULL_NAME);

	// Attempt to create the frame with the saved dimensions
	if(
		(conf_data.app_data.x != -1) &&
		(conf_data.app_data.y != -1) &&
		(conf_data.app_data.width != -1) &&
		(conf_data.app_data.height != -1) 
		)
	{
		m_frame->SetSize(wxRect(
			conf_data.app_data.x, 
			conf_data.app_data.y, 
			conf_data.app_data.width, 
			conf_data.app_data.height));
	}
	else
	{
		m_frame->SetSize(
			(wxSystemSettings::GetMetric(wxSYS_SCREEN_Y) * 3 / 4) + 120,
			wxSystemSettings::GetMetric(wxSYS_SCREEN_Y) * 3 / 4
			);
	}

	if(conf_data.app_data.maximized)
		m_frame->Maximize();

	SetTopWindow(m_frame);
	// Show it
	m_frame->Show(true);

	// Start the event loop
	return true;
}
int raApp::OnRun()
{
	// Check for updates
	wxXmlResource::Get()->InitAllHandlers();
	if(!wxXmlResource::Get()->Load("ra_all_dlgs.xrs"))
	{
		wxLogError(wxString::Format(wxT("Failed to load xrs %s. %s:%d"),GG_CARD_XRS,  __FILE__, __LINE__));
		return 1;
	}

	m_update = NULL;
	m_update = new raUpdate();
	if(!m_update)
	{
		wxLogError(wxString::Format(wxT("m_update = new raUpdate(); failed. %s:%d"), __FILE__, __LINE__));
		wxMessageBox(wxT("Failed to create an instance of the thread which checks for updates!"));
	}
	if (m_update->Create() != wxTHREAD_NO_ERROR )
	{
		wxLogError(wxString::Format(wxT("m_update->Create(). %s:%d"), __FILE__, __LINE__));
		wxMessageBox(wxT("Failed to create the thread which checks for updates!"));
	}
	m_update->Run();

	wxApp::OnRun();
	return 0;
}


int raApp::OnExit()
{
	// Save settings
	if(!raConfig::GetInstance()->Save())
	{
		wxLogError(wxString::Format(wxT("Attempt to save settings failed. %s:%d"), __FILE__, __LINE__));
	}
	
	// Stop logging
	wxLogDebug("Attempting to stop logger.");

	wxLog::SetActiveTarget(m_old_logger);
	delete m_logger;
	fclose(m_logfile);

	return 0;
}

void raFrame::OnAbout(wxCommandEvent& event)
{
	wxString msg;
	raDlgAbout about;
	msg.Append(RA_APP_FULL_NAME);
	msg.Append("\n\n - Copyright, Vipin Cherian 2006-2007");
	// TODO : Add thanks in a respectable version
	//msg.Append(wxT("\n - Special thanks to Gigi, Kamjith,"));
	//msg.Append(wxT("\n     Aravind, Gaurav, Balji, Rajeev,"));
	//msg.Append(wxT("\n     Manish, Keya, Johns."));

	//wxMessageBox(msg, wxT("About"),
	//	wxOK | wxICON_INFORMATION, this);
	if(!wxXmlResource::Get()->LoadDialog(&about, this, "ID_RADLGABOUT"))
	{
		wxLogError(wxString::Format(wxT("Attempt to save settings failed. %s:%d"), __FILE__, __LINE__));
	}
	about.ShowModal();
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

void raFrame::OnClose(wxCloseEvent& event)
{
	raConfig *config;
	raConfData conf_data;
	wxPoint pt;
	wxSize sz;

	// Get confirmation from the user before
	// closing the appliation
	if(wxMessageBox(wxT("Exit application?"), 
		wxT("Confirm"), wxYES_NO | wxICON_QUESTION) == wxNO)
	{
		event.Veto();
		return;
	}

	// Save the frame location and size
	// only if the window is not minimized or iconized
	if(!IsIconized())
	{
		config = raConfig::GetInstance();
		config->GetData(&conf_data);

		if(IsMaximized())
		{
			conf_data.app_data.maximized = true;
		}
		else
		{
			pt = wxPoint(0, 0);
			sz = wxSize(0, 0);
			pt = GetPosition();
			sz = GetSize();

			conf_data.app_data.x = pt.x;
			conf_data.app_data.y = pt.y;
			conf_data.app_data.width = sz.GetWidth();
			conf_data.app_data.height = sz.GetHeight();

			conf_data.app_data.maximized = false;
		}

		config->SetData(&conf_data);
	}

	event.Skip();
}

void raFrame::OnPreferences(wxCommandEvent& event)
{
	if(!ShowPreferences())
	{
		wxLogError(wxString::Format(wxT("ShowPreferences() failed. %s:%d"), __FILE__, __LINE__));
	}
	event.Skip();
}

void raFrame::OnRules(wxCommandEvent& event)
{
	if(!ShowRules())
	{
		wxLogError(wxString::Format(wxT("ShowRules() failed. %s:%d"), __FILE__, __LINE__));
	}
	event.Skip();
}

void raFrame::OnAuction(wxCommandEvent& event)
{
	if(!m_game->ShowAuction())
	{
		wxLogError(wxString::Format(wxT("ShowAuction() failed. %s:%d"), __FILE__, __LINE__));
	}
	event.Skip();
}

void raFrame::OnLastTrick(wxCommandEvent& event)
{
	if(!m_game->ShowLastTrick())
	{
		wxLogError(wxString::Format(wxT("ShowLastTricks() failed. %s:%d"), __FILE__, __LINE__));
	}
	event.Skip();
}

void raFrame::OnSize(wxSizeEvent& event)
{
	if(m_split_main)
	{
		m_split_main->Refresh();
		m_split_main->Update();
	}
	event.Skip();
}
bool raFrame::ShowPreferences()
{
	raDlgPrefs dlg_prefs;

	if(!wxXmlResource::Get()->LoadDialog(&dlg_prefs, this, "ID_RADLGPREFS"))
	{
		wxLogError(wxString::Format(wxT("Attempt to save settings failed. %s:%d"), __FILE__, __LINE__));
		return false;
	}
	dlg_prefs.ShowModal();

	if(!m_game->ReloadFromConfig())
	{
		wxLogError(wxString::Format(wxT("ReloadFromConfig failed. %s:%d"), __FILE__, __LINE__));
		return false;
	}
	return true;
}

bool raFrame::ShowRules()
{
	raDlgRules dlg_rules;

	if(!wxXmlResource::Get()->LoadDialog(&dlg_rules, this, "ID_RADLGRULES"))
	{
		wxLogError(wxString::Format(wxT("Attempt to save settings failed. %s:%d"), __FILE__, __LINE__));
		return false;
	}
	dlg_rules.ShowModal();

	if(!m_game->ReloadFromConfig())
	{
		wxLogError(wxString::Format(wxT("ReloadFromConfig failed. %s:%d"), __FILE__, __LINE__));
		return false;
	}
	return true;
}


//#include "mondrian.xpm"

raFrame::raFrame(const wxString& title) : wxFrame(NULL, wxID_ANY, title)
{
	wxBitmap tile(tile_xpm);
	wxBitmap bmp_new_game(new_game_xpm);
	wxBitmap bmp_exit(exit_xpm);
	wxBitmap bmp_options(options_xpm);
	wxBitmap bmp_rules(rules_xpm);
	wxBitmap bmp_bid_history(bid_history_xpm);
	wxBitmap bmp_last_trick(last_trick_xpm);
	wxBitmap bmp_help(help_xpm);
	wxBitmap bmp_about(about_xpm);


	wxMenuBar *menu_bar = NULL;
	wxMenu *game_menu = NULL;
	wxMenu *help_menu = NULL;
	wxMenu *opt_menu = NULL;
	wxMenu *view_menu = NULL;

	wxMenuItem *game_new = NULL;
	//wxMenuItem *game_open = NULL;
	//wxMenuItem *game_save = NULL;
	wxMenuItem *game_exit = NULL;

	wxMenuItem *opt_prefs = NULL;
	wxMenuItem *opt_rules = NULL;
	
	wxMenuItem *view_bid_history = NULL;
	wxMenuItem *view_last_trick = NULL;

	wxMenuItem *help_conts = NULL;
	wxMenuItem *help_about = NULL;

	wxToolBar *tool_bar;

	wxStatusBar *status_bar;

	m_split_main = NULL;

	// Set the frame icon
	SetIcon(wxIcon(main_icon_16_xpm));

	game_menu = new wxMenu;
	help_menu = new wxMenu;
	opt_menu = new wxMenu;
	view_menu = new wxMenu;

	game_new = new wxMenuItem(game_menu, raID_NEW_GAME, wxT("&New"));
	game_new->SetBitmap(bmp_new_game);
	game_menu->Append(game_new);
	//game_open = new wxMenuItem(game_menu, wxID_OPEN, wxT("&Open"));
	//game_menu->Append(game_open);
	//game_save = new wxMenuItem(game_menu, wxID_SAVE, wxT("&Save"));
	//game_menu->Append(game_save);
	game_exit = new wxMenuItem(game_menu, raID_EXIT, wxT("E&xit"));
	game_exit->SetBitmap(bmp_exit);
	game_menu->Append(game_exit);

	opt_prefs = new wxMenuItem(opt_menu, raID_PREFERENCES, wxT("&Preferences"));
	opt_prefs->SetBitmap(bmp_options);
	opt_menu->Append(opt_prefs);
	opt_rules = new wxMenuItem(opt_menu, raID_RULES, wxT("&Rules"));
	opt_rules->SetBitmap(bmp_rules);
	opt_menu->Append(opt_rules);

	view_bid_history = new wxMenuItem(view_menu, raID_BID_HISTORY, wxT("&Auction"));
	view_bid_history->SetBitmap(bmp_bid_history);
	view_menu->Append(view_bid_history);
	view_last_trick = new wxMenuItem(view_menu, raID_LAST_TRICK, wxT("&Last Trick"));
	view_last_trick->SetBitmap(bmp_last_trick);
	view_menu->Append(view_last_trick);

	help_conts = new wxMenuItem(help_menu, wxID_HELP_CONTENTS, wxT("&Contents"));
	help_conts->SetBitmap(bmp_help);
	help_menu->Append(help_conts);
	help_about = new wxMenuItem(help_menu, raID_ABOUT, wxT("&About"));
	help_about->SetBitmap(bmp_about);
	help_menu->Append(help_about);

	// Now append the freshly created menu to the menu bar...
	menu_bar = new wxMenuBar();
	menu_bar->Append(game_menu, wxT("&Game"));
	menu_bar->Append(opt_menu, wxT("&Options"));
	menu_bar->Append(view_menu, wxT("&View"));
	menu_bar->Append(help_menu, wxT("&Help"));

	// ... and attach this menu bar to the frame
	SetMenuBar(menu_bar);

	//game_open->Enable(false);
	//game_save->Enable(false);
	//opt_prefs->Enable(false);
	help_conts->Enable(false);

	// Create the Tool Bar
	tool_bar = new wxToolBar(this, wxID_ANY,
		wxDefaultPosition, wxDefaultSize, wxTB_HORIZONTAL|wxNO_BORDER|wxTB_FLAT);
	tool_bar->SetToolBitmapSize(wxSize(16,16));

	tool_bar->AddTool(raID_NEW_GAME, bmp_new_game, wxT("New Game"));
	tool_bar->AddTool(raID_EXIT, bmp_exit, wxT("Exit"));
	tool_bar->AddSeparator();
	tool_bar->AddTool(raID_PREFERENCES, bmp_options, wxT("Preferences"));
	tool_bar->AddTool(raID_RULES, bmp_rules, wxT("Rules"));
	tool_bar->AddSeparator();
	tool_bar->AddTool(raID_BID_HISTORY, bmp_bid_history, wxT("Auction"));
	tool_bar->AddTool(raID_LAST_TRICK, bmp_last_trick, wxT("Last Trick"));
	tool_bar->AddSeparator();
	tool_bar->AddTool(wxID_ANY, bmp_help, wxT("Help"));
	tool_bar->AddTool(raID_ABOUT, bmp_about, wxT("About"));
	tool_bar->Realize();
	this->SetToolBar(tool_bar);

	// Create the main splitter control
	m_split_main = new wxSplitterWindow(this);
	//m_split_main->SetWindowStyle(m_split_main->GetWindowStyle() & ~wxSP_3D);
	m_split_main->Refresh();
	m_split_main->SetSashGravity(0.0);
	m_info = new raInfo(m_split_main);
	//m_info->SetWindowStyle(wxSUNKEN_BORDER );

	m_game = new raGame(m_split_main);
	//tile.LoadFile("tile.bmp", wxBITMAP_TYPE_BMP);
	m_game->SetTile(&tile);
	//m_game->SetWindowStyle(wxSUNKEN_BORDER);

	m_game->SetInfoPanel(m_info);
	m_info->SetGamePanel(m_game);

	m_split_main->SplitVertically(m_info, m_game, 160);

	// Create a status bar
	status_bar = CreateStatusBar();
	status_bar->SetFieldsCount(raSBAR_FIELDS);
	int status_widths[raSBAR_FIELDS] = {-1, 200};
	status_bar->SetStatusWidths(raSBAR_FIELDS, status_widths);

	m_game->NewGame();
}
