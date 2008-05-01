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

namespace Simple
{

#pragma warning(disable:4996)

template <class T>
FILE* SIMPLEAPI fopen_t(const T* pszFileName, const T* pszMode);

template <>
inline FILE* SIMPLEAPI fopen_t(const wchar_t* pszFileName, const wchar_t* pszMode)
{
	return _wfopen(pszFileName, pszMode);
}


template <>
inline FILE* SIMPLEAPI fopen_t(const char* pszFileName, const char* pszMode)
{
	return fopen(pszFileName, pszMode);
}
#pragma warning(default:4996)



template <class T, class TString>
bool SIMPLEAPI LoadTextFile(const T* pszFileName, TString& str)
{
	// Open file
	FILE* pFile=fopen_t<T>(pszFileName, t2t<T,char>("rb"));
	if (!pFile)
		return false;

	// Get length of file
	fseek(pFile, 0, SEEK_END);
	int iFileSize=ftell(pFile);
	fseek(pFile, 0, SEEK_SET);

	// Read first two bytes
	wchar_t prefix;
	if (fread(&prefix, sizeof(prefix), 1, pFile)==1 && prefix==0xFeFF)
	{
		CUniString buf;
		int iChars=iFileSize/sizeof(wchar_t)-1;
		bool bSuccess=fread(buf.GetBuffer(iChars), iChars*sizeof(wchar_t), 1, pFile)==1;
		fclose(pFile);

		// Return file content
		if (bSuccess)
			str.Assign(buf);

		// Done
		return bSuccess;
	}
	else
	{
		// Back to start
		fseek(pFile, 0, SEEK_SET);

		// Read entire file in to ansi buffer
		CAnsiString buf;
		bool bSuccess=fread(buf.GetBuffer(iFileSize), iFileSize, 1, pFile)==1;
		fclose(pFile);

		// Return file content
		if (bSuccess)
			str.Assign(buf);

		// Done
		return bSuccess;
	}
}

#pragma warning(disable:4127)	 // conditional expression is constant

template <class T, class TString>
bool SIMPLEAPI SaveTextFile(const T* pszFileName, const TString* psz)
{
	// Open file
	FILE* pFile=fopen_t<T>(pszFileName, t2t<T,char>("wb"));
	if (!pFile)
		return false;

	// If unicode, write prefix character
	if (sizeof(TString)==sizeof(wchar_t))
	{
		wchar_t prefix=0xFEFF;
		if (fwrite(&prefix, sizeof(prefix), 1, pFile)!=1)
		{
			fclose(pFile);
			return false;
		}
	}

	// Write the stirng
	if (fwrite(psz, CString<TString>::len(psz) * sizeof(TString), 1, pFile)!=1)
	{
		fclose(pFile);
		return false;
	}

	// Close file and finished!
	fclose(pFile);
	return true;
}

#pragma warning(default:4127)

inline bool SIMPLEAPI SaveTextFile(const char* pszFileName, const char* psz) 
	{ return SaveTextFile<char, char>(pszFileName, psz); };
inline bool SIMPLEAPI SaveTextFile(const wchar_t* pszFileName, const char* psz) 
	{ return SaveTextFile<wchar_t, char>(pszFileName, psz); };
inline bool SIMPLEAPI SaveTextFile(const char* pszFileName, const wchar_t* psz) 
	{ return SaveTextFile<char, wchar_t>(pszFileName, psz); };
inline bool SIMPLEAPI SaveTextFile(const wchar_t* pszFileName, const wchar_t* psz) 
	{ return SaveTextFile<wchar_t, wchar_t>(pszFileName, psz); };

}	// namespace Simple

#endif	// __LOADSAVETEXTFILE_H

