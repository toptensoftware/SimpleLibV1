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
// StringReplace.h - declaration of StringReplace

#ifndef __STRINGREPLACE_H
#define __STRINGREPLACE_H

namespace Simple
{

CUniString SIMPLEAPI StringReplace(const wchar_t* pszIn, const wchar_t* pszFind, const wchar_t* pszReplace, bool bCaseSensitive, int iStartAt=0, int iMaxReplace=-1);

}	// namespace Simple

#endif	// __STRINGREPLACE_H

