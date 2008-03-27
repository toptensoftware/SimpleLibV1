//////////////////////////////////////////////////////////////////////
//
// SimpleLib Utils Version 1.0
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
// StringSearch.cpp - implementation of StringSearch

#include "stdafx.h"
#include "SimpleLibUtilsBuild.h"

#include "StringSearch.h"


namespace Simple
{



// Search for one string in another
const wchar_t* StringSearch(const wchar_t* pszIn, const wchar_t* pszFind, bool bCaseSensitive)
{
	if (!pszIn || !pszFind)
		return NULL;

	// Get length of string we're looking for
	size_t iFindLen=pszFind ? wcslen(pszFind) : 0;

	// Empty find string?
	if (!iFindLen)
		{
		// Nothing to find!
		return NULL;
		}

	// Search the string...
	const wchar_t* p=pszIn;
	while (p[0])
		{
		bool bMatch=bCaseSensitive ? (wcsncmp(p, pszFind, iFindLen)==0) : (_wcsnicmp(p, pszFind, iFindLen)==0);
		if (bMatch)
			return p;
		p++;
		}

	return NULL;
}

const wchar_t* StringSearchRev(const wchar_t* pszIn, const wchar_t* pszFind, bool bCaseSensitive, int iStartAt)
{
	if (IsEmptyString(pszIn))
		return NULL;

	// Get length of string we're looking for
	size_t iFindLen=pszFind ? wcslen(pszFind) : 0;

	// Empty find string?
	if (!iFindLen)
		{
		// Nothing to find!
		return NULL;
		}

	// Work out where to start
	int iInLen=int(wcslen(pszIn));
	if (iStartAt<0)
		iStartAt=iInLen-1;
	if (iStartAt>=iInLen)
		return NULL;
	const wchar_t* p=pszIn+iStartAt;

	// Search the string...
	while (p>=pszIn)
		{
		bool bMatch=bCaseSensitive ? (wcsncmp(p, pszFind, iFindLen)==0) : (_wcsnicmp(p, pszFind, iFindLen)==0);
		if (bMatch)
			return p;
		p--;
		}

	return NULL;
}


}	// namespace Simple
