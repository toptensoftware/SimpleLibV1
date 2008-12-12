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
// ExpandVariablesEx.cpp - implementation of ExpandVariablesEx

#include "stdafx.h"
#include "SimpleLibWinBuild.h"

#include "ExpandVariables.h"
#include "ShellUtils.h"
#include "RegistryUtils.h"
#include <shlobj.h>

namespace Simple
{

/////////////////////////////////////////////////////////////////////////////
// CSpecialPathVariableProvider

struct CSIDL_MAP
{
	const wchar_t* psz;
	int csidl;
};
CSIDL_MAP csidl_map[] = 
{
	{ L"CSIDL_DESKTOP", CSIDL_DESKTOP },
	{ L"CSIDL_INTERNET", CSIDL_INTERNET },
	{ L"CSIDL_PROGRAMS", CSIDL_PROGRAMS },
	{ L"CSIDL_CONTROLS", CSIDL_CONTROLS },
	{ L"CSIDL_PRINTERS", CSIDL_PRINTERS },
	{ L"CSIDL_PERSONAL", CSIDL_PERSONAL },
	{ L"CSIDL_FAVORITES", CSIDL_FAVORITES },
	{ L"CSIDL_STARTUP", CSIDL_STARTUP },
	{ L"CSIDL_RECENT", CSIDL_RECENT },
	{ L"CSIDL_SENDTO", CSIDL_SENDTO },
	{ L"CSIDL_BITBUCKET", CSIDL_BITBUCKET },
	{ L"CSIDL_STARTMENU", CSIDL_STARTMENU },
	{ L"CSIDL_MYDOCUMENTS", CSIDL_MYDOCUMENTS },
	{ L"CSIDL_MYMUSIC", CSIDL_MYMUSIC },
	{ L"CSIDL_MYVIDEO", CSIDL_MYVIDEO },
	{ L"CSIDL_DESKTOPDIRECTORY", CSIDL_DESKTOPDIRECTORY },
	{ L"CSIDL_DRIVES", CSIDL_DRIVES },
	{ L"CSIDL_NETWORK", CSIDL_NETWORK },
	{ L"CSIDL_NETHOOD", CSIDL_NETHOOD },
	{ L"CSIDL_FONTS", CSIDL_FONTS },
	{ L"CSIDL_TEMPLATES", CSIDL_TEMPLATES },
	{ L"CSIDL_COMMON_STARTMENU", CSIDL_COMMON_STARTMENU },
	{ L"CSIDL_COMMON_PROGRAMS", CSIDL_COMMON_PROGRAMS },
	{ L"CSIDL_COMMON_STARTUP", CSIDL_COMMON_STARTUP },
	{ L"CSIDL_COMMON_DESKTOPDIRECTORY", CSIDL_COMMON_DESKTOPDIRECTORY },
	{ L"CSIDL_APPDATA", CSIDL_APPDATA },
	{ L"CSIDL_PRINTHOOD", CSIDL_PRINTHOOD },
	{ L"CSIDL_LOCAL_APPDATA", CSIDL_LOCAL_APPDATA },
	{ L"CSIDL_ALTSTARTUP", CSIDL_ALTSTARTUP },
	{ L"CSIDL_COMMON_ALTSTARTUP", CSIDL_COMMON_ALTSTARTUP },
	{ L"CSIDL_COMMON_FAVORITES", CSIDL_COMMON_FAVORITES },
	{ L"CSIDL_INTERNET_CACHE", CSIDL_INTERNET_CACHE },
	{ L"CSIDL_COOKIES", CSIDL_COOKIES },
	{ L"CSIDL_HISTORY", CSIDL_HISTORY },
	{ L"CSIDL_COMMON_APPDATA", CSIDL_COMMON_APPDATA },
	{ L"CSIDL_WINDOWS", CSIDL_WINDOWS },
	{ L"CSIDL_SYSTEM", CSIDL_SYSTEM },
	{ L"CSIDL_PROGRAM_FILES", CSIDL_PROGRAM_FILES },
	{ L"CSIDL_MYPICTURES", CSIDL_MYPICTURES },
	{ L"CSIDL_PROFILE", CSIDL_PROFILE },
	{ L"CSIDL_PROGRAM_FILES_COMMON", CSIDL_PROGRAM_FILES_COMMON },
	{ L"CSIDL_PROGRAM_FILES_COMMONX86", CSIDL_PROGRAM_FILES_COMMONX86 },
	{ L"CSIDL_COMMON_TEMPLATES", CSIDL_COMMON_TEMPLATES },
	{ L"CSIDL_COMMON_DOCUMENTS", CSIDL_COMMON_DOCUMENTS },
	{ L"CSIDL_COMMON_ADMINTOOLS", CSIDL_COMMON_ADMINTOOLS },
	{ L"CSIDL_ADMINTOOLS", CSIDL_ADMINTOOLS },
	{ L"CSIDL_CONNECTIONS", CSIDL_CONNECTIONS },
	{ L"CSIDL_COMMON_MUSIC", CSIDL_COMMON_MUSIC },
	{ L"CSIDL_COMMON_PICTURES", CSIDL_COMMON_PICTURES },
	{ L"CSIDL_COMMON_VIDEO", CSIDL_COMMON_VIDEO },
	{ L"CSIDL_RESOURCES", CSIDL_RESOURCES },
	{ L"CSIDL_RESOURCES_LOCALIZED", CSIDL_RESOURCES_LOCALIZED },
	{ L"CSIDL_CDBURN_AREA", CSIDL_CDBURN_AREA },
};

bool CSpecialPathVariableProvider::ResolveVariable(const wchar_t* psz, CUniString& str)
{
	int i;
	for (i=0; i<_countof(csidl_map); i++)
	{
		if (IsEqualStringI(csidl_map[i].psz, psz))
			break;
	}
	if (i==_countof(csidl_map))
		return false;

	return SUCCEEDED(GetSpecialFolderLocation(i, NULL, false, str));
}

CSpecialPathVariableProvider* CSpecialPathVariableProvider::GetInstance()
{
	static CSpecialPathVariableProvider instance;
	return &instance;
}


/////////////////////////////////////////////////////////////////////////////
// CEnvironmentVariableProvider

bool CEnvironmentVariableProvider::ResolveVariable(const wchar_t* psz, CUniString& str)
{
	if (!GetEnvironmentVariable(psz, str.GetBuffer(MAX_PATH), MAX_PATH))
	{
		return GetLastError()!=ERROR_ENVVAR_NOT_FOUND;
	}
	return true;
}

CEnvironmentVariableProvider* CEnvironmentVariableProvider::GetInstance()
{
	static CEnvironmentVariableProvider instance;
	return &instance;
}


/////////////////////////////////////////////////////////////////////////////
// CRegistryVariableProvider


bool CRegistryVariableProvider::ResolveVariable(const wchar_t* psz, CUniString& str)
{
	// Split root key from key
	CUniString strRootKey;
	CUniString strKeyAndValue;
	SplitString(psz, L"\\", strRootKey, strKeyAndValue);

	// Parse root key
	HKEY hKey=NULL;
	if (IsEqualString(strRootKey, L"HKLM"))
		hKey=HKEY_LOCAL_MACHINE;
	else if (IsEqualString(strRootKey, L"HKCU"))
		hKey=HKEY_CURRENT_USER;
	else if (IsEqualString(strRootKey, L"HKCR"))
		hKey=HKEY_CLASSES_ROOT;
	else if (IsEqualString(strRootKey, L"HKCC"))
		hKey=HKEY_CURRENT_CONFIG;
	else if (IsEqualString(strRootKey, L"HKU"))
		hKey=HKEY_USERS;
	else
		return false;

	// Split last value off
	CUniString strKeyName;
	CUniString strValueName;
	SplitPath(strKeyAndValue, &strKeyName, &strValueName);

	if (RegGetString(hKey, strKeyName, strValueName.IsEmpty() ? NULL : strValueName.sz(), str)!=ERROR_SUCCESS)
	{
		str=L"";
	}
	return true;
}

CRegistryVariableProvider* CRegistryVariableProvider::GetInstance()
{
	static CRegistryVariableProvider instance;
	return &instance;
}


/////////////////////////////////////////////////////////////////////////////
// CCompositeVariableProvider

void CCompositeVariableProvider::AddProvider(IVariableProvider* pProvider)
{
	m_Providers.Add(pProvider);
}


bool CCompositeVariableProvider::ResolveVariable(const wchar_t* psz, CUniString& str)
{
	for (int i=0; i<m_Providers.GetSize(); i++)
	{
		str.Empty();
		if (m_Providers[i]->ResolveVariable(psz, str))
			return true;
	}

	return false;
}



/////////////////////////////////////////////////////////////////////////////
// ExpandVariables

// Expands variables in a string of format $(variablename) by calling a provider
// to resolve the variable's value.
// If the variable can't be resolved the $(variablename) is left in the string unchanged.
CUniString ExpandVariables(const wchar_t* psz, IVariableProvider* pProvider)
{
	CUniString buf;
	const wchar_t* p=psz;
	while (p[0])
	{
		if (p[0]=='$' && p[1]=='(')
		{
			// Skip it
			p+=2;

			// Store start of var
			const wchar_t* pszVar=p;

			// Find end of var
			while (p[0] && p[0]!=')')
				p++;

			if (p[0]!=')')
			{
				buf+=L"$(";
				p=pszVar;
				continue;
			}

			// Extract variable name
			CUniString strVar(pszVar, p-pszVar);

			// Skip closing bracket
			p++;

			CUniString strValue;
			if (pProvider->ResolveVariable(strVar, strValue))
			{
				// Resolved
				buf+=strValue;
			}
			else
			{
				// Unknown!
				buf+=Format(L"$(%s)", strVar);
			}

		}
		else
		{
			buf+=*p++;
		}
	}

	return buf;
}


}	// namespace Simple
