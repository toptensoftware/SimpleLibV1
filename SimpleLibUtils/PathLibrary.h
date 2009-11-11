//////////////////////////////////////////////////////////////////////
//
// SimpleLib Utils Version 1.0
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
// PathLibrary.h - declaration of PathLibrary

#ifndef __PATHLIBRARY_H
#define __PATHLIBRARY_H

namespace Simple
{

bool SIMPLEAPI IsPathSeparator(wchar_t ch);
void SIMPLEAPI RemoveTrailingBackslash(wchar_t* pszPath);
CUniString SIMPLEAPI SimplePathAppend(const wchar_t* pszPath1, const wchar_t* pszPath2);
CUniString SIMPLEAPI CurrentDirectory();
CUniString SIMPLEAPI QualifyPath(const wchar_t* psz);
const wchar_t* SIMPLEAPI FindExtension(const wchar_t* pszPath);
void SIMPLEAPI RemoveExtension(wchar_t* pszPath);
const wchar_t* SIMPLEAPI FindLastElement(const wchar_t* pszPath, bool bIgnoreTrailingSlash);
void SIMPLEAPI RemoveLastElement(wchar_t* pszPath);
bool SIMPLEAPI SplitPath(const wchar_t* pszPath, CUniString* pstrFolder, CUniString* pstrFileName);
CUniString SIMPLEAPI ExtractFileTitle(const wchar_t* pszPath);
CUniString SIMPLEAPI CanonicalizePath(const wchar_t* pszPath);
CUniString SIMPLEAPI CanonicalPathAppend(const wchar_t* pszPath1, const wchar_t* pszPath2);
#ifdef _WIN32
const wchar_t* SIMPLEAPI FindEndOfDrive(const wchar_t* pszPath);
#endif
bool SIMPLEAPI IsFullyQualified(const wchar_t* pszPath);
CUniString SIMPLEAPI FindFileOnSearchPath(CVector<CUniString>& vecPath, const wchar_t* pszFileName);
CUniString SIMPLEAPI FindFileOnSearchPath(const wchar_t* pszSearchPath, const wchar_t* pszFileName);
bool SIMPLEAPI DoesFileExist(const wchar_t* pszFileName);
bool SIMPLEAPI DoesPathExist(const wchar_t* pszFileName);
CUniString SIMPLEAPI ChangeFileName(const wchar_t* pszFileName, const wchar_t* pszNewFileName);
CUniString SIMPLEAPI ChangeFileExtension(const wchar_t* pszFileName, const wchar_t* pszNewExtension);
#ifdef _WIN32
CUniString SIMPLEAPI ExtractDrive(const wchar_t* pszPath);
#endif
CUniString SIMPLEAPI FindRelativePath(const wchar_t* pszPath1, const wchar_t* pszPath2);
CUniString SIMPLEAPI ExtractFolder(const wchar_t* pszFileName);
CUniString SIMPLEAPI ExtractFileName(const wchar_t* pszFileName);

}	// namespace Simple

#endif	// __PATHLIBRARY_H

