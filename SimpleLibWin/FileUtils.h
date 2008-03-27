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
// FileUtils.h - declaration of FileUtils

#ifndef __FILEUTILS_H
#define __FILEUTILS_H


namespace Simple
{

HRESULT RecursiveCreateDirectory(const wchar_t* psz);
//bool IsFullyQualified(const wchar_t* pszPath);
CUniString SlxGetSystemDirectory();
CUniString SlxGetModuleFileName(HMODULE hModule);
CUniString SlxGetModuleFolder(HMODULE hModule);

}	// namespace Simple

#endif	// __FILEUTILS_H

