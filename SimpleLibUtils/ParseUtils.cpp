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

#include "ParseUtils.h"

namespace Simple
{


bool IsEOL(wchar_t ch)
{
	return ch==L'\r' || ch==L'\n';
}

bool SkipEOL(const wchar_t*& p)
{
	if (IsEOL(p[0]))
		{
		if (p[0]==L'\r') p++;
		if (p[0]==L'\n') p++;
		return true;
		}

	return false;
}

void SkipToEOL(const wchar_t*& p)
{
	while (p[0] && !IsEOL(p[0]))
		p++;
}

void SkipToNextLine(const wchar_t*& p)
{
	SkipToEOL(p);
	SkipEOL(p);
}

bool DoesMatch(const wchar_t* p, const wchar_t* psz, bool bEndWord)
{
	if (!psz)
		return false;

	size_t iLen=wcslen(psz);
	if (wcsncmp(p, psz, iLen)==0 && (!bEndWord || !IsIdentifierChar(p[iLen])))
		{
		return true;
		}
	return false;
}

bool SkipMatch(const wchar_t*& p, const wchar_t* psz, bool bEndWord)
{
	if (!psz)
		return false;

	size_t iLen=wcslen(psz);
	if (wcsncmp(p, psz, iLen)==0 && (!bEndWord || !IsIdentifierChar(p[iLen])))
		{
		p+=iLen;
		return true;
		}
	return false;
}

bool DoesMatchI(const wchar_t* p, const wchar_t* psz, bool bEndWord)
{
	if (!psz)
		return false;

	size_t iLen=wcslen(psz);
	if (_wcsnicmp(p, psz, iLen)==0 && (!bEndWord || !IsIdentifierChar(p[iLen])))
		{
		return true;
		}
	return false;
}

bool SkipMatchI(const wchar_t*& p, const wchar_t* psz, bool bEndWord)
{
	if (!psz)
		return false;

	size_t iLen=wcslen(psz);
	if (_wcsnicmp(p, psz, iLen)==0 && (!bEndWord || !IsIdentifierChar(p[iLen])))
		{
		p+=iLen;
		return true;
		}
	return false;
}

bool SkipWhiteSpace(const wchar_t*& p)
{
	if (!IsWhiteSpace(p[0]))
		return false;

	p++;
	while (IsWhiteSpace(p[0]))
		p++;

	return true;
}

/*
// Check if a character is a word char
bool IsWordChar(wchar_t ch)
{
	return (iswalnum(ch) || ch==L'_' || ch==L'[' || ch==L']');
}


bool CanWordBreakHere(const wchar_t* pszStart, int iPos)
{
	if (iPos==0)
		return true;

	if (!IsWordChar(pszStart[iPos]))
		return true;

	if (IsWordChar(pszStart[iPos-1]))
		return false;

	return true;
}
*/



bool IsLineEndChar(wchar_t ch)
{
	return ch==L'\r' || ch==L'\n';
}

bool IsLineSpace(wchar_t ch)
{
	return ch==' ' || ch=='\t';
}

bool IsWhiteSpace(wchar_t ch)
{
	return IsLineSpace(ch) || IsLineEndChar(ch);
}

bool IsIdentifierLeadChar(wchar_t ch, const wchar_t* pszExtraChars)
{
	if (pszExtraChars && wcschr(pszExtraChars, ch)!=NULL)
		return true;

	return (ch>='a' && ch<='z') || (ch>='A' && ch<='Z') || ch==L'_';
}

bool IsIdentifierChar(wchar_t ch, const wchar_t* pszExtraChars)
{
	if (pszExtraChars && wcschr(pszExtraChars, ch)!=NULL)
		return true;

	return (ch>='0' && ch<='9') || IsIdentifierLeadChar(ch);
}

bool SkipLineSpace(const wchar_t*& p)
{
	if (!IsLineSpace(p[0]))
		return false;

	p++;
	while (IsLineSpace(p[0]))
		p++;

	return true;
}

bool SkipLineEnd(const wchar_t*& p)
{
	if (!IsEOL(p[0]))
		return false;

	if (p[0]==L'\r')
		p++;
	if (p[0]==L'\n')
		p++;

	return true;
}

bool SkipIdentifier(const wchar_t*& p, CUniString& str, const wchar_t* pszExtraLeadChars, const wchar_t* pszExtraChars)
{
	// Store start
	const wchar_t* pszIdentifier=p;

	// Skip leading character
	if (!IsIdentifierLeadChar(p[0], pszExtraLeadChars))
		return false;
	p++;

	// Skip remaining characters
	while (true)
		{
		if (IsIdentifierChar(p[0], pszExtraChars))
			{
			p++;
			continue;
			}

		break;
		}

	// Setup return value
	str=CUniString(pszIdentifier, int(p-pszIdentifier));

	return true;
}

// Read an integer from an wchar_t buffer
bool ReadInt(const wchar_t*& p, int* pi)
{
	const wchar_t* pszOrig=p;

	bool bNegative=false;
	if (p[0]=='-')
	{
		p++;
		bNegative=true;
	}

	if (!iswdigit(p[0]))
	{
		p=pszOrig;
		return false;
	}

	*pi=0;
	while (iswdigit(p[0]))
		{
		*pi=(*pi)*10 + (p[0]-L'0');
		p++;
		}

	if (bNegative)
		pi[0]=-pi[0];

	return true;
}

bool ReadDoubleU(const wchar_t*& p, double* pdblVal)
{
	// Clear out values
	pdblVal[0]=0;

	// Must start with a digit or decimal
	if (!iswdigit(p[0]) && p[0]!='.')
		return false;

	// Parse the integer part
	while (iswdigit(p[0]))
	{
		pdblVal[0]=pdblVal[0] * 10 + (p[0]-'0');
		p++;
	}

	// Decimal part?
	if (p[0]=='.')
	{
		p++;
		double dblScale=0.1;
		while (iswdigit(p[0]))
		{
			pdblVal[0]=pdblVal[0] + dblScale * (p[0]-'0');
			p++;
			dblScale*=0.1;
		}
	}

	return true;
}

bool ReadDouble(const wchar_t*& psz, double* pdblVal)
{
	// Use local ptr
	const wchar_t* p=psz;

	// Handle negative
	bool bNegative=p[0]=='-';
	if (bNegative)
		p++;

	// Double?
	if (!ReadDoubleU(psz, pdblVal))
		return false;

	if (bNegative)
		pdblVal[0]*=-1.0;

	return true;
}






}