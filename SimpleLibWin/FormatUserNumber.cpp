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
// AutoSizeControl.cpp - implementation of AutoSizeControl

#include "stdafx.h"
#include "SimpleLibWinBuild.h"

#include "FormatUserNumber.h"

namespace Simple
{


CUniString FormatUserR8(double dbl, int iDP)
{
	BSTR bstr;

	VARIANT var;
	V_VT(&var)=VT_R8;
	V_R8(&var)=dbl;
	VarFormatNumber(&var, iDP, -2, -2, -2, 0, &bstr);


	CUniString str=bstr;
	SysFreeString(bstr);
	return str;
}

bool ParseUserR8(const wchar_t* psz, double* pVal)
{
	return SUCCEEDED(VarR8FromStr(psz, LOCALE_USER_DEFAULT, 0, pVal));
}

CUniString FormatUserI4(int iVal)
{
	BSTR bstr;
	VarBstrFromI4(iVal, LOCALE_USER_DEFAULT, 0, &bstr);
	CUniString str=bstr;
	SysFreeString(bstr);
	return str;
}

bool ParseUserI4(const wchar_t* psz, int* pVal)
{
	return SUCCEEDED(VarI4FromStr(psz, LOCALE_USER_DEFAULT, 0, (LONG*)pVal));
}

CUniString FormatUserUI4(unsigned int iVal)
{
	BSTR bstr;
	VarBstrFromUI4(iVal, LOCALE_USER_DEFAULT, 0, &bstr);
	CUniString str=bstr;
	SysFreeString(bstr);
	return str;
}

bool ParseUserUI4(const wchar_t* psz, unsigned int* pVal)
{
	return SUCCEEDED(VarUI4FromStr(psz, LOCALE_USER_DEFAULT, 0, (ULONG*)pVal));
}


}	// namespace Simple
