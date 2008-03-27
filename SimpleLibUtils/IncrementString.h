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
// IncrementString.h - declaration of IncrementString

#ifndef __INCREMENTSTRING_H
#define __INCREMENTSTRING_H

namespace Simple
{

CUniString IncrementString(const wchar_t* psz);
CUniString IncrementStringEx(const wchar_t* psz, const wchar_t* pszDefAppend=L" (001)");
bool IncrementStringExTest(const wchar_t* psz);

}	// namespace Simple

#endif	// __INCREMENTSTRING_H

