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
// MessageBox.cpp - implementation of SlxMessageBox

#include "stdafx.h"
#include "SimpleLibWinBuild.h"

#include "MessageBox.h"
#include "LoadString.h"

#ifndef IDS_PROJNAME
#define IDS_PROJNAME	100
#endif

namespace Simple
{

static CUniString g_strAppName;
static SlxInteractive g_Interactive=slxInteractive;
static CUniString g_strLastError;


void SIMPLEAPI SlxMessageBoxSetAppName(const wchar_t* pszAppName)
{
	g_strAppName=pszAppName;
}

CUniString SIMPLEAPI SlxMessageBoxGetAppName()
{
	if (g_strAppName.IsEmpty())
		return LoadString(IDS_PROJNAME);
	else
		return g_strAppName;
}

int SIMPLEAPI SlxMessageBox(HWND hWndParent, const wchar_t* pszMessage, UINT nFlags, const wchar_t* pszCaption)
{
	if (g_Interactive!=slxInteractive)
	{
		if ((nFlags & 0x000000F0)==MB_ICONHAND)
			g_strLastError=pszMessage;

		if ((nFlags & MB_YESNOCANCEL) || (nFlags & MB_YESNO))
			return g_Interactive==slxNonInteractiveSaveChanges ? IDYES : IDNO;

		return IDOK;
	}

	CUniString strAppName;
	if (!pszCaption || pszCaption[0]==L'\0')
	{
		strAppName=SlxMessageBoxGetAppName();
		pszCaption=strAppName;
	}

	return MessageBox(hWndParent, pszMessage, pszCaption, nFlags);
}

int SIMPLEAPI SlxMessageBox(const wchar_t* pszMessage, UINT nFlags, const wchar_t* pszCaption)
{
	return SlxMessageBox(GetActiveWindow(), pszMessage, nFlags, pszCaption);
}

int SIMPLEAPI SlxMessageBox(UINT nResID, UINT nFlags, const wchar_t* pszCaption)
{	
	return SlxMessageBox(GetActiveWindow(), LoadString(nResID), nFlags, pszCaption);
}


SlxInteractive SlxGetInteractive()
{
	return g_Interactive;
}


void SlxSetInteractive(SlxInteractive newVal)
{
	g_Interactive=newVal;
	g_strLastError.Empty();
}

CUniString SlxGetNonInteractiveError()
{
	return g_strLastError;
}



}	// namespace Simple
