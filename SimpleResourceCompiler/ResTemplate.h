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
// ResTemplat.h - declaration of CResNode class

#ifndef __RESTEMPLATE_H
#define __RESTEMPLATE_H

namespace Simple
{


class CResExpression;
class CResStatements;

class CResTemplate
{
public:
// Construction
			CResTemplate();
	virtual ~CResTemplate();

// Operations	
	bool ParseString(const CAnyString strContent);
	bool ParseFile(const CAnyString& strFileName);

// Render
	bool Render(CResNode* pNode, CUniString& strOutput);

// Get last error
	CUniString GetError();
	bool SetError(const wchar_t* psz, const wchar_t* pszPosition);
	bool InError();

// Implementation
protected:
// Attributes
	CUniString		m_strText;
	const wchar_t*	m_pszText;
	CUniString		m_strError;
	CResStatements*	m_pStatements;

	bool IsBlockMode(const wchar_t* pszTagStart, const wchar_t*& pszTagEnd);

	CResExpression* ParseExpression(const wchar_t*& pszText);
	bool ParseStatements(CResStatements* pStatements, const wchar_t*& pszText, bool bBlockMode);
};


}

#endif	// __RESTEMPLATE_H

