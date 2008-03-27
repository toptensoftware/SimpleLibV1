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
// IncrementString.cpp - implementation of IncrementString

#include "stdafx.h"
#include "SimpleLibUtilsBuild.h"

#include "IncrementString.h"
#include "ParseUtils.h"

namespace Simple
{

CUniString IncrementString(const wchar_t* psz)
{
	if (IsEmptyString(psz))
	{
		return L"1";
	}

	// Find the last digit...
	const wchar_t* pszEnd=psz+wcslen(psz);
	while ( (pszEnd-1>=psz) && iswdigit(pszEnd[-1]))
		pszEnd--;

	// Have trailing number already
	if (pszEnd[0])
		{
		// Yes
		CUniString strPrefix(psz, int(pszEnd-psz));
		int i;
		ReadInt(pszEnd, &i);
		return Format(L"%s%i", strPrefix, i+1);
		}
	else
		{
		// No, append a "1"
		return Format(L"%s1", psz);
		}
}

CUniString IncrementStringEx(const wchar_t* psz, const wchar_t* pszDefAppend)
{
	if (IsEmptyString(psz))
		{
		return L"1";
		}

	// Find the last digit...
	const wchar_t* pszEnd=psz+wcslen(psz);

	// Skip trailing non digit characters
	while ( (pszEnd-1>=psz) && !iswdigit(pszEnd[-1]) && !iswalpha(pszEnd[-1]))
		pszEnd--;

	const wchar_t* pszTrailing=pszEnd;

	// Skip digits
	while ( (pszEnd-1>=psz) && iswdigit(pszEnd[-1]) )
		pszEnd--;

	// How many digits
	int iDigits=int(pszTrailing-pszEnd);

	CUniString strNumber;
	if (!isdigit(pszEnd[0]))
	{
		strNumber=pszDefAppend;
	}
	else
	{
		int i;
		const wchar_t* p=pszEnd;
		ReadInt(p, &i);

		strNumber=Format(L"%.*i", iDigits, i+1);
	}

	// Yes
	CUniString strPrefix(psz, int(pszEnd-psz));
	return Format(L"%s%s%s", strPrefix, strNumber, pszTrailing);
}


// Test if string contains a number
bool IncrementStringExTest(const wchar_t* psz)
{
	if (IsEmptyString(psz))
		{
		return false;
		}

	// Find the last digit...
	const wchar_t* pszEnd=psz+wcslen(psz);

	// Skip trailing non digit characters
	while ( (pszEnd-1>=psz) && !iswdigit(pszEnd[-1]) && !iswalpha(pszEnd[-1]))
		pszEnd--;

	const wchar_t* pszTrailing=pszEnd;

	// Skip digits
	while ( (pszEnd-1>=psz) && iswdigit(pszEnd[-1]) )
		pszEnd--;

	// How many digits
	int iDigits=int(pszTrailing-pszEnd);

	return iDigits>0;
}


}	// namespace Simple
