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

int SIMPLEAPI SlxMessageBox(HWND hWndParent, const wchar_t* pszMessage, UINT nFlags, const wchar_t* pszCaption)
{
	CUniString strAppName;
	if (!pszCaption || pszCaption[0]==L'\0')
	{
		strAppName=LoadString(IDS_PROJNAME);
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






}	// namespace Simple
