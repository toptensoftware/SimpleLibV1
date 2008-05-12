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
#include <shlobj.h>

namespace Simple
{

static CUniString		g_strCompanyName;
static CUniString		g_strAppName;
static CProfileFile		g_ProfileFile;
static bool				g_bFileBased=false;
static bool				g_bProfileFileDirty=false;
static HCALLBACKTIMER	g_hCallbackTimer=NULL;

static class CAutoFlush
{
public:
	CAutoFlush()
	{
	};
	~CAutoFlush()
	{
		SlxFlushProfileFile();
	}
} g_AutoFlush;



const wchar_t* SIMPLEAPI SlxGetCompanyName()
{
	return g_strCompanyName;
}

const wchar_t* SIMPLEAPI SlxGetAppName()
{
	return g_strAppName;
}

CUniString _SlxGetProfileFileName(const wchar_t* pszCompanyName, const wchar_t* pszAppName, bool bCreate)
{
	CUniString str;
	GetSpecialFolderLocation(CSIDL_APPDATA, Format(L"%s\\%s", pszCompanyName, pszAppName), bCreate, str);
	return SimplePathAppend(str, L"settings.ini");
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
		CUniString strSettingsFile=_SlxGetProfileFileName(pszCompanyName, pszAppName, true);

		// Load settings
		if (!g_ProfileFile.Load(strSettingsFile))
		{
			// Save immediately to attach file name with CProfileFile
			g_ProfileFile.SetFileName(strSettingsFile);
		}

		// Not modified
		g_bProfileFileDirty=false;
	}
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

bool SIMPLEAPI SlxUpgradeProfile(const wchar_t* pszOldCompanyName, const wchar_t* pszOldAppName, bool bOldFileBased)
{
	if (!g_bFileBased)
	{
		// Can't convert from file based to registry based
		ASSERT(!bOldFileBased);

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
		}
		else
		{
			// Load old data
			CUniString strOldName=g_ProfileFile.GetFileName();
			g_ProfileFile.Load(_SlxGetProfileFileName(pszOldCompanyName, pszOldAppName, false));
			g_ProfileFile.SetFileName(strOldName);

		}


		// Save it
		SlxProfileFileSetModified();

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
		SlxProfileFileSetModified();
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
		SlxProfileFileSetModified();
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
		SlxProfileFileSetModified();
	}
	else
	{
		RegNukeKey(HKEY_CURRENT_USER, SlxGetProfileKey(pszSection));
	}
}

bool SIMPLEAPI SlxEnumProfileValues(const wchar_t* pszSection, CVector<CUniString>& vec)
{
	if (g_bFileBased)
	{
		CProfileSection* pSection=g_ProfileFile.FindSection(pszSection);
		for (int i=0; i<pSection->GetSize(); i++)
		{
			vec.Add(pSection->GetAt(i)->GetName());
		}
		return true;
	}
	else
	{
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

void CALLBACK ProfileFlushProc(LPARAM lUnused)
{
	SlxFlushProfileFile();
}


void SIMPLEAPI SlxProfileFileSetModified()
{
	// Quit if already dirty
	if (g_bProfileFileDirty)
		return;

	g_bProfileFileDirty=true;

	if (g_hCallbackTimer==NULL)
	{
		g_hCallbackTimer=SetCallbackTimer(10, 0, ProfileFlushProc, 0);
	}
}


bool SIMPLEAPI SlxFlushProfileFile()
{
	if (g_hCallbackTimer)
	{
		KillCallbackTimer(g_hCallbackTimer);
		g_hCallbackTimer=NULL;
	}

	if (!g_bFileBased || !g_bProfileFileDirty)
		return false;

	g_ProfileFile.Save(NULL, true, true);

	g_bProfileFileDirty=false;

	return true;
}

}	// namespace Simple


