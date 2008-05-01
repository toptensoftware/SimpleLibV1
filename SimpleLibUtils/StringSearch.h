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
// StringSearch.h - declaration of StringSearch

#ifndef __STRINGSEARCH_H
#define __STRINGSEARCH_H

namespace Simple
{

const wchar_t* SIMPLEAPI StringSearch(const wchar_t* pszIn, const wchar_t* pszFind, bool bCaseSensitive);
const wchar_t* SIMPLEAPI StringSearchRev(const wchar_t* pszIn, const wchar_t* pszFind, bool bCaseSensitive, int iStartAt=-1);

}	// namespace Simple

#endif	// __STRINGSEARCH_H

