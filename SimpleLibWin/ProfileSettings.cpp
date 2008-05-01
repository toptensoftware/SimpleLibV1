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
// ProfileSettings.cpp - implementation of ProfileSettings

#include "stdafx.h"
#include "SimpleLibWinBuild.h"

#include "ProfileSettings.h"

#include "RegistryUtils.h"
#include "SmartHandle.h"


namespace Simple
{

CUniString  g_strCompanyName;
CUniString  g_strAppName;


const wchar_t* SIMPLEAPI SlxGetCompanyName()
{
	return g_strCompanyName;
}

const wchar_t* SIMPLEAPI SlxGetAppName()
{
	return g_strAppName;
}

void SIMPLEAPI SlxInitProfile(const wchar_t* pszCompanyName, const wchar_t* pszAppName)
{
	g_strCompanyName=pszCompanyName;
	g_strAppName=pszAppName;
	ASSERT(!IsEmptyString(g_strCompanyName));
	ASSERT(!IsEmptyString(g_strAppName));
}


bool SIMPLEAPI SlxUpgradeProfile(const wchar_t* pszOldCompanyName, const wchar_t* pszOldAppName)
{
	// Quit if key already exists
	CSmartHandle<HKEY> key;
	if (RegOpenKeyEx(HKEY_CURRENT_USER, SlxGetProfileKey(), 0, KEY_READ, &key)==ERROR_SUCCESS)
		{
		return false;
		}
	key.Release();

	// Copy from old key...
	return RegCopyKey(HKEY_CURRENT_USER, SlxGetProfileKey(),
						HKEY_CURRENT_USER, Format(L"Software\\%s\\%s", pszOldCompanyName, pszOldAppName))==ERROR_SUCCESS;
}

int SIMPLEAPI SlxGetProfileInt(const wchar_t* pszSection, const wchar_t* pszEntry, int nDefault)
{
	ASSERT(!IsEmptyString(g_strAppName));

	DWORD dw;
	if (RegGetDWORD(HKEY_CURRENT_USER, SlxGetProfileKey(pszSection), pszEntry, &dw)!=ERROR_SUCCESS)
		return nDefault;

	return int(dw);
}

bool SIMPLEAPI SlxSetProfileInt(const wchar_t* pszSection, const wchar_t* pszEntry, int nValue)
{
	ASSERT(!IsEmptyString(g_strAppName));

	CSmartHandle<HKEY> Key;
	if (RegCreateKey(HKEY_CURRENT_USER, Format(L"Software\\%s\\%s\\%s", g_strCompanyName, g_strAppName, pszSection), &Key)!=ERROR_SUCCESS)
		return false;

	RegSetValueEx(Key, pszEntry, 0, REG_DWORD, (BYTE*)&nValue, sizeof(nValue));
	return true;
}

CUniString SIMPLEAPI SlxGetProfileString(const wchar_t* pszSection, const wchar_t* pszEntry, const wchar_t* pszDefault)
{
	ASSERT(!IsEmptyString(g_strAppName));

	CUniString str;
	if (RegGetString(HKEY_CURRENT_USER, SlxGetProfileKey(pszSection), pszEntry, str)!=ERROR_SUCCESS)
		return pszDefault;

	return str;
}


bool SIMPLEAPI SlxSetProfileString(const wchar_t* pszSection, const wchar_t* pszEntry, const wchar_t* pszValue)
{
	ASSERT(!IsEmptyString(g_strAppName));

	CSmartHandle<HKEY> Key;
	if (RegCreateKey(HKEY_CURRENT_USER, Format(L"Software\\%s\\%s\\%s", g_strCompanyName, g_strAppName, pszSection), &Key)!=ERROR_SUCCESS)
		return false;

	pszValue=pszValue ? pszValue : L"";

	RegSetValueEx(Key, pszEntry, 0, REG_SZ, (BYTE*)pszValue, sizeof(wchar_t)*(lstrlen(pszValue)+1));
	return true;
}


void SIMPLEAPI SlxDeleteProfileSection(const wchar_t* pszSection)
{
	RegNukeKey(HKEY_CURRENT_USER, SlxGetProfileKey(pszSection));
}

bool SIMPLEAPI SlxEnumProfileValues(const wchar_t* pszSection, CVector<CUniString>& vec)
{
	return RegEnumAllValues(HKEY_CURRENT_USER, SlxGetProfileKey(pszSection), vec)==ERROR_SUCCESS;
}

bool SIMPLEAPI SlxEnumProfileSections(const wchar_t* pszSection, CVector<CUniString>& vec)
{
	return RegEnumAllKeys(HKEY_CURRENT_USER, SlxGetProfileKey(pszSection), vec)==ERROR_SUCCESS;
}



CUniString SIMPLEAPI SlxGetProfileKey(const wchar_t* pszSuffix)
{
	ASSERT(!g_strAppName.IsEmpty());
	if (pszSuffix)
		return Format(L"Software\\%s\\%s\\%s", g_strCompanyName, g_strAppName, pszSuffix);
	else
		return Format(L"Software\\%s\\%s", g_strCompanyName, g_strAppName);
}

}	// namespace Simple

