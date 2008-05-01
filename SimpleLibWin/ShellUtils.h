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
// ShellUtils.h - declaration of ShellUtils

#ifndef __SHELLUTILS_H
#define __SHELLUTILS_H

namespace Simple
{

#pragma comment(lib, "Shell32.lib")

HRESULT SIMPLEAPI GetSpecialFolderLocation(int i, const wchar_t* pszSubFolder, bool bCreate, CUniString& str);
LPITEMIDLIST SIMPLEAPI GetItemIDFromPath(const wchar_t* pszPath);
HRESULT SIMPLEAPI BrowseForFolder(HWND hWndParent, const wchar_t* pszTitle, const wchar_t* pszInitialDir, CUniString& bstrPath);

}	// namespace Simple

#endif	// __SHELLUTILS_H

