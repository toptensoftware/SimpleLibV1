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
// SimpleFileDialog.h - declaration of A8SimpleFileDialog

#ifndef __SIMPLEFILEDIALOG_H
#define __SIMPLEFILEDIALOG_H

#include <CommDlg.h>

namespace Simple
{

// Number of extra bytes required in sizeof OPENFILENAME to
// get the new style common dialogs (ie: with "places bar" on left)
#define EXTRA_SIZEOF_OFN		12

class CSimpleFileDialog
{
public:
// Constructor
	CSimpleFileDialog(bool bSave, const wchar_t* pszFilter, const wchar_t* pszDefExt, const wchar_t* pszFileName=NULL, 
								DWORD dwFlags=OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT);
	~CSimpleFileDialog();

// Operations
	int DoModal(HWND hWndParent=NULL);
	static bool IsPlacesBarSupported();

	void SetInitialDirKey(const wchar_t* pszKey, const wchar_t* pszValue, const wchar_t* pszDefault);
	void SetInitialDir(const wchar_t* pszSection, const wchar_t* pszEntry, const wchar_t* pszDefault);
	void SetInitialDir(const wchar_t* pszInitialDir);

	const wchar_t* GetFilePath();

	int GetSelectedFiles(CVector<CUniString>& vec);

// Attributes
#if 0
	OPENFILENAME_NT4	m_ofn;
#else
	OPENFILENAME		m_ofn;
#endif
	BYTE			m_ofnExtra[EXTRA_SIZEOF_OFN];		// Extra memory for new style common dialog
	bool			m_bSave;
	wchar_t*		m_pszFilter;
	wchar_t			m_szFileName[MAX_PATH*10];
	bool			m_bInitialDirProfile;
	CUniString		m_strInitialDirKey;
	CUniString		m_strInitialDirValue;
	CUniString		m_strInitialDir;
};


}	// namespace Simple

#endif	// __SIMPLEFILEDIALOG_H

