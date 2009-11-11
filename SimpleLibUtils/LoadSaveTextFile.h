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
// LoadSaveTextFile.h - declaration of SaveTextFile

#ifndef __LOADSAVETEXTFILE_H
#define __LOADSAVETEXTFILE_H

#include "File.h"

namespace Simple
{
#ifdef _MSC_VER
#pragma warning(disable:4996)
#endif

#ifdef _MSC_VER
#pragma warning(default:4996)
#endif



template <class T>
result_t LoadText(IFile* pFile, CString<T>& str)
{
	int iLen=(int)pFile->GetLength();

	// Read first two bytes
	wchar_t prefix;
	ReturnIfFailed(pFile->Read(&prefix, sizeof(prefix), NULL));

	if (prefix==0xFeFF)
	{
		CUniString buf;
		int iChars=iLen/sizeof(wchar_t)-1;
		if (iChars>0)
		{
			ReturnIfFailed(pFile->Read(buf.GetBuffer(iChars), iChars*sizeof(wchar_t), NULL));
		}

		// Done
		str.Assign(buf);
		return s_ok;
	}
	else
	{
		// Back to start
		pFile->Seek(0);

		// Read entire file in to ansi buffer
		CAnsiString buf;
		if (iLen>0)
		{
			ReturnIfFailed(pFile->Read(buf.GetBuffer(iLen), iLen, NULL));
		}

		// Done
		str.Assign(buf);
		return s_ok;
	}
}

#ifdef _MSC_VER
#pragma warning(disable:4127)	 // conditional expression is constant
#endif

template <class T>
result_t SaveText(IFile* pFile, const T* psz)
{
	// If unicode, write prefix character
	if (sizeof(T)==sizeof(wchar_t))
	{
		wchar_t prefix=0xFEFF;
		ReturnIfFailed(pFile->Write(&prefix, sizeof(prefix), NULL));
	}

	// Write the stirng
	return pFile->Write(psz, CString<T>::len(psz) * sizeof(T), NULL);
}

#ifdef _MSC_VER
#pragma warning(default:4127)
#endif

template <class T>
result_t LoadText(const CAnyString& pszFileName, CString<T>& str)
{
	CFile file;
	ReturnIfFailed(file.OpenExisting(pszFileName));
	return LoadText(&file, str);
}

template <class T>
result_t SaveText(const CAnyString& pszFileName, const T* psz)
{
	CFile file;
	ReturnIfFailed(file.CreateNew(pszFileName));
	return SaveText(&file, psz);
}


/*
inline result_t SaveText(const char* pszFileName, const char* psz)
	{ return SaveText<char, char>(pszFileName, psz); };
inline result_t SaveText(const wchar_t* pszFileName, const char* psz)
	{ return SaveText<wchar_t, char>(pszFileName, psz); };
inline result_t SaveText(const char* pszFileName, const wchar_t* psz)
	{ return SaveText<char, wchar_t>(pszFileName, psz); };
inline result_t SaveText(const wchar_t* pszFileName, const wchar_t* psz)
	{ return SaveText<wchar_t, wchar_t>(pszFileName, psz); };
	*/

// For backwards compatibility - these return bools, not result_t
template <class T>
inline bool LoadTextFile(const CAnyString& pszFileName, CString<T>& str)
	{ return LoadText<T>(pszFileName, str)==0; }
inline bool SaveTextFile(const CAnyString& pszFileName, const char* psz)
	{ return SaveText<char>(pszFileName, psz)==0; };
inline bool SaveTextFile(const CAnyString& pszFileName, const wchar_t* psz)
	{ return SaveText<wchar_t>(pszFileName, psz)==0; };

}	//  namespace Simple

#endif	// __LOADSAVETEXTFILE_H

