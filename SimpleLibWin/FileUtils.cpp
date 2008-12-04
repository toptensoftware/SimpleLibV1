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
// FileUtils.cpp - implementation of FileUtils

#include "stdafx.h"
#include "SimpleLibWinBuild.h"
#include "FileUtils.h"

namespace Simple
{

HRESULT SIMPLEAPI RecursiveCreateDirectory(const wchar_t* psz)
{
	// Check if exists
	DWORD dwAttribs=GetFileAttributes(psz);
	if (dwAttribs!=0xFFFFFFFF && (dwAttribs & FILE_ATTRIBUTE_DIRECTORY)!=0)
		return S_OK;

	// Create parent directory
	CUniString strParent(psz);
	RemoveLastElement(strParent.GetBuffer(0));
	if (strParent.IsEmpty())
		return HRESULT_FROM_WIN32(ERROR_PATH_NOT_FOUND);

	RETURNIFFAILED(RecursiveCreateDirectory(strParent));

	// Create it
	if (CreateDirectory(psz, NULL))
		return S_OK;

	return HRESULT_FROM_WIN32(GetLastError());
}



/*
// Find the end of the drive or UNC part of a path
const wchar_t* FindEndOfDrive(const wchar_t* pszPath)
{
	const wchar_t* p=pszPath;

	// Drive letter?
	if ( ((p[0]>='A' && p[0]<='Z') || (p[0]>='a' && p[0]<='z')) && p[1]==L':')
		return pszPath+2;

	// UNC Path?
	if (p[0]==L'\\' && p[1]==L'\\')
		{
		// Skip leading backslash
		p+=2;

		// Skip machine name
		while (p[0]!=L'\\' && p[0])
			p++;

		// Found it?
		if (p[0]==L'\\')
			{
			// Skip backslash
			p++;

			// Skip share name
			while (p[0]!=L'\\' && p[0])
				p++;

			// Done
			return p;
			}
		}

	// Not a drive or UNC
	return NULL;
}


// Check if path contains drive or UNC part
bool IsFullyQualified(const wchar_t* pszPath)
{
	return FindEndOfDrive(pszPath)!=NULL;
}
*/

// Get the system directory
CUniString SIMPLEAPI SlxGetSystemDirectory()
{
	OLECHAR sz[MAX_PATH];
	GetSystemDirectory(sz, MAX_PATH);
	return sz;
}

// Get the file name of a module
CUniString SIMPLEAPI SlxGetModuleFileName(HMODULE hModule)
{
	OLECHAR sz[MAX_PATH];
	GetModuleFileName(hModule, sz, MAX_PATH);
	return sz;
}

// Get the folder location of a module
CUniString SIMPLEAPI SlxGetModuleFolder(HMODULE hModule)
{
	CUniString str;
	SplitPath(SlxGetModuleFileName(hModule), &str, NULL);
	return str;
}



// Copy a folder recursively.
//   If bContinueOnError == true, will continue to copy if error encountered but will still return false
//   If bContinueOnError == false, will abort if an error is encountered, but anything copied thus far stays copied.
// Destination folder recursively created if necessary
// GetLastError() will return error code if bContinueOnError==false, otherwise undefined.
bool SIMPLEAPI CopyFolder(const wchar_t* pszDest, const wchar_t* pszSource, bool bContinueOnError, bool bFailIfExists)
{
	// Make sure target directory exists
	if (GetFileAttributes(pszDest)==0xFFFFFFFF)
	{
		// Create target directory
		if (FAILED(RecursiveCreateDirectory(pszDest)))
		{
			return false;
		}
	}

	// Enumerate everything in the source folder
	WIN32_FIND_DATA fd;
	HANDLE hFind=FindFirstFile(SimplePathAppend(pszSource, L"*.*"), &fd);
	bool bSuccess=true;
	while (hFind!=INVALID_HANDLE_VALUE)
	{
		// Is it a directory?
		if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			if (!IsEqualString(fd.cFileName, L".") && IsEqualString(fd.cFileName, L".."))
			{
				// Copy sub-folder
				if (!CopyFolder(SimplePathAppend(pszDest, fd.cFileName), SimplePathAppend(pszSource, fd.cFileName), bContinueOnError, bFailIfExists))
				{
					bSuccess=false;
					if (!bContinueOnError)
					{
						DWORD dwError=GetLastError();
						FindClose(hFind);
						SetLastError(dwError);
						return false;
					}
				}
			}
				
		}
		else
		{
			// Copy file
			if (!CopyFile(SimplePathAppend(pszSource, fd.cFileName), SimplePathAppend(pszDest, fd.cFileName), bFailIfExists))
			{
				bSuccess=false;
				if (!bContinueOnError)
				{
					DWORD dwError=GetLastError();
					FindClose(hFind);
					SetLastError(dwError);
					return false;
				}
			}
		}

		// Next file
		if (!FindNextFile(hFind, &fd))
		{
			FindClose(hFind);
			hFind=INVALID_HANDLE_VALUE;
		}
	}

	return bSuccess;
}



}	// namespace Simple
