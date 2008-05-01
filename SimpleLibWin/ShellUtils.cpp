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
// ShellUtils.cpp - implementation of ShellUtils

#include "stdafx.h"
#include "SimpleLibWinBuild.h"

#include "ShellUtils.h"
#include <shlobj.h>
#include "FileUtils.h"

namespace Simple
{

#pragma comment(lib, "Shell32.lib")

HRESULT SIMPLEAPI GetSpecialFolderLocation(int i, const wchar_t* pszSubFolder, bool bCreate, CUniString& str)
{
	CAutoPtr<IMalloc, SRefCounted> spMalloc;
	SHGetMalloc(&spMalloc);

	LPITEMIDLIST pidl=NULL;
	SHGetSpecialFolderLocation(NULL, i, &pidl);

	TCHAR szSpecialFolder[MAX_PATH];
	SHGetPathFromIDList(pidl, szSpecialFolder);

	if (pszSubFolder)
		{
		str=SimplePathAppend(szSpecialFolder, pszSubFolder);

		if (bCreate)
			{
			RecursiveCreateDirectory(str);
			}
		}
	else
		{
		str=szSpecialFolder;
		}
		
	if (pidl)
		spMalloc->Free(pidl);

	return S_OK;
}


static int CALLBACK BrowseFolderCallback(HWND hWnd, UINT msg, LPARAM lParam, LPARAM lpData)
{
	if (msg==BFFM_INITIALIZED)
	{
		if (lpData)
			SendMessage(hWnd, BFFM_SETSELECTION, FALSE, lpData);
	}

	return 0;
}
						
LPITEMIDLIST SIMPLEAPI GetItemIDFromPath(const wchar_t* pszPath)
{
	if (!pszPath)
		return NULL;

	// NB: XP and later can use SHParseDisplayName()...

	CAutoPtr<IShellFolder, SRefCounted> spDesktop;
	::SHGetDesktopFolder(&spDesktop);
	if( !spDesktop )
		return NULL;

	HRESULT       hr;
	LPITEMIDLIST  pidl;
	ULONG         dwAttributes;
	CUniString		strPath(pszPath);
	hr = spDesktop->ParseDisplayName(NULL, NULL, strPath.GetBuffer(0), NULL, &pidl, &dwAttributes);
    
	if( FAILED(hr)) 
		return NULL;

	return pidl;
}


HRESULT SIMPLEAPI BrowseForFolder(HWND hWndParent, const wchar_t* pszTitle, const wchar_t* pszInitialDir, CUniString& str)
{
	CAutoPtr<IMalloc, SRefCounted> spMalloc;
	SHGetMalloc(&spMalloc);

	BROWSEINFO bi;
	ZeroMemory(&bi, sizeof(bi));
	bi.hwndOwner=hWndParent;
	bi.lpszTitle=pszTitle;
	bi.ulFlags = BIF_NEWDIALOGSTYLE|BIF_RETURNONLYFSDIRS;

	LPITEMIDLIST pidlInitial = GetItemIDFromPath(pszInitialDir);
	bi.lParam=(LPARAM)static_cast<LPCITEMIDLIST>(pidlInitial);
	bi.lpfn=BrowseFolderCallback;
	LPITEMIDLIST pidl=SHBrowseForFolder(&bi);

	if (pidlInitial)
		{
		spMalloc->Free(pidlInitial);
		}

	if (!pidl)
		return S_FALSE;

	// get the name of the folder
	HRESULT hr = E_FAIL;
    if (SHGetPathFromIDList( pidl, str.GetBuffer(MAX_PATH*2)))
    {
		hr = S_OK;
    }

	spMalloc->Free(pidl);

	return hr;
}



}	// namespace Simple
