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
// RegistryUtils.cpp - implementation of registry utility functions

#include "stdafx.h"
#include "SimpleLibWinBuild.h"

#include "RegistryUtils.h"
#include "SmartHandle.h"


namespace Simple
{

#define HANDLEREGERR(x) { LONG l=(x); if (l!=ERROR_SUCCESS) return l; }

LONG RegNukeKey(HKEY hKey, const wchar_t* pszSubKey)
{
	// Open the key
	CSmartHandle<HKEY> Key;
	HANDLEREGERR(RegOpenKeyEx(hKey, pszSubKey, 0, KEY_READ|KEY_WRITE, &Key));

	DWORD dwSize = 256;
	wchar_t szBuffer[MAX_PATH];
	while (RegEnumKey(Key, 0, szBuffer, dwSize)==ERROR_SUCCESS)
	{
		HANDLEREGERR(RegNukeKey(Key, szBuffer));
		dwSize = 256;
	}

	Key.Release();
	return RegDeleteKey(hKey, pszSubKey);
}


LONG RegGetString(HKEY hKey, const wchar_t* pszKey, const wchar_t* pszValue, CUniString& str)
{
	// Open key
	CSmartHandle<HKEY> KeyTemp;
	if (pszKey)
		{
		HANDLEREGERR(RegOpenKeyEx(hKey, pszKey, 0, KEY_READ, &KeyTemp));
		hKey=KeyTemp;
		}

	// Get value size
	DWORD dwType;
	DWORD cbData=0;
	HANDLEREGERR(RegQueryValueEx(hKey, pszValue, NULL, &dwType, NULL, &cbData));

	// Is it a string
	if (dwType!=REG_SZ && dwType!=REG_EXPAND_SZ && dwType!=REG_MULTI_SZ)
		return ERROR_INVALID_DATA;

	// Get the actual value
	HANDLEREGERR(RegQueryValueEx(hKey, pszValue, NULL, NULL, (LPBYTE)str.GetBuffer(cbData/sizeof(wchar_t)+2), &cbData));

	// Done!
	return ERROR_SUCCESS;
}



LONG RegGetDWORD(HKEY hKey, const wchar_t* pszKey, const wchar_t* pszValue, DWORD* pVal)
{
	// Open key
	CSmartHandle<HKEY> KeyTemp;
	if (pszKey)
		{
		HANDLEREGERR(RegOpenKeyEx(hKey, pszKey, 0, KEY_READ, &KeyTemp));
		hKey=KeyTemp;
		}

	// Get value size
	DWORD dwType;
	DWORD dwData;
	DWORD cbData=sizeof(DWORD);
	HANDLEREGERR(RegQueryValueEx(hKey, pszValue, NULL, &dwType, (BYTE*)&dwData, &cbData));

	// Is it a string
	if (dwType!=REG_DWORD)
		return ERROR_INVALID_DATA;

	*pVal=dwData;
	return S_OK;
}


LONG RegSetString(HKEY hKey, const wchar_t* pszKey, const wchar_t* pszValueName, const wchar_t* pszValue)
{
	// Create key
	CSmartHandle<HKEY> KeyTemp;
	if (pszKey)
		{
		HANDLEREGERR(RegCreateKey(hKey, pszKey, &KeyTemp));
		hKey=KeyTemp;
		}

	pszValue=pszValue ? pszValue : L"";
	return RegSetValueEx(hKey, pszValueName, 0, REG_SZ, (BYTE*)pszValue, sizeof(wchar_t)*(lstrlen(pszValue)+1));
}



HRESULT RegSetDWORD(HKEY hKey, const wchar_t* pszKey, const wchar_t* pszValueName, DWORD dwValue)
{
	// Create key
	CSmartHandle<HKEY> KeyTemp;
	if (pszKey)
		{
		HANDLEREGERR(RegCreateKey(hKey, pszKey, &KeyTemp));
		hKey=KeyTemp;
		}

	return RegSetValueEx(hKey, pszValueName, 0, REG_DWORD, (BYTE*)&dwValue, sizeof(dwValue));
}





LONG RegEnumAllValues(HKEY hKey, const wchar_t* pszSubKey, CVector<CUniString>& vec)
{
	// Open key
	CSmartHandle<HKEY> KeyTemp;
	if (pszSubKey)
		{
		HANDLEREGERR(RegOpenKeyEx(hKey, pszSubKey, 0, KEY_READ, &KeyTemp));
		hKey=KeyTemp;
		}


	OLECHAR szValueName[MAX_PATH];
	DWORD cbValueName=_countof(szValueName);
	DWORD dwIndex=0;
	while (RegEnumValue(hKey, dwIndex++, szValueName, &cbValueName, NULL, NULL, NULL, NULL)==ERROR_SUCCESS)
	{
		vec.Add(szValueName);
		cbValueName=_countof(szValueName);
	}

	return ERROR_SUCCESS;	
}

LONG RegEnumAllKeys(HKEY hKey, const wchar_t* pszSubKey, CVector<CUniString>& vec)
{
	// Open key
	CSmartHandle<HKEY> KeyTemp;
	if (pszSubKey)
		{
		HANDLEREGERR(RegOpenKeyEx(hKey, pszSubKey, 0, KEY_READ, &KeyTemp));
		hKey=KeyTemp;
		}

	OLECHAR szName[MAX_PATH];
	DWORD dwIndex=0;
	while (RegEnumKey(hKey, dwIndex++, szName, MAX_PATH)==ERROR_SUCCESS)
	{
		vec.Add(szName);
	}

	return ERROR_SUCCESS;	
}

LONG RegCopyKey(HKEY hDestKey, const wchar_t* pszDestSubKey, HKEY hSrcKey, const wchar_t* pszSrcSubKey)
{
	// Open source key...
	CSmartHandle<HKEY> keySrc;
	HANDLEREGERR(RegOpenKeyEx(hSrcKey, pszSrcSubKey, 0, KEY_READ, &keySrc));

	// Create destination key
	CSmartHandle<HKEY> keyDest;
	HANDLEREGERR(RegCreateKey(hDestKey, pszDestSubKey, &keyDest));

	// Recursively copy sub keys
	DWORD dwIndex=0;
	TCHAR szKey[MAX_PATH];
	while (RegEnumKey(keySrc, dwIndex++, szKey, MAX_PATH)==ERROR_SUCCESS)
		{
		RegCopyKey(keyDest, szKey, keySrc, szKey);
		}

	// Now copy values
	dwIndex=0;
	TCHAR szValue[MAX_PATH];
	DWORD cbValue=_countof(szValue);
	DWORD dwType;
	BYTE bData[65536];
	DWORD cbData=sizeof(bData);
	while (RegEnumValue(keySrc, dwIndex++, szValue, &cbValue, NULL, &dwType, bData, &cbData)==ERROR_SUCCESS)
		{
		RegSetValueEx(keyDest, szValue, 0, dwType, bData, cbData);
		cbData=sizeof(bData);
		cbValue=_countof(szValue);
		}

	return ERROR_SUCCESS;
}


}	// namespace Simple
