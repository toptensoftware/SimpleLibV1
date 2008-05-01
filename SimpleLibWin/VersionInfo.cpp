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
// VersionInfo.cpp - implementation of version helpers

#include "stdafx.h"
#include "SimpleLibWinBuild.h"

#include "VersionInfo.h"

namespace Simple
{

#pragma comment(lib, "version.lib")

bool IsVista()
{
	static int iIsVista=-1;
	if (iIsVista<0)
	{
		OSVERSIONINFO osvi;
		memset(&osvi, 0, sizeof(osvi));
		osvi.dwOSVersionInfoSize=sizeof(osvi);
		GetVersionEx((OSVERSIONINFO*)&osvi);
		iIsVista=osvi.dwMajorVersion>=6;
	}
	return !!iIsVista;
}

bool SIMPLEAPI GetModuleVersionInfo(const wchar_t* pszModuleName, VS_FIXEDFILEINFO* pInfo)
{
	// Get verion info size
	DWORD dwHandle;
    DWORD cbVerInfo=GetFileVersionInfoSize(pszModuleName, &dwHandle);
	if (!cbVerInfo)
		return false;

	// Allocate buffer for version info
	LPVOID pVerInfo=_alloca(cbVerInfo);

	// Load version info
	if (!GetFileVersionInfo(pszModuleName, dwHandle, cbVerInfo, pVerInfo))
		return false;

	VS_FIXEDFILEINFO* pvi=NULL;
	UINT cbvi=0;
	if (!VerQueryValue(pVerInfo, L"\\", (void**)&pvi, &cbvi))
		return false;

	if (cbvi<sizeof(pInfo))
		return false;

	*pInfo=*pvi;
	return true;
}

bool SIMPLEAPI GetModuleVersionInfo(HMODULE hModule, VS_FIXEDFILEINFO* pInfo)
{
	// Get module file name
	wchar_t szFileName[MAX_PATH];
	GetModuleFileName(NULL, szFileName, MAX_PATH);

	return GetModuleVersionInfo(szFileName, pInfo);
}



DWORD SIMPLEAPI GetModuleVersion(const wchar_t* pszModuleName, CUniString& str)
{
	// Get verion info size
	DWORD dwHandle;
    DWORD cbVerInfo=GetFileVersionInfoSize(pszModuleName, &dwHandle);
	if (!cbVerInfo)
		return 0;

	// Allocate buffer for version info
	LPVOID pVerInfo=_alloca(cbVerInfo);

	// Load version info
	if (!GetFileVersionInfo(pszModuleName, dwHandle, cbVerInfo, pVerInfo))
		return 0;

	// Get translation info
	UINT cbTranslation=0;
	LPBYTE pTranslation=NULL;
	if (!VerQueryValue(pVerInfo, L"\\VarFileInfo\\Translation", (void**)&pTranslation, &cbTranslation))
		return 0;
	
	DWORD dwTrans=*(DWORD*)pTranslation;
	wchar_t szTrans[64];
	swprintf(szTrans, _countof(szTrans), L"%.2x%.2x%.2x%.2x", 
					HIBYTE(LOWORD(dwTrans)),
					LOBYTE(LOWORD(dwTrans)),
					HIBYTE(HIWORD(dwTrans)),
					LOBYTE(HIWORD(dwTrans))
					);

	wchar_t szValueName[MAX_PATH];


	UINT cb=0;
	const wchar_t* pszVersion=NULL;
	swprintf(szValueName, _countof(szValueName), L"\\StringFileInfo\\%s\\FileVersion", szTrans);
    if (!VerQueryValue(pVerInfo, szValueName, (void**)&pszVersion, &cb)) 
		return 0;

	str=pszVersion;
	return TRUE;
}

DWORD SIMPLEAPI GetModuleVersion(HMODULE hModule, CUniString& str)
{
	// Get module file name
	wchar_t szFileName[MAX_PATH];
	GetModuleFileName(hModule, szFileName, MAX_PATH);

	return GetModuleVersion(szFileName, str);
}

DWORD SIMPLEAPI GetDllVersion(const wchar_t* pszDllName)
{
	HINSTANCE hInstDll = GetModuleHandle(pszDllName);
	if (!hInstDll)
		return 0;

	DLLGETVERSIONPROC pDllGetVersion;
	pDllGetVersion = (DLLGETVERSIONPROC)GetProcAddress(hInstDll, "DllGetVersion");
	if (!pDllGetVersion)
		return 0;

	DLLVERSIONINFO dvi;
	HRESULT hr;

	ZeroMemory(&dvi, sizeof(dvi));
	dvi.cbSize = sizeof(dvi);

	hr = (*pDllGetVersion)(&dvi);
	if (FAILED(hr))
		return 0;

	return MAKELONG(dvi.dwMinorVersion, dvi.dwMajorVersion);
}

DWORD SIMPLEAPI GetCommonControlsVersion()
{
	static DWORD dwVer=0;
	if (!dwVer)
		dwVer=GetDllVersion(L"comctl32.dll");
	
	return dwVer;
}



}