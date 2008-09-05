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
// ParseUtils.h - declaration of ParseUtils

#ifndef __PARSEUTILS_H
#define __PARSEUTILS_H

#include "SplitString.h"


namespace Simple
{

bool SIMPLEAPI IsEOL(wchar_t ch);
bool SIMPLEAPI SkipEOL(const wchar_t*& p);
void SIMPLEAPI SkipToEOL(const wchar_t*& p);
void SIMPLEAPI SkipToNextLine(const wchar_t*& p);
bool SIMPLEAPI DoesMatch(const wchar_t* p, const wchar_t* psz, bool bEndWord=false);
bool SIMPLEAPI SkipMatch(const wchar_t*& p, const wchar_t* psz, bool bEndWord=false);
bool SIMPLEAPI DoesMatchI(const wchar_t* p, const wchar_t* psz, bool bEndWord=false);
bool SIMPLEAPI SkipMatchI(const wchar_t*& p, const wchar_t* psz, bool bEndWord=false);
bool SIMPLEAPI SkipWhiteSpace(const wchar_t*& p);
bool SIMPLEAPI IsLineEndChar(wchar_t ch);
bool SIMPLEAPI IsLineSpace(wchar_t ch);
bool SIMPLEAPI IsWhiteSpace(wchar_t ch);
bool SIMPLEAPI IsIdentifierLeadChar(wchar_t ch, const wchar_t* pszExtraChars=NULL);
bool SIMPLEAPI IsIdentifierChar(wchar_t ch, const wchar_t* pszExtraChars=NULL);
bool SIMPLEAPI SkipLineSpace(const wchar_t*& p);
bool SIMPLEAPI SkipLineEnd(const wchar_t*& p);
bool SIMPLEAPI SkipIdentifier(const wchar_t*& p, CUniString& str, const wchar_t* pszExtraLeadChars=NULL, const wchar_t* pszExtraChars=NULL);
bool SIMPLEAPI ReadInt(const wchar_t*& p, int* pi);
bool SIMPLEAPI ReadInt64(const wchar_t*& p, __int64* pi);
bool SIMPLEAPI ReadDouble(const wchar_t*& psz, double* pdblVal);
const wchar_t* SIMPLEAPI ParseInt(const wchar_t* psz, int* piValue);
const wchar_t* SIMPLEAPI ParseInt64(const wchar_t* psz, __int64* piValue);


template <class T>
const T* SIMPLEAPI FindOneOf(const T* psz, const T* pszDelims)
{
	while (psz[0])
	{
		if (FindChar(pszDelims, psz[0]))
			return psz;
		psz++;
	}
	return NULL;
}

class CSimpleParser
{
public:
	CSimpleParser(const wchar_t* psz=NULL) : 
		m_psz(psz),
		m_pszError(NULL),
		m_bSkipWhiteSpace(false),
		m_bCaseSensitive(false)
	{
	}

	void Init(const wchar_t* psz)
	{
		m_psz=psz;
		m_pszError=NULL;
	}

	void SetAutoSkipWhiteSpace(bool bSkipWhiteSpace)
	{
		m_bSkipWhiteSpace=bSkipWhiteSpace;
	}

	void SetCaseSensitive(bool bCaseSensitive)
	{
		m_bCaseSensitive=bCaseSensitive;
	}
	
	bool IsEOL()
	{
		if (m_pszError)
			return false;
		return Simple::IsEOL(m_psz[0]);
	}

	bool SkipEOL()
	{
		if (m_pszError)
			return false;

		if (m_bSkipWhiteSpace)
			SkipLineSpace();

		if (!IsEOL())
		{
			m_pszError=m_psz;
			return false;
		}

		SkipToNextLine();
		return true;
	}

	bool CheckError(bool bSuccess)
	{
		if (!bSuccess && !m_pszError)
		{
			m_pszError=m_psz;
		}

		return bSuccess;
	}

	void AutoSkipWhiteSpace()
	{
		if (m_bSkipWhiteSpace)
			SkipWhiteSpace();
	}

	bool ReadInt(int& iVal)
	{
		if (m_pszError)
			return false;
		AutoSkipWhiteSpace();

		return CheckError(Simple::ReadInt(m_psz, &iVal));
	}

	bool ReadDouble(double& dblVal)
	{
		if (m_pszError)
			return false;
		AutoSkipWhiteSpace();

		return CheckError(Simple::ReadDouble(m_psz, &dblVal));
	}

	bool ReadString(const wchar_t* pszDelims, CUniString& str)
	{
		if (m_pszError)
			return false;
		AutoSkipWhiteSpace();

		const wchar_t* psz=FindOneOf(m_psz, pszDelims);
		if (!psz)
		{
			m_pszError=m_psz;
			return false;
		}

		str=CUniString(m_psz, int(psz-m_psz));
		m_psz=psz;
		return true;
	}
	
	bool ReadLine(CUniString& str)
	{
		if (m_pszError)
			return false;
		AutoSkipWhiteSpace();

		if (m_psz[0]==0)
		{
			m_pszError=m_psz;
			return false;
		}

		const wchar_t* pszStart=m_psz;
		SkipToEOL();

		str=CUniString(pszStart, int(m_psz-pszStart));
		
		SkipToNextLine();

		return true;
	}

	bool ReadIdentifier(CUniString& str)
	{
		if (m_pszError)
			return false;
		AutoSkipWhiteSpace();

		return CheckError(SkipIdentifier(m_psz, str));
	}

	int ReadOneOf(const wchar_t* pszOperators)
	{
		if (m_pszError)
			return -1;
		AutoSkipWhiteSpace();

		CUniStringVector vec;
		SplitString(pszOperators, L" ", vec);
		ASSERT(vec.GetSize()!=0);

		for (int i=0; i<vec.GetSize(); i++)
		{
			ASSERT(!vec[0].IsEmpty());

			if (SkipToken(vec[i]))
				return i;

			ClearError();
		}

		m_pszError=m_psz;
		return -1;
	}

	bool IsToken(const wchar_t* pszToken)
	{
		if (m_pszError)
			return false;
		AutoSkipWhiteSpace();

		int iLen=(int)wcslen(pszToken);

		ASSERT(iLen!=0);

		if (m_bCaseSensitive)
		{
			if (wcsncmp(m_psz, pszToken, iLen)==0)
				return true;
		}
		else
		{
			if (_wcsnicmp(m_psz, pszToken, iLen)==0)
				return true;
		}

		return false;
	}

	bool SkipToken(const wchar_t* pszToken)
	{
		if (m_pszError)
			return false;
		AutoSkipWhiteSpace();

		if (!IsToken(pszToken))
		{
			m_pszError=m_psz;
			return false;
		}

		m_psz+=wcslen(pszToken);
		return true;
	}

	bool SkipOptionalToken(const wchar_t* pszToken)
	{
		if (m_pszError)
			return false;
		AutoSkipWhiteSpace();

		if (!IsToken(pszToken))
		{
			return false;
		}

		m_psz+=wcslen(pszToken);
		return true;
	}

	bool SkipWhiteSpace()
	{
		return Simple::SkipWhiteSpace(m_psz);
	}
	
	bool SkipLineSpace()
	{
		return Simple::SkipLineSpace(m_psz);
	}
	
	bool SkipToEOL()
	{
		Simple::SkipToEOL(m_psz);
		return true;
	}
	
	bool SkipToNextLine()
	{
		Simple::SkipToNextLine(m_psz);
	}

	bool CheckEOF()
	{
		if (m_pszError)
			return false;
		AutoSkipWhiteSpace();
		if (m_psz[0]==0)
			return true;

		m_pszError=m_psz;
		return false;
	}
	
	void ClearError()
	{
		m_pszError=NULL;
	}
	
	const wchar_t* GetErrorPosition()
	{
		return m_pszError;
	}

	const wchar_t* m_psz;
	const wchar_t* m_pszError;
	bool m_bSkipWhiteSpace;
	bool m_bCaseSensitive;
};




}	// namespace Simple

#endif	// __PARSEUTILS_H

