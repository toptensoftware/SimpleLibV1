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
// FindFiles.cpp - implementation of FindFiles

#include "stdafx.h"
#include "SimpleLibWinBuild.h"

#include "FindFiles.h"

namespace Simple
{
void SIMPLEAPI FindFiles(const wchar_t* pszSpec, CVector<CUniString>& Files, bool bFullPath)
{
	CUniString strFolder;
	SplitPath(pszSpec, &strFolder, NULL);

	WIN32_FIND_DATA fd;
	HANDLE hFind=FindFirstFile(pszSpec, &fd);
	while (hFind!=INVALID_HANDLE_VALUE)
	{
		if ((fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)==0)
		{
			if (bFullPath)
				Files.Add(SimplePathAppend(strFolder, fd.cFileName));
			else
				Files.Add(fd.cFileName);
		}

		if (!FindNextFile(hFind, &fd))
		{
			FindClose(hFind);
			hFind=INVALID_HANDLE_VALUE;
		}
	}
}

void SIMPLEAPI FindSubFolders(const wchar_t* pszFolder, CVector<CUniString>& Folders, bool bFullPath)
{
	WIN32_FIND_DATA fd;
	HANDLE hFind=FindFirstFile(SimplePathAppend(pszFolder, L"*.*"), &fd);
	while (hFind!=INVALID_HANDLE_VALUE)
	{
		if ((fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)!=0)
		{
			if (!IsEqualString(fd.cFileName, L".") && !IsEqualString(fd.cFileName, L".."))
			{
				if (bFullPath)
					Folders.Add(SimplePathAppend(pszFolder, fd.cFileName));
				else
					Folders.Add(fd.cFileName);
			}
		}

		if (!FindNextFile(hFind, &fd))
		{
			FindClose(hFind);
			hFind=INVALID_HANDLE_VALUE;
		}
	}
}

}	// namespace Simple
