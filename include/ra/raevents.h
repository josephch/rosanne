// Rosanne : Twenty Eight (28) Card Game
// Copyright (C) 2006-2009 Vipin Cherian
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

#ifndef _RAEVENTS_H
#define _RAEVENTS_H

#include "ra/racommon.h"

extern const wxEventType raINFO_EVT;
extern const wxEventType raBID_EVT;
extern const wxEventType raUPDATE_EVT;

class raInfoEvent : public wxEvent
{
	DECLARE_DYNAMIC_CLASS(raInfoEvent)
public:
	/** Default constructor */
	raInfoEvent();
	raInfoEvent(const raInfoEvent &evt);
	wxEvent *Clone(void) const { return new raInfoEvent(*this); }
	void SetCommand(int cmd);
	int GetCommand();
private:
	int m_cmd;

};

typedef void (wxEvtHandler::*raInfoEventFunction)(raInfoEvent&);

#define EVT_INFO(fn) \
	DECLARE_EVENT_TABLE_ENTRY( \
	raINFO_EVT, wxID_ANY, wxID_ANY, \
	(wxObjectEventFunction)(wxEventFunction)(raInfoEventFunction)&fn, \
	(wxObject *) NULL \
	),

class raBidEvent : public wxEvent
{
	DECLARE_DYNAMIC_CLASS(raBidEvent)
public:
	/** Default constructor */
	raBidEvent();
	raBidEvent(const raBidEvent &evt);
	wxEvent *Clone(void) const { return new raBidEvent(*this); }
	void SetBid(int bid);
	int GetBid();
private:
	int m_bid;

};

typedef void (wxEvtHandler::*raBidEventFunction)(raBidEvent&);

#define EVT_BID(fn) \
	DECLARE_EVENT_TABLE_ENTRY( \
	raBID_EVT, wxID_ANY, wxID_ANY, \
	(wxObjectEventFunction)(wxEventFunction)(raBidEventFunction)&fn, \
	(wxObject *) NULL \
	),
class raUpdateEvent : public wxEvent
{
	DECLARE_DYNAMIC_CLASS(raUpdateEvent)
public:
	/** Default constructor */
	raUpdateEvent();
	raUpdateEvent(const raUpdateEvent &evt);
	wxEvent *Clone(void) const { return new raUpdateEvent(*this); }
	void SetMessage(wxString msg);
	wxString GetMessage();
private:
	wxString m_msg;

};

typedef void (wxEvtHandler::*raUpdateEventFunction)(raUpdateEvent&);

#define EVT_UPDATE(fn) \
	DECLARE_EVENT_TABLE_ENTRY( \
	raUPDATE_EVT, wxID_ANY, wxID_ANY, \
	(wxObjectEventFunction)(wxEventFunction)(raUpdateEventFunction)&fn, \
	(wxObject *) NULL \
	),
#endif
