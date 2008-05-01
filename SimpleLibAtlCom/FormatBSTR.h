//////////////////////////////////////////////////////////////////////
//
// SimpleLib ATL COM Version 1.0
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
// FormatBSTR.h - declaration of FormatBSTR

#ifndef __FORMATBSTR_H
#define __FORMATBSTR_H


namespace Simple
{

// Possibly elsewhere useful helpers required by FormatBSTR
void SIMPLEAPI CharString(OLECHAR ch, int iLen, BSTR* pVal);
void SIMPLEAPI FormatHex(DWORD dwValue, int iPrecision, bool bUpperCase, BSTR* pVal);

// Format into a BSTR!!
// Supports most format specifiers, doesn't require C runtime library!
// Alignment and width supported
// Precision supported, unless otherwise indicated

// Standard:
//		%c or %lc or %lC			unicode character
//		%C or %hC or %hc			ansi character
//		%s or %ls or %lS			unicode string
//		%S or %hS or %hs			ansi string
//		%u							unsigned int
//		%i							signed int	(precision weird if negative)
//		%hu							unsigned short
//		%hi							signed short (precision weird if negative)
//		%x							lower case hex (DWORD)
//		%X							upport case hex (DWORD)
//		%hx							lower case hex (WORD)
//		%hX							upport case hex (WORD)
//		%v for a variant			VariantChangeType(VT_BSTR)
//		%V for a variant			VarFormat, where subsequence param is format string
// 		%f = VarBstrFromR8/R4		(doesn't support precision)
//		%g = VarFormatNumber		(does support precision, also groups thousands)

// Plus: 
//		%+							If at very start of format string, will append existing string in *pVal
//		%y %Y						CURRENCY using VarFormatCurrency
//		%p %P						percentage using VarFormatPercentage
//		%t							short time (pass a double) (precision ignored)
//		%T							long time (pass a double) (precision ignored)
//		%j							short date (pass a double) (precision ignored)
//		%J							long date (pass a double) (precision ignored)
//		%E							HRESULT to string with GetComErrorMessage

//		NB: No octal support (SO WHAT!!!)

// If on enter, pVal points to an existing string it will be SysFreeString'd

int SIMPLEAPI FormatBSTR(CUniString& buf, LPCOLESTR pszFormat, va_list args);
int SIMPLEAPI FormatBSTR(BSTR* pVal, LPCOLESTR pszFormat, va_list args);
int SIMPLEAPI FormatBSTR(BSTR* pVal, LPCOLESTR pszFormat, ...);
CComBSTR SIMPLEAPI FormatBSTR(LPCOLESTR pszFormat, ...);

int SIMPLEAPI FormatBSTR(BSTR* pVal, UINT nResIDFormat, va_list args);
int SIMPLEAPI FormatBSTR(BSTR* pVal, UINT nResIDFormat, ...);
CComBSTR SIMPLEAPI FormatBSTR(UINT nResIDFormat, ...);

}	// namespace Simple

#endif	// __FORMATBSTR_H

