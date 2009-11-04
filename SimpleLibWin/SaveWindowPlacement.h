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
// SaveWindowPlacement.h - declaration of SaveWindowPlacement

#ifndef __SAVEWINDOWPLACEMENT_H
#define __SAVEWINDOWPLACEMENT_H


namespace Simple
{

// Save the placement of a frame window, using CuSetProfileXXX
// Use pOverride if you have a saved window placement that should be used instead of the current
// window placement (eg: for a main app window that's in full screen mode)
void SIMPLEAPI SaveWindowPlacement(HWND hWnd, CProfileSection* pSection, const wchar_t* pszPrefix=L"MainFrame", WINDOWPLACEMENT* pOverride=NULL);
void SIMPLEAPI SaveWindowPlacement(HWND hWnd, const wchar_t* pszSection=L"Window", const wchar_t* pszPrefix=L"MainFrame", WINDOWPLACEMENT* pOverride=NULL);

// Loads the window placement.  Returns a ShowWindow constant for showing the window
int SIMPLEAPI LoadWindowPlacement(HWND hWnd, CProfileSection* pSection, const wchar_t* pszPrefix=L"MainFrame");
int SIMPLEAPI LoadWindowPlacement(HWND hWnd, const wchar_t* pszSection=L"Window", const wchar_t* pszPrefix=L"MainFrame");


}	// namespace Simple

#endif	// __SAVEWINDOWPLACEMENT_H

