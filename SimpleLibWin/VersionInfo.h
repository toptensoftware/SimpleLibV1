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
// VersionInfo.h - declaration of version helper functions

#ifndef __VERSIONINFO_H
#define __VERSIONINFO_H


namespace Simple
{

bool IsVista();

bool GetModuleVersionInfo(const wchar_t* pszModuleName, VS_FIXEDFILEINFO* pInfo);
bool GetModuleVersionInfo(HMODULE hModule, VS_FIXEDFILEINFO* pInfo);
DWORD GetModuleVersion(const wchar_t* pszModuleName, CUniString& str);
DWORD GetModuleVersion(HMODULE hModule, CUniString& str);

// Helper to get dll versions
DWORD GetDllVersion(const wchar_t* pszDllName);
DWORD GetCommonControlsVersion();


}	// namespace Simple

#endif	// __VERSIONINFO_H

