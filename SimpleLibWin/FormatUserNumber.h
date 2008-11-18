//////////////////////////////////////////////////////////////////////
//
// SimpleLib Win Version 1.0
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
// FormatUserNumber.h - declaration of FormatUserNumber

#ifndef __FORMATUSERNUMBER_H
#define __FORMATUSERNUMBER_H

namespace Simple
{

CUniString FormatUserR8(double dbl, int iDP=-1);
bool ParseUserR8(const wchar_t* psz, double* pVal);
CUniString FormatUserI4(int iVal);
bool ParseUserI4(const wchar_t* psz, int* pVal);
CUniString FormatUserUI4(unsigned int iVal);
bool ParseUserUI4(const wchar_t* psz, unsigned int* pVal);


}	// namespace Simple

#endif	// __FORMATUSERNUMBER_H

