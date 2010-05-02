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
#include "ShellUtils.h"
#include "RegistryStream.h"
#include "ProfileStream.h"
#include "CopyStream.h"
#include "CallbackTimer.h"
#include "MessageBox.h"
#include <shlobj.h>

namespace Simple
{

CUniString SlxGetProfileFileName(const wchar_t* pszCompanyName, const wchar_t* pszAppName, bool bCreate)
{
	CUniString str;
	GetSpecialFolderLocation(CSIDL_APPDATA, Format(L"%s\\%s", pszCompanyName, pszAppName), bCreate, str);
	return SimplePathAppend(str, L"settings.ini");
}

static void _SlxConvertProfileRegKey(const wchar_t* pszRegKey, CProfileSection* pSection)
{
	// Open sub key
	CSmartHandle<HKEY> Key;
	if (RegOpenKeyEx(HKEY_CURRENT_USER, pszRegKey, 0, KEY_READ, &Key)==ERROR_SUCCESS)
	{

		// Enumerate all values
		DWORD dwIndex=0;
		TCHAR szName[MAX_PATH];
		DWORD cbName=MAX_PATH;
		DWORD dwType;
		while (RegEnumValue(Key, dwIndex++, szName, &cbName, NULL, &dwType, NULL, NULL)==ERROR_SUCCESS)
		{
			switch (dwType)
			{
				case REG_SZ:
				{
					CUniString str;
					if (RegGetString(Key, NULL, szName, str)==ERROR_SUCCESS)
					{
						pSection->SetValue(szName, str);
					}
					break;
				}

				case REG_DWORD:
				{
					DWORD dw;
					if (RegGetDWORD(Key, NULL, szName, &dw)==ERROR_SUCCESS)
					{
						pSection->SetIntValue(szName, dw);
					}
					break;
				}

				case REG_BINARY:
				{
					CAutoPtr<IStream, SRefCounted> spStreamSrc;
					if (SUCCEEDED(OpenRegistryStream(Key, NULL, szName, &spStreamSrc)))
					{
						CAutoPtr<IStream, SRefCounted> spStreamDest;
						if (SUCCEEDED(CreateProfileStream(pSection->CreateEntry(szName), NULL, &spStreamDest)))
						{
							CopyStream(spStreamDest, spStreamSrc);
						}
					}
					break;
				}
			}


			// Reset size
			cbName=MAX_PATH;
		}

		Key.Release();
	}

	// Copy sub sections
	CUniStringVector vecSubSections;
	RegEnumAllKeys(HKEY_CURRENT_USER, pszRegKey, vecSubSections);
	for (int i=0; i<vecSubSections.GetSize(); i++)
	{
		_SlxConvertProfileRegKey(Format(L"%s\\%s", pszRegKey, vecSubSections[i]), pSection->CreateSection(vecSubSections[i]));
	}

}


CProfileSettings::CProfileSettings()
{
	m_bModified=false;
}

CProfileSettings::~CProfileSettings()
{
	Flush();
}

void CProfileSettings::Init(const wchar_t* pszFileName)
{
	// Load
	m_bModified=true;

	// Load settings
	if (!CProfileFile::Load(pszFileName))
	{
		// Save immediately to attach file name with CProfileFile
		CProfileFile::Reset(false);
		CProfileFile::SetFileName(pszFileName);

		if (DoesFileExist(pszFileName))
		{
			SlxMessageBox(Format(L"Failed to load settings file '%s', resetting everything to default (%s)", pszFileName, CProfileFile::GetParseError()), MB_OK|MB_ICONWARNING);
			CProfileFile::Reset(false);
		}
	}

	m_bModified=false;
}

bool CProfileSettings::Upgrade(const wchar_t* pszFileName)
{
	// Quit if file already exists
	if (GetSize()!=0)
		return false;

	// Load old data
	CUniString strOldName=GetFileName();
	CProfileFile::Load(pszFileName);
	CProfileFile::SetFileName(strOldName);
	return true; 
}


void CALLBACK ProfileSettingsFlushProc(LPARAM lParam)
{
	CProfileSettings* pThis=(CProfileSettings*)lParam;
	pThis->Flush();
}


void CProfileSettings::SetModified()
{
	// Quit if already dirty
	if (m_bModified)
		return;

	m_bModified=true;

	if (m_hCallbackTimer==NULL)
	{
		m_hCallbackTimer=SetCallbackTimer(10, 0, ProfileSettingsFlushProc, (LPARAM)this);
	}
}

void CProfileSettings::Flush()
{
	if (m_hCallbackTimer)
	{
		KillCallbackTimer(m_hCallbackTimer);
		m_hCallbackTimer=NULL;
	}

	if (!m_bModified)
		return;

	Save(NULL, true, true);

	m_bModified=false;
}

void CProfileSettings::OnModified()
{
	SetModified();
}




static CUniString		g_strCompanyName;
static CUniString		g_strAppName;
static CProfileSettings	g_ProfileFile;
static bool				g_bFileBased=false;


const wchar_t* SIMPLEAPI SlxGetCompanyName()
{
	return g_strCompanyName;
}

const wchar_t* SIMPLEAPI SlxGetAppName()
{
	return g_strAppName;
}

void SIMPLEAPI SlxInitProfile(const wchar_t* pszCompanyName, const wchar_t* pszAppName, bool bFileBased)
{
	g_strCompanyName=pszCompanyName;
	g_strAppName=pszAppName;
	g_bFileBased=bFileBased;
	ASSERT(!IsEmptyString(g_strCompanyName));
	ASSERT(!IsEmptyString(g_strAppName));

	if (g_bFileBased)
	{
		// Work out profile file name
		CUniString strSettingsFile=SlxGetProfileFileName(pszCompanyName, pszAppName, true);

		// Load settings
		g_ProfileFile.Init(strSettingsFile);
	}
}


bool SIMPLEAPI SlxUpgradeProfile(const wchar_t* pszOldCompanyName, const wchar_t* pszOldAppName, bool bOldFileBased)
{
	if (!g_bFileBased)
	{
		// Can't convert from file based to registry based
		ASSERT(!bOldFileBased);

		// Quit if key already exists
		CSmartHandle<HKEY> key;
		if (RegOpenKeyEx(HKEY_CURRENT_USER, SlxGetProfileKey(), 0, KEY_READ, &key)==ERROR_SUCCESS)
			return false;

		key.Release();

		// Copy from old key...
		return RegCopyKey(HKEY_CURRENT_USER, SlxGetProfileKey(),
							HKEY_CURRENT_USER, Format(L"Software\\%s\\%s", pszOldCompanyName, pszOldAppName))==ERROR_SUCCESS;
	}
	else
	{
		// Quit if file already exists
		if (g_ProfileFile.GetSize()!=0)
			return false;

		if (!bOldFileBased)
		{
			// Enumerate registry, writing all values to file
			CUniStringVector vec;
			RegEnumAllKeys(HKEY_CURRENT_USER, Format(L"Software\\%s\\%s", pszOldCompanyName, pszOldAppName), vec);
			for (int i=0; i<vec.GetSize(); i++)
			{
				_SlxConvertProfileRegKey(Format(L"Software\\%s\\%s\\%s", pszOldCompanyName, pszOldAppName, vec[i]), g_ProfileFile.CreateSection(vec[i]));
			}

			// Delete registry if just "moving" from registry to file
			if (IsEqualString(g_strCompanyName, pszOldCompanyName) &&
				IsEqualString(g_strAppName, pszOldAppName) &&
				g_bFileBased)
			{
				// Flush file first
				SlxFlushProfileFile();

				// Nuke it
				RegNukeKey(HKEY_CURRENT_USER, Format(L"Software\\%s\\%s", pszOldCompanyName, pszOldAppName));
			}
		}
		else
		{
			// Load old data
			CUniString strOldName=g_ProfileFile.GetFileName();
			g_ProfileFile.Load(SlxGetProfileFileName(pszOldCompanyName, pszOldAppName, false));
			g_ProfileFile.SetFileName(strOldName);
		}

		return true;
	}

}

int SIMPLEAPI SlxGetProfileInt(const wchar_t* pszSection, const wchar_t* pszEntry, int nDefault)
{
	ASSERT(!IsEmptyString(g_strAppName));

	if (g_bFileBased)
	{
		return g_ProfileFile.GetIntValue(pszSection, pszEntry, nDefault);
	}
	else
	{
		DWORD dw;
		if (RegGetDWORD(HKEY_CURRENT_USER, SlxGetProfileKey(pszSection), pszEntry, &dw)!=ERROR_SUCCESS)
			return nDefault;
		return int(dw);
	}
}

bool SIMPLEAPI SlxSetProfileInt(const wchar_t* pszSection, const wchar_t* pszEntry, int nValue)
{
	ASSERT(!IsEmptyString(g_strAppName));

	if (g_bFileBased)
	{
		g_ProfileFile.SetIntValue(pszSection, pszEntry, nValue);
	}
	else
	{
		CSmartHandle<HKEY> Key;
		if (RegCreateKey(HKEY_CURRENT_USER, Format(L"Software\\%s\\%s\\%s", g_strCompanyName, g_strAppName, pszSection), &Key)!=ERROR_SUCCESS)
			return false;

		RegSetValueEx(Key, pszEntry, 0, REG_DWORD, (BYTE*)&nValue, sizeof(nValue));
	}
	return true;
}

CUniString SIMPLEAPI SlxGetProfileString(const wchar_t* pszSection, const wchar_t* pszEntry, const wchar_t* pszDefault)
{
	ASSERT(!IsEmptyString(g_strAppName));

	if (g_bFileBased)
	{
		return g_ProfileFile.GetValue(pszSection, pszEntry, pszDefault);
	}
	else
	{
		CUniString str;
		if (RegGetString(HKEY_CURRENT_USER, SlxGetProfileKey(pszSection), pszEntry, str)!=ERROR_SUCCESS)
			return pszDefault;
		return str;
	}

}


bool SIMPLEAPI SlxSetProfileString(const wchar_t* pszSection, const wchar_t* pszEntry, const wchar_t* pszValue)
{
	ASSERT(!IsEmptyString(g_strAppName));

	if (g_bFileBased)
	{
		g_ProfileFile.SetValue(pszSection, pszEntry, pszValue);
	}
	else
	{
		CSmartHandle<HKEY> Key;
		if (RegCreateKey(HKEY_CURRENT_USER, Format(L"Software\\%s\\%s\\%s", g_strCompanyName, g_strAppName, pszSection), &Key)!=ERROR_SUCCESS)
			return false;

		pszValue=pszValue ? pszValue : L"";

		RegSetValueEx(Key, pszEntry, 0, REG_SZ, (BYTE*)pszValue, sizeof(wchar_t)*(lstrlen(pszValue)+1));
	}
	return true;
}


void SIMPLEAPI SlxDeleteProfileSection(const wchar_t* pszSection)
{
	if (g_bFileBased)
	{
		g_ProfileFile.DeleteSection(pszSection);
	}
	else
	{
		RegNukeKey(HKEY_CURRENT_USER, SlxGetProfileKey(pszSection));
	}
}

void SIMPLEAPI SlxDeleteProfileValue(const wchar_t* pszSection, const wchar_t* pszEntry)
{
	if (g_bFileBased)
	{
		// Find section
		CProfileSection* pSection=g_ProfileFile.FindSection(pszSection);

		// If exists, delete value
		if (pSection)
			pSection->DeleteValue(pszEntry);
	}
	else
	{
		// Open key
		CSmartHandle<HKEY> Key;
		if (RegOpenKeyEx(HKEY_CURRENT_USER, Format(L"Software\\%s\\%s\\%s", g_strCompanyName, g_strAppName, pszSection), 0, KEY_READ|KEY_WRITE, &Key)!=ERROR_SUCCESS)
			return;

		// Delete value
		RegDeleteValue(Key, pszEntry);
	}
}


bool SIMPLEAPI SlxEnumProfileValues(const wchar_t* pszSection, CVector<CUniString>& vec)
{
	if (g_bFileBased)
	{
		CProfileSection* pSection=g_ProfileFile.FindSection(pszSection);
		if (pSection)
		{
			for (int i=0; i<pSection->GetSize(); i++)
			{
				vec.Add(pSection->GetAt(i)->GetName());
			}
		}
		return true;
	}
	else
	{
		CSmartHandle<HKEY> Key;
		if (RegCreateKey(HKEY_CURRENT_USER, Format(L"Software\\%s\\%s\\%s", g_strCompanyName, g_strAppName, pszSection), &Key)!=ERROR_SUCCESS)
			return false;

		return RegEnumAllValues(HKEY_CURRENT_USER, SlxGetProfileKey(pszSection), vec)==ERROR_SUCCESS;
	}
}

bool SIMPLEAPI SlxEnumProfileSections(const wchar_t* pszSection, CVector<CUniString>& vec)
{
	if (g_bFileBased)
	{
		CProfileSection* pSection=g_ProfileFile.FindSection(pszSection);
		for (int i=0; i<pSection->GetSubSectionCount(); i++)
		{
			vec.Add(pSection->GetSubSection(i)->GetName());
		}
		return true;
	}
	else
	{
		return RegEnumAllKeys(HKEY_CURRENT_USER, SlxGetProfileKey(pszSection), vec)==ERROR_SUCCESS;
	}
}



CUniString SIMPLEAPI SlxGetProfileKey(const wchar_t* pszSuffix)
{
	ASSERT(!g_strAppName.IsEmpty());
	ASSERT(!g_bFileBased);

	if (pszSuffix)
		return Format(L"Software\\%s\\%s\\%s", g_strCompanyName, g_strAppName, pszSuffix);
	else
		return Format(L"Software\\%s\\%s", g_strCompanyName, g_strAppName);
}

CProfileSection* SIMPLEAPI SlxGetProfileSection(const wchar_t* pszSection)
{
	ASSERT(g_bFileBased);
	return g_ProfileFile.FindSection(pszSection);
}

CProfileSection* SIMPLEAPI SlxCreateProfileSection(const wchar_t* pszSection)
{
	ASSERT(g_bFileBased);
	CProfileSection* pSection=g_ProfileFile.CreateSection(pszSection);
	return pSection;
}


void CALLBACK ProfileFlushProc(LPARAM lUnused)
{
	SlxFlushProfileFile();
}



bool SIMPLEAPI SlxFlushProfileFile()
{
	g_ProfileFile.Flush();
	return true;
}

CUniString SIMPLEAPI SlxGetProfileFileName()
{
	ASSERT(g_bFileBased);
	return g_ProfileFile.GetFileName();
}

CUniString SIMPLEAPI SlxGetProfileFolderName()
{
	ASSERT(g_bFileBased);
	CUniString str;
	SplitPath(g_ProfileFile.GetFileName(), &str, NULL);
	return str;
}

}	// namespace Simple


