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

const wchar_t* SIMPLEAPI SlxGetCompanyName();
const wchar_t* SIMPLEAPI SlxGetAppName();
void SIMPLEAPI SlxInitProfile(const wchar_t* pszCompanyName, const wchar_t* pszAppName=NULL, bool bFileBased=false);
bool SIMPLEAPI SlxUpgradeProfile(const wchar_t* pszOldCompanyName, const wchar_t* pszOldAppName, bool bOldFileBased=false);
int SIMPLEAPI SlxGetProfileInt(const wchar_t* pszSection, const wchar_t* pszEntry, int nDefault);
bool SIMPLEAPI SlxSetProfileInt(const wchar_t* pszSection, const wchar_t* pszEntry, int nValue);
CUniString SIMPLEAPI SlxGetProfileString(const wchar_t* pszSection, const wchar_t* pszEntry, const wchar_t* pszDefault);
bool SIMPLEAPI SlxSetProfileString(const wchar_t* pszSection, const wchar_t* pszEntry, const wchar_t* pszValue);
CUniString SIMPLEAPI SlxGetProfileKey(const wchar_t* pszSuffix=NULL);
void SIMPLEAPI SlxDeleteProfileSection(const wchar_t* pszSection);
void SIMPLEAPI SlxDeleteProfileValue(const wchar_t* pszSection, const wchar_t* pszEntry);
bool SIMPLEAPI SlxEnumProfileValues(const wchar_t* pszSection, CVector<CUniString>& vec);
bool SIMPLEAPI SlxEnumProfileSections(const wchar_t* pszSection, CVector<CUniString>& vec);
void SIMPLEAPI SlxProfileFileSetModified();
bool SIMPLEAPI SlxFlushProfileFile();
CProfileSection* SIMPLEAPI SlxGetProfileSection(const wchar_t* pszSection);
CProfileSection* SIMPLEAPI SlxCreateProfileSection(const wchar_t* pszSection);
CUniString SIMPLEAPI SlxGetProfileFileName();
CUniString SIMPLEAPI SlxGetProfileFolderName();

}	// namespace Simple

#endif	// __PROFILESETTINGS_H

