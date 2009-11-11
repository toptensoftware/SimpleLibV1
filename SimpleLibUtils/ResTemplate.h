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

#include "ResNode.h"
#include "Result.h"


namespace Simple
{


class CResExpression;
class CResStatement;

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
	CUniString Render(CResNode* pNode);

// Get last error
	CUniString GetError();
	void SetError(const wchar_t* psz);

// Implementation
protected:
// Attributes
	CUniString		m_strError;
	CResStatement*	m_pRootStatement;
};

}

#endif	// __RESTEMPLATE_H

