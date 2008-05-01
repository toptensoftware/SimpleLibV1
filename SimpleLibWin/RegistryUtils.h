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
// RegistryUtils.h - helpers for working with registry

#ifndef __REGISTRYUTILS_H
#define __REGISTRYUTILS_H


namespace Simple
{

LONG SIMPLEAPI RegNukeKey(HKEY hKey, const wchar_t* pszSubKey);
LONG SIMPLEAPI RegGetString(HKEY hKey, const wchar_t* pszSubKey, const wchar_t* pszValue, CUniString& str);
LONG SIMPLEAPI RegSetString(HKEY hKey, const wchar_t* pszSubKey, const wchar_t* pszValueName, const wchar_t* pszValue);
LONG SIMPLEAPI RegGetDWORD(HKEY hKey, const wchar_t* pszSubKey, const wchar_t* pszValue, DWORD* pVal);
LONG SIMPLEAPI RegSetDWORD(HKEY hKey, const wchar_t* pszSubKey, const wchar_t* pszValueName, DWORD dwValue);
LONG SIMPLEAPI RegEnumAllValues(HKEY hKey, const wchar_t* pszSubKey, CVector<CUniString>& vec);
LONG SIMPLEAPI RegEnumAllKeys(HKEY hKey, const wchar_t* pszSubKey, CVector<CUniString>& vec);
LONG SIMPLEAPI RegCopyKey(HKEY hDestKey, const wchar_t* pszDestSubKey, HKEY hSrcKey, const wchar_t* pszSrcSubKey);

}	// namespace Simple

#endif	// __REGISTRYUTILS

