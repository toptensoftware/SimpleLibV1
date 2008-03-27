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
// TrimString.h - declaration of TrimString

#ifndef __TRIMSTRING_H
#define __TRIMSTRING_H

namespace Simple
{

CUniString LTrimString(const wchar_t* pszIn);
CUniString RTrimString(const wchar_t* pszIn);
CUniString TrimString(const wchar_t* pszIn);

}	// namespace Simple

#endif	// __TRIMSTRING_H

