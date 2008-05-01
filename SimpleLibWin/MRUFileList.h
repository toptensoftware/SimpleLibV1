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
// MRUFileList.h - declaration of MRUFileList

#ifndef __MRUFILELIST_H
#define __MRUFILELIST_H

#include "ProfileSettings.h"

namespace Simple
{

// MRU file list
class CMRUFileList : public CVector<CUniString, SCaseInsensitive>
{
public:
// Construction
			CMRUFileList();
	virtual ~CMRUFileList();

// Operations
	void Add(const wchar_t* pszPath);
	void Remove(const wchar_t* pszPath);
	void SetMaxSize(int iMax);

// Save/load
	void Save(const wchar_t* pszSubKey=L"RecentFiles", const wchar_t* pszKey=SlxGetProfileKey());
	void Load(const wchar_t* pszSubKey=L"RecentFiles", const wchar_t* pszKey=SlxGetProfileKey());

// Menu operations
	void UpdateMenu(HMENU hMenu, UINT nID, const wchar_t* pszEmpty=NULL, const wchar_t* pszBaseDir=NULL, bool bTitleOnly=false);

// Attributes
	int m_iMaxSize;
};


}	// namespace Simple

#endif	// __MRUFILELIST_H

