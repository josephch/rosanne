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

#ifndef _RAUPDATE_H_
#define _RAUPDATE_H_

#include "ra/racommon.h"
#include "wx/thread.h"
//#include "wx/filesys.h"
//#include "wx/fs_inet.h"
#include "wx/protocol/http.h"
#include "wx/url.h"

#define raUPDATE_VER "1"
#define raUPDATE_URL "http://rosanne.sourceforge.net/ver.txt"
//#define raUPDATE_APPURL "http://rosanne.sourceforge.net"


class raUpdate : public wxThread
{
public:
	raUpdate();
	virtual void* Entry();
private:
	//wxString m_new_ver;
	//wxFSFile *m_f;
	//wxFileSystem *m_fs;
	int CheckForUpdate(wxString *new_ver = NULL);

};

#endif
