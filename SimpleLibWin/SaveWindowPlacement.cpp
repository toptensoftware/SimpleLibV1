//////////////////////////////////////////////////////////////////////
//
// SimpleLib Win Version 1.0
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
// SaveWindowPlacement.cpp - implementation of SaveWindowPlacement

#include "stdafx.h"
#include "SimpleLibWinBuild.h"

#include "SaveWindowPlacement.h"
#include "ProfileSettings.h"
#include "GeoOps.h"

namespace Simple
{


void SIMPLEAPI SaveWindowPlacement(HWND hWnd, const wchar_t* pszSection, const wchar_t* pszPrefix, WINDOWPLACEMENT* pOverride)
{
	// Get placement
	WINDOWPLACEMENT wp;
	if (pOverride)
	{
		wp=*pOverride;
	}				
	else
	{
		wp.length=sizeof(wp);
		GetWindowPlacement(hWnd, &wp);
	}

	// Save position
	SlxSetProfileInt(pszSection, Format(L"%sX", pszPrefix), wp.rcNormalPosition.left);
	SlxSetProfileInt(pszSection, Format(L"%sY", pszPrefix), wp.rcNormalPosition.top);

	// Save size...
	if (GetWindowLong(hWnd, GWL_STYLE) & WS_THICKFRAME)
	{
		SlxSetProfileInt(pszSection, Format(L"%sWidth", pszPrefix), Width(wp.rcNormalPosition));
		SlxSetProfileInt(pszSection, Format(L"%sHeight", pszPrefix), Height(wp.rcNormalPosition));
	}

	// Save maximized
	if (GetWindowLong(hWnd, GWL_STYLE) & WS_MAXIMIZEBOX)
	{
		SlxSetProfileInt(pszSection, Format(L"%sMaximized", pszPrefix), wp.showCmd==SW_SHOWMAXIMIZED);
	}
}

int SIMPLEAPI LoadWindowPlacement(HWND hWnd, const wchar_t* pszSection, const wchar_t* pszPrefix)
{
	// Get current placement
	WINDOWPLACEMENT wp;
	wp.length=sizeof(wp);
	GetWindowPlacement(hWnd, &wp);

	SIZE size=Size(wp.rcNormalPosition);

	// Load position
	wp.rcNormalPosition.left=SlxGetProfileInt(pszSection, Format(L"%sX", pszPrefix), -10000);
	wp.rcNormalPosition.top=SlxGetProfileInt(pszSection, Format(L"%sY", pszPrefix), -10000);
	if (wp.rcNormalPosition.left<=-10000 || wp.rcNormalPosition.top<=-10000)
		return SW_SHOWNORMAL;

	// Load size
	if (GetWindowLong(hWnd, GWL_STYLE) & WS_THICKFRAME)
	{
		wp.rcNormalPosition.right=SlxGetProfileInt(pszSection, Format(L"%sWidth", pszPrefix), -10000);
		wp.rcNormalPosition.bottom=SlxGetProfileInt(pszSection, Format(L"%sHeight", pszPrefix), -10000);

		if (wp.rcNormalPosition.right<=-10000 || wp.rcNormalPosition.bottom<=-10000)
			return SW_SHOWNORMAL;
		wp.rcNormalPosition.right += wp.rcNormalPosition.left;
		wp.rcNormalPosition.bottom += wp.rcNormalPosition.top;
	}
	else
	{
		wp.rcNormalPosition.right=wp.rcNormalPosition.left + size.cx;
		wp.rcNormalPosition.bottom=wp.rcNormalPosition.top + size.cy;
	}

	// Save maximized
	if (GetWindowLong(hWnd, GWL_STYLE) & WS_MAXIMIZEBOX)
	{
		if (SlxGetProfileInt(pszSection, Format(L"%sMaximized", pszPrefix), 0))
		{
			wp.showCmd=SW_SHOWMAXIMIZED;
		}
	}

	// Only apply maximized if not visible
	int iRetv=wp.showCmd;
	if (!IsWindowVisible(hWnd))
	{
		wp.showCmd=SW_HIDE;
	}

	// Set window placement
	SetWindowPlacement(hWnd, &wp);

	// Return show command
	return iRetv;
}




}