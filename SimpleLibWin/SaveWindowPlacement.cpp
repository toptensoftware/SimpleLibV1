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

void SIMPLEAPI SaveWindowPlacement(HWND hWnd, CProfileSection* pSection, const wchar_t* pszPrefix, WINDOWPLACEMENT* pOverride)
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
	pSection->SetIntValue(Format(L"%sX", pszPrefix), wp.rcNormalPosition.left);
	pSection->SetIntValue(Format(L"%sY", pszPrefix), wp.rcNormalPosition.top);

	// Save size...
	if (GetWindowLong(hWnd, GWL_STYLE) & WS_THICKFRAME)
	{
		pSection->SetIntValue(Format(L"%sWidth", pszPrefix), Width(wp.rcNormalPosition));
		pSection->SetIntValue(Format(L"%sHeight", pszPrefix), Height(wp.rcNormalPosition));
	}

	// Save maximized
	if (GetWindowLong(hWnd, GWL_STYLE) & WS_MAXIMIZEBOX)
	{
		pSection->SetIntValue(Format(L"%sMaximized", pszPrefix), wp.showCmd==SW_SHOWMAXIMIZED);
	}
}

void SIMPLEAPI SaveWindowPlacement(HWND hWnd, const wchar_t* pszSection, const wchar_t* pszPrefix, WINDOWPLACEMENT* pOverride)
{
	SaveWindowPlacement(hWnd, SlxCreateProfileSection(pszSection), pszPrefix, pOverride);
}

int SIMPLEAPI LoadWindowPlacement(HWND hWnd, CProfileSection* pSection, const wchar_t* pszPrefix)
{
	// Get current placement
	WINDOWPLACEMENT wp;
	wp.length=sizeof(wp);
	GetWindowPlacement(hWnd, &wp);

	SIZE size=Size(wp.rcNormalPosition);

	if (!pSection)
		return SW_SHOWNORMAL;

	// Load position
	wp.rcNormalPosition.left=pSection->GetIntValue(Format(L"%sX", pszPrefix), -10000);
	wp.rcNormalPosition.top=pSection->GetIntValue(Format(L"%sY", pszPrefix), -10000);
	if (wp.rcNormalPosition.left<=-10000 || wp.rcNormalPosition.top<=-10000)
		return SW_SHOWNORMAL;

	// Load size
	if (GetWindowLong(hWnd, GWL_STYLE) & WS_THICKFRAME)
	{
		wp.rcNormalPosition.right=pSection->GetIntValue(Format(L"%sWidth", pszPrefix), -10000);
		wp.rcNormalPosition.bottom=pSection->GetIntValue(Format(L"%sHeight", pszPrefix), -10000);

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
		if (pSection->GetIntValue(Format(L"%sMaximized", pszPrefix), 0))
		{
			wp.showCmd=SW_SHOWMAXIMIZED;
		}
		else if (wp.showCmd==SW_SHOWMAXIMIZED)
		{
			wp.showCmd=SW_SHOWNORMAL;
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

int SIMPLEAPI LoadWindowPlacement(HWND hWnd, const wchar_t* pszSection, const wchar_t* pszPrefix)
{
	return LoadWindowPlacement(hWnd, SlxGetProfileSection(pszSection), pszPrefix);
}



}