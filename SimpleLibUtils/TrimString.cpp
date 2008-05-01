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
// TrimString.cpp - implementation of TrimString

#include "stdafx.h"
#include "SimpleLibUtilsBuild.h"

#include "TrimString.h"

namespace Simple
{

CUniString SIMPLEAPI LTrimString(const wchar_t* pszIn)
{
	// Skip leading whitespace
	while (iswspace(pszIn[0]))
		pszIn++;

	return pszIn;
}

CUniString SIMPLEAPI RTrimString(const wchar_t* pszIn)
{
	// Skip trailing whitespace
	const wchar_t* p=pszIn + wcslen(pszIn);
	while (p>pszIn && iswspace(p[-1]))
		p--;

	return CUniString(pszIn, int(p-pszIn));
}

CUniString SIMPLEAPI TrimString(const wchar_t* pszIn)
{
	// Skip leading whitespace
	while (iswspace(pszIn[0]))
		pszIn++;

	return RTrimString(pszIn);
}


}	// namespace Simple
