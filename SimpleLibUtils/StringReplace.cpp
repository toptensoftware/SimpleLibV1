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
// StringReplace.cpp - implementation of StringReplace

#include "StdAfx.h"
#include "SimpleLibUtilsBuild.h"

#include "StringReplace.h"


namespace Simple
{


// String replace helper
CUniString SIMPLEAPI StringReplace(const wchar_t* pszIn, const wchar_t* pszFind, const wchar_t* pszReplace, bool bCaseSensitive, int iStartAt, int iMaxReplace)
{
	// NULL search string
	if (!pszIn)
		return NULL;

	// Get length of string we're looking for
	size_t iFindLen=pszFind ? wcslen(pszFind) : 0;

	// Empty find string?
	if (!iFindLen || iStartAt>=(int)wcslen(pszIn))
		return pszIn;

	// NULL replace string?
	if (!pszReplace)
		pszReplace=L"";


	CUniString buf;
	if (iStartAt>0)
		buf.Append(pszIn, iStartAt);
	const wchar_t* p=pszIn+iStartAt;
	const wchar_t* pszStart=p;
	bool bMatches=false;
	while (p[0] && iMaxReplace)
		{
		bool bMatch=bCaseSensitive ? (wcsncmp(p, pszFind, iFindLen)==0) : (_wcsnicmp(p, pszFind, iFindLen)==0);
		if (bMatch)
			{
			// Append everything before the match
			if (p>pszStart)
				buf.Append(pszStart, int(p-pszStart));
			buf.Append(pszReplace);
			p+=iFindLen;
			pszStart=p;
			bMatches=true;

			if (iMaxReplace>0)
				iMaxReplace--;
			}
		else
			{
			// Skip the character
			p++;
			}
		}

	// Skip rest of string
	while (p[0])
		p++;

	// Append trailing stuff
	if (p>pszStart)
		buf.Append(pszStart, int(p-pszStart));

	return buf;
};



}	// namespace Simple
