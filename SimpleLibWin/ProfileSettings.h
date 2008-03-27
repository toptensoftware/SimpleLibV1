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
// ProfileSettings.h - declaration of ProfileSettings

#ifndef __PROFILESETTINGS_H
#define __PROFILESETTINGS_H

namespace Simple
{

const wchar_t* SlxGetCompanyName();
const wchar_t* SlxGetAppName();
void SlxInitProfile(const wchar_t* pszCompanyName, const wchar_t* pszAppName=NULL);
bool SlxUpgradeProfile(const wchar_t* pszOldCompanyName, const wchar_t* pszOldAppName);
int SlxGetProfileInt(const wchar_t* pszSection, const wchar_t* pszEntry, int nDefault);
bool SlxSetProfileInt(const wchar_t* pszSection, const wchar_t* pszEntry, int nValue);
CUniString SlxGetProfileString(const wchar_t* pszSection, const wchar_t* pszEntry, const wchar_t* pszDefault);
bool SlxSetProfileString(const wchar_t* pszSection, const wchar_t* pszEntry, const wchar_t* pszValue);
CUniString SlxGetProfileKey(const wchar_t* pszSuffix=NULL);
void SlxDeleteProfileSection(const wchar_t* pszSection);
bool SlxEnumProfileValues(const wchar_t* pszSection, CVector<CUniString>& vec);
bool SlxEnumProfileSections(const wchar_t* pszSection, CVector<CUniString>& vec);

}	// namespace Simple

#endif	// __PROFILESETTINGS_H

