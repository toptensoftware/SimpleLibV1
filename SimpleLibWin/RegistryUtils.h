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

LONG RegNukeKey(HKEY hKey, const wchar_t* pszSubKey);
LONG RegGetString(HKEY hKey, const wchar_t* pszSubKey, const wchar_t* pszValue, CUniString& str);
LONG RegSetString(HKEY hKey, const wchar_t* pszSubKey, const wchar_t* pszValueName, const wchar_t* pszValue);
LONG RegGetDWORD(HKEY hKey, const wchar_t* pszSubKey, const wchar_t* pszValue, DWORD* pVal);
LONG RegSetDWORD(HKEY hKey, const wchar_t* pszSubKey, const wchar_t* pszValueName, DWORD dwValue);
LONG RegEnumAllValues(HKEY hKey, const wchar_t* pszSubKey, CVector<CUniString>& vec);
LONG RegEnumAllKeys(HKEY hKey, const wchar_t* pszSubKey, CVector<CUniString>& vec);
LONG RegCopyKey(HKEY hDestKey, const wchar_t* pszDestSubKey, HKEY hSrcKey, const wchar_t* pszSrcSubKey);

}	// namespace Simple

#endif	// __REGISTRYUTILS

