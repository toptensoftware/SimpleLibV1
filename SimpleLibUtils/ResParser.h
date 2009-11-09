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
// ResParser.h - declaration of CResNode class

#ifndef __RESPARSER_H
#define __RESPARSER_H

#include "ResNode.h"
#include "Tokenizer.h"
#include "Result.h"


namespace Simple
{

class CResParser : 
	public CCppTokenizer,
	public CFileContentProvider
{
public:
// Construction
			CResParser();
	virtual ~CResParser();

// Operations
	bool Parse(CResNode* pRootNode, const wchar_t* pszFileName);
	bool Parse(CResNode* pRootNode, const wchar_t* pszContent, const wchar_t* pszFileName);
	CUniString GetParseError();

protected:
	bool ParseValue(CUniString& strValue);

};

void FormatResNode(CUniString& buf, CResNode* pNode, int iIndent, bool bIncludeSelf=true);
CUniString FormatResNode(CResNode* pNode, bool bIncludeSelf=true);

result_t SaveResNode(CResNode* pNode, const wchar_t* pszFileName);
result_t LoadResNode(CResNode* pNode, const wchar_t* pszFileName);

}

#endif	// __RESPARSER_H

