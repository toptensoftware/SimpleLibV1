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
// ResParser.cpp - implementation of CResNode class

#include "stdafx.h"
#include "SimpleLibUtilsBuild.h"

#include "ResParser.h"
#include "ParseUtils.h"
#include "LoadSaveTextFile.h"

namespace Simple
{


/////////////////////////////////////////////////////////////////////////////
// CResParser

enum
{
	tokenOpenBrace=tokenOperatorBase,
	tokenCloseBrace,
	tokenEquals,
	tokenSemiColon,
	tokenPointer,
	tokenOr,
};


// Constructor
CResParser::CResParser()
{
	SetContentProvider(this);
	SetExtraIdentifierChars(NULL, L".");
	AddOperator(L"{", tokenOpenBrace);
	AddOperator(L"}", tokenCloseBrace);
	AddOperator(L"=", tokenEquals);
	AddOperator(L";", tokenSemiColon);
	AddOperator(L"->", tokenPointer);
	AddOperator(L"|", tokenOr);
}

// Destructor
CResParser::~CResParser()
{
}


bool CResParser::Parse(CResNode* pRootNode, const wchar_t* pszFileName)
{
	CUniString str;
	result_t r=LoadText(pszFileName, str);
	if (r)
	{
		SetError(Format(L"Failed to load '%s' - %s", pszFileName, FormatResult(r)), L"<inputfile>", -1);
		return false;
	}

	return Parse(pRootNode, str, pszFileName);
}

bool CResParser::Parse(CResNode* pRootNode, const wchar_t* pszContent, const wchar_t* pszFileName)
{
	// Initialize tokenizer
	CCppTokenizer::ParseString(pszContent, pszFileName);

	// Parse it
	CVector<CResNode*>	ScopeStack;
	ScopeStack.Push(pRootNode);
	while (CurrentToken()!=tokenEOF)
	{
		// Ending a Node
		if (CurrentToken()==tokenCloseBrace)
		{
			// Skip it
			NextToken();

			if (CurrentToken()==tokenSemiColon)
			{
				NextToken();
			}

			// Pop scope stack...
			ScopeStack.Pop();

			// Check have something to pop!
			if (ScopeStack.IsEmpty())
			{
				Unexpected();
				return false;
			}

			continue;
		}

		// Get name of element - expect string literal or identifier...
		CUniString strName=GetStringLiteral();
		if (CurrentToken()!=tokenStringLiteral && CurrentToken()!=tokenIdentifier)
		{
			Unexpected(L"when parsing Node or value name ");
			return false;
		}
		NextToken();

		if (CurrentToken()==tokenPointer)
		{
			// Get the link to name
			NextToken();
			CUniString strLinkTo=GetStringLiteral();
			if (!Skip(tokenStringLiteral))
				return false;
			if (strLinkTo.IsEmpty())
			{
				SetError(L"Missing link reference name");
				return false;
			}

			// Start a new Node
			CResNode* pNewSection;
			pNewSection=ScopeStack.Top()->CreateNewNode(strName);

			pNewSection->SetLink(strLinkTo);

			Skip(tokenSemiColon);
			continue;
		}

		// Parse an optional assign before open brace...
		bool bHaveAssign=CurrentToken()==tokenEquals;
		if (bHaveAssign)
		{
			NextToken();
		}

		// Starting a Node?
		if (CurrentToken()==tokenOpenBrace)
		{
			// Skip it
			NextToken();

			// Start a new Node
			ScopeStack.Push(ScopeStack.Top()->CreateNewNode(strName));
			continue;
		}

		// Expect assignment
		if (!bHaveAssign)
		{
			if (!Skip(tokenEquals))
				return false;
			NextToken();
		}

		// Parse value...
		CUniString strValue;
		if (!ParseValue(strValue))
			return false;

		// Skip semicolon
		if (!Skip(tokenSemiColon))
			return false;

		// Store value...
		ScopeStack.Top()->SetValue(strName, strValue);

	}

	// Check nothing left on stack except the root node
	if (ScopeStack.GetSize()!=1)
	{
		SetError(L"Missing closing brace");
		return false;
	}

	// Done!
	return true;
}

bool CResParser::ParseValue(CUniString& strValue)
{
	switch (CurrentToken())
	{
		case tokenStringLiteral:
		case tokenIdentifier:
			strValue=GetStringLiteral();
			NextToken();
			break;

		case tokenDoubleLiteral:
			strValue=GetTokenText();
			NextToken();
			break;

		case tokenInt32Literal:
			if (GetTypeQualifier()==tqRgb)
			{
				strValue=GetTokenText();
				NextToken();
			}
			else
			{
				int iValue=GetInt32Literal();
				NextToken();
				while (CurrentToken()==tokenOr)
				{
					NextToken();
					if (!Check(tokenInt32Literal))
						return false;
					iValue|=GetInt32Literal();
					NextToken();
				}

				strValue=Format(L"%i", iValue);
			}
			break;

		case tokenInt64Literal:
			strValue=Format(L"%I64i", GetInt64Literal());
			NextToken();
			break;

		default:
			Unexpected(L"when parsing value ");
			return false;
	}

	return true;
}

CUniString CResParser::GetParseError()
{
	return CCppTokenizer::GetError();
}

static CUniString EscapeString(const wchar_t* psz, bool bValue, bool bForceQuotes=false)
{
	if (IsEmptyString(psz))
		return L"\"\"";

	if (bValue)
	{
		// Check if all digits
		const wchar_t* p=psz;
		while (p[0])
		{
			if (!iswdigit(p[0]))
				break;
			p++;
		}

		// Yes!
		if (!p[0])
			return psz;
	}

	CUniString str;

	bool bNeedQuotes=!!iswdigit(psz[0]) || bForceQuotes;

	const wchar_t* p=psz;
	while (p[0])
	{
		if (!iswalnum(p[0]))
			bNeedQuotes=true;

		switch (p[0])
		{
			case L'\\':
				str+=L"\\\\";
				break;

			case L'\"':
				str+=L"\\\"";
				break;

			case L'\n':
				str+=L"\\n";
				break;

			case L'\r':
				str+=L"\\r";
				break;

			case L'\t':
				str+=L"\\t";
				break;

			default:
				str.Append(p, 1);
				break;
		}

		p++;
	}

	if (bNeedQuotes)
		str=Format(L"\"%s\"", str.sz());

	return str;
}

CUniString TabString(int iTabs)
{
	CUniString str;
	const wchar_t pszTabs[]=L"\t\t\t\t\t\t\t\t";
	while (iTabs>0)
	{
		str.Append(pszTabs, min(8, iTabs));
		iTabs-=8;
	}
	return str;
}



void FormatResNode(CUniString& buf, CResNode* pNode, int iIndent, bool bIncludeSelf)
{
	if (bIncludeSelf)
	{
		buf.Append(TabString(iIndent));
		buf.Append(EscapeString(pNode->GetName(), false));

		CUniString strLink=pNode->GetLink();
		if (!strLink.IsEmpty())
		{
			buf.Append(L"->");
			buf.Append(EscapeString(strLink, false, true));
			buf.Append(L";\r\n");
			return;
		}

		buf.Append(L"\r\n");
		buf.Append(TabString(iIndent));
		buf.Append(L"{\r\n");

		iIndent++;

		// Write values
		for (int i=0; i<pNode->GetValueCount(); i++)
		{
			buf.Append(TabString(iIndent));
			buf.Append(EscapeString(pNode->GetValueName(i), false));
			buf.Append(L"=");
			buf.Append(EscapeString(pNode->GetValue(i), true));
			buf.Append(L";\r\n");
		}
	}

	// Write sub sections
	for (int i=0; i<pNode->GetNodeCount(); i++)
	{
		FormatResNode(buf, pNode->GetNode(i), iIndent, true);
	}

	if (bIncludeSelf)
	{
		iIndent--;
		buf.Append(TabString(iIndent));
		buf.Append(L"}\r\n");
	}
}



CUniString FormatResNode(CResNode* pNode, bool bIncludeSelf)
{
	CUniString str;
	FormatResNode(str, pNode, 0, bIncludeSelf);
	return str;
}

result_t SaveResNode(CResNode* pNode, const wchar_t* pszFileName)
{
	CUniString str=FormatResNode(pNode, false);
	return SaveTextFile(pszFileName, str) ? s_ok : e_fail;
}

result_t LoadResNode(CResNode* pNode, const wchar_t* pszFileName)
{
	CResParser p;
	return p.Parse(pNode, pszFileName) ? s_ok : e_fail;
}

}
