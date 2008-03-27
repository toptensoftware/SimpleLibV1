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

bool IsPathSeparator(wchar_t ch);
void RemoveTrailingBackslash(wchar_t* pszPath);
CUniString SimplePathAppend(const wchar_t* pszPath1, const wchar_t* pszPath2);
CUniString QualifyPath(const wchar_t* psz);
const wchar_t* FindExtension(const wchar_t* pszPath);
void RemoveExtension(wchar_t* pszPath);
const wchar_t* FindLastElement(const wchar_t* pszPath);
void RemoveLastElement(wchar_t* pszPath);
bool SplitPath(const wchar_t* pszPath, CUniString* pstrFolder, CUniString* pstrFileName);
CUniString ExtractFileTitle(const wchar_t* pszPath);
CUniString CanonicalizePath(const wchar_t* pszPath);
CUniString CanonicalPathAppend(const wchar_t* pszPath1, const wchar_t* pszPath2);
const wchar_t* FindEndOfDrive(const wchar_t* pszPath);
bool IsFullyQualified(const wchar_t* pszPath);
CUniString FindFileOnSearchPath(CVector<CUniString>& vecPath, const wchar_t* pszFileName);
CUniString FindFileOnSearchPath(const wchar_t* pszSearchPath, const wchar_t* pszFileName);
bool DoesFileExist(const wchar_t* pszFileName);
bool DoesPathExist(const wchar_t* pszFileName);
CUniString ChangeFileName(const wchar_t* pszFileName, const wchar_t* pszNewFileName);
CUniString ChangeFileExtension(const wchar_t* pszFileName, const wchar_t* pszNewExtension);
CUniString ExtractDrive(const wchar_t* pszPath);
CUniString FindRelativePath(const wchar_t* pszPath1, const wchar_t* pszPath2);

}	// namespace Simple

#endif	// __PATHLIBRARY_H

