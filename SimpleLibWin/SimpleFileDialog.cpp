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
// SimpleFileDialog.cpp - implementation of SimpleFileDialog

#include "stdafx.h"
#include "SimpleLibWinBuild.h"

#include "SimpleFileDialog.h"
#include "RegistryUtils.h"
#include "FileUtils.h"

namespace Simple
{


#pragma comment(lib, "comdlg32.lib")

// Constructor
CSimpleFileDialog::CSimpleFileDialog(bool bSave, const wchar_t* pszFilter, const wchar_t* pszDefExt, const wchar_t* pszFileName, 
							DWORD dwFlags)
{
	// Initialise OPENFILENAME
#if (_WIN32_WINNT < 0x0500)
	int iExtra=IsPlacesBarSupported() ? EXTRA_SIZEOF_OFN : 0;
#else
	int iExtra=IsPlacesBarSupported() ? 0 : -EXTRA_SIZEOF_OFN;
#endif

	memset(&m_ofn, 0, sizeof(m_ofn)+iExtra);
	m_ofn.lStructSize=sizeof(m_ofn)+iExtra;
	m_ofn.lpstrFile=m_szFileName;
	m_ofn.nMaxFile=_countof(m_szFileName);
	m_ofn.Flags=dwFlags;
	m_ofn.lpstrDefExt=pszDefExt;

	// Setup filename
	lstrcpy(m_szFileName, pszFileName ? pszFileName : L"");
		
	// Store and fixup filter string
	m_pszFilter=pszFilter ? _wcsdup(pszFilter) : NULL;
	if (m_pszFilter)
		{
		LPTSTR p=m_pszFilter;
		while (p[0])
			{
			if (p[0]==L'|' || p[0]==L'\n')
				p[0]=L'\0';
			p++;
			}
		}
	m_ofn.lpstrFilter=m_pszFilter;

	// Save or load?
	m_bSave=bSave;
}

// Destructor
CSimpleFileDialog::~CSimpleFileDialog()
{
	if (m_pszFilter)
		free(m_pszFilter);
};

void CSimpleFileDialog::SetInitialDirKey(const wchar_t* pszKey, const wchar_t* pszValue, const wchar_t* pszDefault)
{
	// Store key
	m_strInitialDirKey=pszKey;
	m_strInitialDirValue=pszValue;

	// Get last folder
	CUniString strFolder;
	RegGetString(HKEY_CURRENT_USER, pszKey, pszValue, strFolder);

	// Is not found, use default
	if (IsEmptyString(strFolder))
		strFolder=pszDefault;

	// Set it
	if (!IsEmptyString(strFolder))
		SetInitialDir(strFolder);
}

void CSimpleFileDialog::SetInitialDir(const wchar_t* pszInitialDir)
{
	// Save initial dir
	m_strInitialDir=pszInitialDir;
}

int CSimpleFileDialog::DoModal(HWND hWndParent)
{
	// Setup parent window handle
	if (!hWndParent)
		hWndParent=GetActiveWindow();
	m_ofn.hwndOwner=hWndParent;

	// Setup initial directory
	if (!IsEmptyString(m_strInitialDir) && m_ofn.lpstrInitialDir==NULL)
		{
		if (!IsFullyQualified(m_szFileName))
			m_ofn.lpstrInitialDir=m_strInitialDir;
		}

	if (m_bSave)
		{
		if (!GetSaveFileName((OPENFILENAME*)&m_ofn))
			return IDCANCEL;
		}
	else
		{
		if (!GetOpenFileName((OPENFILENAME*)&m_ofn))
			return IDCANCEL;
		}

	// Save directory as initial directory for next time
	if (!IsEmptyString(m_strInitialDirKey))
		{
		// Start with folder name
		CUniString strFolder;	

		if (m_ofn.Flags & OFN_ALLOWMULTISELECT)
			{
			const wchar_t* pszFile=m_szFileName + m_ofn.nFileOffset;
			if (pszFile[lstrlen(pszFile)+1]==0)
				{
				// Split folder
				SplitPath(m_szFileName, &strFolder, NULL);
				}
			else
				{
				strFolder=m_szFileName;
				}
			}
		else
			{
			// Split folder
			SplitPath(m_szFileName, &strFolder, NULL);
			}
		
		RegSetString(HKEY_CURRENT_USER, m_strInitialDirKey, m_strInitialDirValue, strFolder);
		}


	return IDOK;
}


const wchar_t* CSimpleFileDialog::GetFilePath() 
{ 
	return m_szFileName; 
};

int CSimpleFileDialog::GetSelectedFiles(CVector<CUniString>& vec)
{
	ASSERT(m_ofn.Flags & OFN_ALLOWMULTISELECT);
	ASSERT(m_ofn.Flags & OFN_EXPLORER);

	// Start with folder name
	CUniString strFolder=m_szFileName;	

	const wchar_t* pszFile=m_szFileName + m_ofn.nFileOffset;

	if (pszFile[lstrlen(pszFile)+1]==0)
		{
		vec.Add(strFolder);
		}
	else
		{
		while (pszFile[0])
			{
			vec.Add(SimplePathAppend(strFolder, pszFile));
			pszFile+=lstrlen(pszFile)+1;
			}
		}

	return vec.GetSize();
}

bool CSimpleFileDialog::IsPlacesBarSupported()
{
	static int iSupported=-1;

	if (iSupported<0)
		{
		OSVERSIONINFO osvi;
		osvi.dwOSVersionInfoSize=sizeof(osvi);
		GetVersionEx(&osvi);

		iSupported=osvi.dwMajorVersion>=5;
		}

	return !!iSupported;
}




}	// namespace Simple
