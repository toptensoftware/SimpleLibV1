//////////////////////////////////////////////////////////////////////
//
// SimpleLib ATL Win Version 1.0
//
// Copyright (C) 1998-2007 Topten Software.  All Rights Reserved
// http://www.toptensoftware.com
//
// This code has been released for use "as is".  Any redistribution or 
// modification however is strictly prohibited.   See the readme.txt file 
// for complete terms and conditions.
//
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// MdiWindow.cpp - implementation of MdiWindow

#include "stdafx.h"
#include "SimpleLibAtlWinBuild.h"

#include "MdiWindow.h"

namespace Simple
{

// Constructor
CMDIClientWindow::CMDIClientWindow()
{
}

// Destructor
CMDIClientWindow::~CMDIClientWindow()
{
}


bool CMDIClientWindow::Create(HWND hWndParent, HMENU hWindowMenu, UINT idFirstChild, UINT idClient)
{
	// Find window menu?
	if (!hWindowMenu)
	{
		hWindowMenu=FindWindowMenu(::GetMenu(hWndParent));
	}

	// Setup MDI create struct
    CLIENTCREATESTRUCT ccs; 
    ccs.hWindowMenu = hWindowMenu;
    ccs.idFirstChild = idFirstChild; 

    // Create the MDI client window. 
	if (!CWindow::Create(_T("mdiclient"), hWndParent, SRect(0,0,0,0), NULL,
				WS_VISIBLE |WS_CHILD | WS_CLIPCHILDREN | WS_VSCROLL | WS_HSCROLL, WS_EX_CLIENTEDGE, idClient, &ccs))
		return false;

	return true;
}


HMENU CMDIClientWindow::FindWindowMenu(HMENU hMainMenu)
{
	int iCount=GetMenuItemCount(hMainMenu);
	for (int i=iCount-1; i>=0; i--)
	{
		TCHAR szTemp[MAX_PATH];
		GetMenuString(hMainMenu, i, szTemp, MAX_PATH, MF_BYPOSITION);

		if (lstrcmpi(szTemp, _T("Window"))==0 || lstrcmpi(szTemp, _T("&Window"))==0)
			return GetSubMenu(hMainMenu, i);
	}

	return NULL;
}


bool CMDIClientWindow::TranslateAccelerator(MSG* pMsg)
{
   return !!TranslateMDISysAccel(m_hWnd, pMsg);
}

void CMDIClientWindow::MdiClose()
{
	HWND hWndActive=MdiGetActive();
	if (hWndActive)
	{
		::SendMessage(hWndActive, WM_SYSCOMMAND, SC_CLOSE, 0);
	}
}

void CMDIClientWindow::MdiCloseAll()
{
	HWND hWndActive;
	while (hWndActive=MdiGetActive())
	{
		if (hWndActive)
		{
			// Send close message
			::SendMessage(hWndActive, WM_SYSCOMMAND, SC_CLOSE, 0);

			// Quit if window didn't close 
			if (::IsWindow(hWndActive))
				return;
		}
	}
}

}	// namespace Simple
