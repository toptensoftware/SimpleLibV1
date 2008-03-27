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
// ParseUtils.cpp - implementation of ParseUtils

#include "stdafx.h"
#include "SimpleLibUtilsBuild.h"

#include "Wildcard.h"

namespace Simple
{

inline wchar_t towupper_fast(wchar_t ch)
{
	return (ch>='a' && ch<='z') ? (ch - 'a' + 'A') : ch;
}

// Do a wildcard match on two strings
bool DoesWildcardMatch(const wchar_t* pszSpec, const wchar_t* pszString)
{
	// Process the string vs spec...
	while (pszSpec[0] && pszString[0])
		{
		if (pszSpec[0]==L'*')
			{
			// Match wild card
			while (pszString[0])
				{
				// If matching dots then quit wildcard match
				if (pszString[0]=='.' && pszSpec[1]=='.')
					{
					if (DoesWildcardMatch(pszSpec, pszString+1))
						return true;

					pszString++;		// Skip the dot
					pszSpec++;			// Skip the dot
					break;
					}

				// Does RHS match?
				if (DoesWildcardMatch(pszSpec+1, pszString))
					return true;

				// Skip a character and try again
				pszString++;
				}

			// Skip the wildcard character
			pszSpec++;
			}
		else if (pszSpec[0]==L'?')
			{
			// Match any character
			pszSpec++;
			pszString++;
			}
		else
			{
			// Match specific character (case insensitive)
			if (towupper_fast(pszSpec[0])!=towupper_fast(pszString[0]))
				return false;

			pszSpec++;
			pszString++;
			}
		}

	// '.' at end allowed to match
	if (pszSpec[0]==L'.' && pszSpec[1]==L'\0' && pszString[0]==L'\0')
		return true;

	// '.*' at end allowed to match
	if (pszSpec[0]==L'.' && pszSpec[1]==L'*' && pszString[0]==L'\0')
		return true;

	// Wild card and end allowed to match
	if (pszSpec[0]==L'*')
		return true;

	// Both string finished?
	return !pszSpec[0] && !pszString[0];
}




}