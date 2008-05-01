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
// SplitString.h - declaration of SplitString

#ifndef __SPLITSTRING_H
#define __SPLITSTRING_H

namespace Simple
{

template <class T>
const T* SIMPLEAPI FindChar(const T* psz, T ch)
{
	while (psz[0])
	{
		if (psz[0]==ch)
			return psz;
		psz++;
	}
	return NULL;
}

template <class T>
void SIMPLEAPI SplitString(const T* pszString, const T* pszDelimChars, CVector< CString<T> >& vecStrings, bool bKeepBlanks=false)
{
	if (!pszString)
		return;

	ASSERT(pszDelimChars!=NULL);
	
	const T* p=pszString;

	// Store start of word
	const T* pszStartWord=bKeepBlanks ? p : NULL;

	while (p[0])
		{
		// Is it a delimiter
		if (FindChar(pszDelimChars, p[0]))
			{
			if (pszStartWord)
			{
				// Add to collection
				vecStrings.Add(CString<T>(pszStartWord, int(p-pszStartWord)));
			}

			// Clear the word
			if (!bKeepBlanks)
				pszStartWord=NULL;
			else
				pszStartWord=p+1;

			// Next character
			p++;
			}
		else
			{
			if (!pszStartWord)
				pszStartWord=p;

			p++;
			}
		}

	// Add the last item
	if (pszStartWord)
	{
		vecStrings.Add(CString<T>(pszStartWord, int(p-pszStartWord)));
	}
}


inline void SIMPLEAPI SplitString(const wchar_t* pszString, const wchar_t* pszDelimChars, CVector< CString<wchar_t> >& vecStrings, bool bKeepBlanks=false)
{ SplitString<wchar_t>(pszString, pszDelimChars, vecStrings, bKeepBlanks); }
inline void SIMPLEAPI SplitString(const char* pszString, const char* pszDelimChars, CVector< CString<char> >& vecStrings, bool bKeepBlanks=false)
{ SplitString<char>(pszString, pszDelimChars, vecStrings, bKeepBlanks); }

template <class T>
bool SIMPLEAPI SplitString(const T* pszString, const T* pszDelimChars, CString<T>& strLeft, CString<T>& strRight)
{
	if (!pszString)
		return false;

	const T* p=pszString;
	const T* pszRight=NULL;
	while (p[0])
		{
		// Is it a delimiter
		if (FindChar(pszDelimChars, p[0]))
			{
			pszRight=p+1;
			break;
			}
		
		p++;
		}

	if (pszRight)
		{
		strLeft=CString<T>(pszString, int(pszRight-pszString-1));
		strRight=pszRight;
		return true;
		}
	else
		{
		strLeft=pszString;
		return false;
		}
}

inline bool SIMPLEAPI SplitString(const wchar_t* pszString, const wchar_t* pszDelimChars, CUniString& strLeft, CUniString& strRight)
{
	return SplitString<wchar_t>(pszString, pszDelimChars, strLeft, strRight);
}

inline bool SIMPLEAPI SplitString(const char* pszString, const char* pszDelimChars, CAnsiString& strLeft, CAnsiString& strRight)
{
	return SplitString<char>(pszString, pszDelimChars, strLeft, strRight);
}

template <class T>
CString<T> SIMPLEAPI UnSplitString(CVector< CString<T> >& vecStrings, const T* pszDelim)
{
	CString<T> str;
	for (int i=0; i<vecStrings.GetSize(); i++)
	{
		if (i>0)
			str+=pszDelim;
		str+=vecStrings[i];
	}
	return str;
}

}	// namespace Simple

#endif	// __SPLITSTRING_H

