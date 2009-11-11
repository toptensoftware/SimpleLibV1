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
// Tokenizer.cpp - implementation of CCppTokenizer class

#include "StdAfx.h"
#include "SimpleLibUtilsBuild.h"

#include "Tokenizer.h"

#include "PathLibrary.h"
#include "LoadSaveTextFile.h"
#include "SplitString.h"
#include "ParseUtils.h"

#define ERR_NONE						0
#define ERR_EOFINCOMMENT				1
#define ERR_MACROSYNTAX					2
#define ERR_DUPLICATEMACRO				3
#define ERR_SYNTAX						4
#define ERR_EOFINMACRO					5
#define ERR_PREPROCNOTATSTARTOFLINE		6
#define ERR_INVALIDPREPROCDIRECTIVE		7
#define ERR_NOTENOUGHPARAMS				8
#define ERR_TOOMANYPARAMS				9
#define ERR_UNEXPECTEDELSE				10
#define ERR_UNEXPECTEDENDIF				11
#define ERR_PREPROCTRAILINGCHARACTERS	12
#define ERR_UNEXPECTEDEOF				13
#define ERR_EOFINSTRING					14

namespace Simple
{

static const wchar_t* g_pszErrors[] =
{
	L"No error",
	L"Unexpected end of file found in comment",
	L"#define syntax error - %s",
	L"Duplicate macro definition - '%s'",
	L"Syntax error - %s",
	L"Unexpected end of file in macro expansion",
	L"Preprocessor directives must start as first non-whitespace",
	L"Invalid preprocessor directive '%s'",
	L"Not enough actual parameters for macro '%s'",
	L"Too many actual parameters for macro '%s'",
	L"Unexpected #else",
	L"Unexpected #endif",
	L"Unexpected characters following preprocessor directive - expected a newline",
	L"Unexpected end of file found",
	L"Unexpected new line in string literal",
};


//////////////////////////////////////////////////////////////////////////
// Parser utility functions

int SIMPLEAPI ParseHexChar(wchar_t ch)
{
	if (ch>=L'0' && ch<='9')
		return ch-L'0';

	if (ch>=L'a' && ch<='f')
		return ch-L'a'+10;

	if (ch>=L'A' && ch<='F')
		return ch-L'A'+10;

	return -1;
}

bool SIMPLEAPI ParseHexU(const wchar_t*& p, uint64_t& iVal, bool& bOverflow)
{
	// Store start
	const wchar_t* pszStart=p;

	bOverflow=false;

	// Parse all hex characters
	int iNibble;
	iVal=0;
	while ((iNibble=ParseHexChar(p[0]))>=0)
	{
		// Check for overflow
		if (iVal & 0xF000000000000000ULL)
			bOverflow=true;

		iVal=(iVal << 4) | iNibble;
		p++;
	}

	// Return whether we parsed anything
	return p>pszStart;
}

bool SIMPLEAPI ParseIntU(const wchar_t*& p, uint64_t& iVal, bool& bOverflow)
{
	// Store start
	const wchar_t* pszStart=p;

	bOverflow=false;

	// Parse all digit characters
	iVal=0;
	while (iswdigit(p[0]))
	{
		uint64_t iPrev=iVal;
		iVal=iVal*10 + (p[0]-'0');

		if (iPrev>iVal)
			bOverflow=true;

		p++;
	}

	// Return whether we parsed anything
	return p>pszStart;
}

bool SIMPLEAPI IsDoubleU(const wchar_t* p)
{
	if (p[0]=='.')
		return true;

	if (!iswdigit(p[0]))
		return false;

	while (iswdigit(p[0]))
		p++;

	return p[0]=='.';
}

bool SIMPLEAPI ParseDoubleU(const wchar_t*& p, double& dblVal, bool& bOverflow)
{
	// Clear out values
	bOverflow=false;
	dblVal=0;

	// Must start with a digit or decimal
	if (!iswdigit(p[0]) && p[0]!='.')
		return false;

	// Parse the integer part
	while (iswdigit(p[0]))
	{
		dblVal=dblVal * 10 + (p[0]-'0');
		p++;
	}

	// Decimal part?
	if (p[0]=='.')
	{
		p++;
		double dblScale=0.1;
		while (iswdigit(p[0]))
		{
			dblVal=dblVal + dblScale * (p[0]-'0');
			p++;
			dblScale*=0.1;
		}
	}

	return true;
}

bool SIMPLEAPI ParseDouble(const wchar_t*& psz, double& dblVal, bool& bError)
{
	// Use local ptr
	const wchar_t* p=psz;

	// Clear error
	bError=false;

	// Handle negative
	bool bNegative=p[0]=='-';
	if (bNegative)
		p++;

	// Is it a number?
	if (!IsDoubleU(p))
		return false;

	// Double?
	ParseDoubleU(p, dblVal, bError);
	psz=p;

	if (bNegative)
		dblVal*=-1.0;

	return true;
}

bool SIMPLEAPI ParseInt64(const wchar_t*& psz, int64_t& iVal, bool& bError)
{
	// Use local ptr
	const wchar_t* p=psz;

	// Clear error
	bError=false;

	// Handle negative
	bool bNegative=p[0]=='-';
	if (bNegative)
		p++;

	if (!iswdigit(p[0]))
		return false;

	// Hex?
	uint64_t uVal;
	if (p[0]=='0' && (p[1]=='x' || p[1]=='X'))
	{
		p+=2;
		ParseHexU(p, uVal, bError);
	}
	else
	{
		ParseIntU(p, uVal, bError);
	}

	// Return new position
	psz=p;

	// Quit if error
	if (bError)
		return true;

	// Apply negative
	if (bNegative)
	{
		iVal=-int64_t(uVal);
	}
	else
	{
		iVal=uVal;
	}

	return true;

}


#define PPDIR_NONE		-1
#define PPDIR_DEFINE	0
#define PPDIR_UNDEF		1
#define PPDIR_IFDEF		2
#define PPDIR_IFNDEF	3
#define PPDIR_ELSE		4
#define PPDIR_ENDIF		5
#define PPDIR_ELIF		6
#define PPDIR_IF		7
#define PPDIR_INCLUDE	8
#define PPDIR_COUNT		9

const char* g_pszPPDirs[PPDIR_COUNT] =
{
	"define",
	"undef",
	"ifdef",
	"ifndef",
	"else",
	"endif",
	"elif",
	"if",
	"include",
};

int ParsePPDir(const wchar_t* pszU)
{
	CAnsiString str=w2a(pszU);

	for (int i=0; i<PPDIR_COUNT; i++)
	{
		if (Compare(g_pszPPDirs[i], str)==0)
			return i;
	}

	return PPDIR_NONE;

}


//////////////////////////////////////////////////////////////////////////
// OpChar Map

// Add a new operator string to this map
// Called recursively to create a tree of character sequences
void COperatorMap::Add(const wchar_t* psz, int iToken)
{
	// Look up or create entry for first character
	COperatorChar* pChar=NULL;
	if (!Find(psz[0], pChar))
	{
		pChar=new COperatorChar;
		pChar->m_ch=psz[0];
		pChar->m_iToken=0;
		base::Add(psz[0], pChar);
	}

	if (psz[1])
	{
		// If there's a subsequent character, recurse down adding rest of operator
		// to chile map
		pChar->m_mapNext.Add(psz+1, iToken);
	}
	else
	{
		// Just store the token
		pChar->m_iToken=iToken;
	}
}

// Read and operator from a string
int COperatorMap::Parse(const wchar_t*& p)
{
	// Lookup first character, quit if not found
	COperatorChar* pChar;
	if (!Find(p[0], pChar))
		return 0;

	// Increment position
	p++;

	// Recursively lookup following characters
	int iSub=pChar->m_mapNext.Parse(p);
	if (iSub)
		return iSub;

	// If didn't match anything and this is not a valid operator
	// by itself, then rewind
	if (!pChar->m_iToken)
	{
		p--;
	}

	// Return the token for this operator
	return pChar->m_iToken;
}

// Walk map looking building original token string from token id
bool COperatorMap::GetToken(int iToken, CUniString& str)
{
	// Just call each operator char, looking for match
	for (int i=0; i<GetSize(); i++)
	{
		if ((*this)[i].Value->GetToken(iToken, str))
			return true;
	}

	return false;
}

// Get characters for a token
bool COperatorChar::GetToken(int iToken, CUniString& str)
{
	// Is this the token?
	if (m_iToken==iToken)
	{
		// Yes, just return this character
		str=Format(L"%c", m_ch);
		return true;
	}

	// Check all subsequent characters
	CUniString strTemp;
	if (m_mapNext.GetToken(iToken, strTemp))
	{
		// Match, so return the following string, prefixed with this character
		str=Format(L"%c%s", m_ch, strTemp.sz());
		return true;
	}

	// Not found!
	return false;
}


//////////////////////////////////////////////////////////////////////////
// CFileContentProvider

// Constructor
CFileContentProvider::CFileContentProvider()
{
	m_pNext=NULL;
}

// Destructor
CFileContentProvider::~CFileContentProvider()
{
}

// Get the content of a file
bool CFileContentProvider::GetFileContent(
			const wchar_t* pszCurrentFileName,
			const wchar_t* pszFileName,
			bool bSysPath,
			CUniString& strQualifiedFileName,
			CUniString& strContent
			)
{
	// If have a current file, prepend its folder to the search path
	if (pszCurrentFileName)
	{
		bool bTempFirstSearchPath=false;
		if (!bSysPath)
		{
			CUniString strCurrentFolder;
			SplitPath(pszCurrentFileName, &strCurrentFolder, NULL);

			if (!strCurrentFolder.IsEmpty())
			{
				m_vecIncludePath.InsertAt(0, strCurrentFolder);
				bTempFirstSearchPath=true;
			}
		}

		// Search for the file...
		strQualifiedFileName=FindFileOnSearchPath(m_vecIncludePath, pszFileName);

		if (bTempFirstSearchPath)
			m_vecIncludePath.RemoveAt(0);
	}
	else
	{
		// Qualify specifed file...
		if (IsFullyQualified(pszFileName))
			strQualifiedFileName=pszFileName;
		else
			strQualifiedFileName=QualifyPath(pszFileName);

	}

	if (!DoesFileExist(strQualifiedFileName))
	{
		if (m_pNext)
		{
			return m_pNext->GetFileContent(pszCurrentFileName, pszFileName, bSysPath, strQualifiedFileName, strContent);
		}
		else
		{
			return false;
		}
	}

	// Load script
	return LoadTextFile<wchar_t>(strQualifiedFileName, strContent);
}

void CFileContentProvider::AddIncludePath(const wchar_t* pszPath, bool bAddToHead)
{
	if (bAddToHead)
	{
		CVector<CUniString> vec;
		SplitString(pszPath, L";", vec);
		for (int i=0; i<vec.GetSize(); i++)
		{
			m_vecIncludePath.InsertAt(i, vec[i]);
		}
	}
	else
	{
		SplitString(pszPath, L";", m_vecIncludePath);
	}
}


void CFileContentProvider::SetNextContentProvider(CContentProvider* pContentProvider)
{
	m_pNext=pContentProvider;
}



//////////////////////////////////////////////////////////////////////////
// CCppTokenizer

// Constructor
CCppTokenizer::CCppTokenizer()
{
	m_bReportLineBreaks=false;
	m_pContentProvider=&m_DefaultProvider;
}

// Destructor
CCppTokenizer::~CCppTokenizer()
{
}

bool CCppTokenizer::ParseString(const wchar_t* pszScript, const wchar_t* pszFileName)
{
	if (!pszScript)
		pszScript=L"";
	m_pszScript=pszScript;
	m_pszPos=pszScript;
	m_pszToken=pszScript;
	m_strError.Empty();
	m_strFileName=pszFileName;
	m_iLineNumber=1;
	m_ConditionalState.RemoveAll();
	m_iToken=-1;
	return NextToken()!=tokenError;
}



bool CCppTokenizer::ParseFile(const wchar_t* pszFileName)
{
	ASSERT(m_pContentProvider!=NULL);

	// Get file content from content provider
	m_strScript.Empty();
	if (!m_pContentProvider->GetFileContent(NULL, pszFileName, false, m_strFileName, m_strScript))
	{
		if (m_strError.IsEmpty())
		{
			SetError(Format(L"Failed to load '%s'.", pszFileName));
		}
		return false;
	}

	// Parse loaded text...
	return ParseString(m_strScript, pszFileName);
}


void CCppTokenizer::AddKeyword(const wchar_t* pszKeyword, int iToken)
{
	if (m_mapKeywords.HasKey(pszKeyword))
		return;

	m_mapKeywords.Add(pszKeyword, iToken);
	m_mapKeywordTokens.Add(iToken, pszKeyword);
}

void CCppTokenizer::AddKeywords(const wchar_t* pszKeywords, int iBaseToken)
{
	CVector<CUniString> vecKeywords;
	SplitString(pszKeywords, L" ", vecKeywords);
	for (int i=0; i<vecKeywords.GetSize(); i++)
	{
		AddKeyword(vecKeywords[i], iBaseToken+i);
	}
}

void CCppTokenizer::AddOperator(const wchar_t* pszOperator, int iToken)
{
	m_OpMap.Add(pszOperator, iToken);
}

void CCppTokenizer::AddOperators(const wchar_t* pszOperators, int iBaseToken)
{
	CVector<CUniString> vecOps;
	SplitString(pszOperators, L" ", vecOps);
	for (int i=0; i<vecOps.GetSize(); i++)
	{
		m_OpMap.Add(vecOps[i], iBaseToken+i);
	}
}


void CCppTokenizer::SetContentProvider(CContentProvider* pContentProvider)
{
	m_pContentProvider=pContentProvider;
}

CFileContentProvider* CCppTokenizer::GetDefaultContentProvider()
{
	return &m_DefaultProvider;
}




void CCppTokenizer::ReportLineBreaks(bool bNewVal)
{
	m_bReportLineBreaks=bNewVal;
}


int CCppTokenizer::CurrentToken()
{
	if (!IsEmptyString(m_strError))
	{
		return tokenError;
	}
	return m_iToken;
}


int CCppTokenizer::NextToken()
{
	// Get next token
	m_iToken=NextTokenInternal();

	if (!IsEmptyString(m_strError))
		m_iToken=tokenError;

	// Return it
	return m_iToken;
}

void CCppTokenizer::SetError(const wchar_t* pszMessage, const wchar_t* pszFileName, int iLine)
{
	// Check if error already set
	if (!IsEmptyString(m_strError))
		return;

	// Find file name
	CUniString strFileName=pszFileName;
	if (strFileName.IsEmpty())
	{
		iLine=GetCurrentPosition(strFileName);
	}

	// Store error location...
	m_strError=Format(L"%s(%i): %s", strFileName.sz(), iLine, pszMessage);
	m_iToken=tokenError;
}

const wchar_t* CCppTokenizer::GetError()
{
	return m_strError;
}

const wchar_t* CCppTokenizer::GetStringLiteral()
{
	return m_strString;
}

int CCppTokenizer::GetInt32Literal()
{
	return int(m_iVal);
}

int64_t CCppTokenizer::GetInt64Literal()
{
	return m_iVal;
}

double CCppTokenizer::GetDoubleLiteral()
{
	if (m_iToken==tokenInt32Literal)
	{
		return double(m_iVal);
	}
	if (m_iToken==tokenInt64Literal)
	{
		return double(m_iVal);
	}
	else if (m_iToken==tokenDoubleLiteral)
	{
		ASSERT(m_iToken==tokenDoubleLiteral);
		return m_dblVal;
	}
	else
		return 0;
}


TypeQualifier CCppTokenizer::GetTypeQualifier()
{
	return m_TypeQualifier;
}



int CCppTokenizer::NextTokenInternal()
{
	if (!IsEmptyString(m_strError))
		return tokenError;
	if (m_iToken==tokenEOF)
		return tokenEOF;

StartAgain:
	if (!IsEmptyString(m_strError))
		return tokenError;

	m_strString.Empty();
	m_iVal=0;
	m_dblVal=0;
	m_TypeQualifier=tqNone;

	// Skip whitespace and comments
	SkipWhiteSpaceAndComments();

	// EOF?
	if (m_pszPos[0]=='\0')
	{
		// Check if we have pushed state we can pop from
		if (PopState())
			goto StartAgain;

		// Nope, really the end of the file
		return tokenEOF;
	}

	// EOL?
	if (IsEOL(m_pszPos[0]))
	{
		SkipEOL(m_pszPos);
		return tokenEOL;
	}

	// Preprocessor
	if (m_pszPos[0]=='#')
	{
		// Preprocess directives must be first non-whitespace on line
		bool bIsStartOfLine=IsStartOfLine();


		// Skip # + whitespace
		m_pszPos++;

		// HTML RGB value
		const wchar_t* psz=m_pszPos;
		uint64_t rgb;
		bool bOverflow;
		if (ParseHexU(psz, rgb, bOverflow))
		{
			if (int(psz-m_pszPos)==6)
			{
				m_iVal=((rgb & 0xFF)<<16) |
						(rgb & 0xFF00) |
						((rgb >> 16) & 0xFF);
				m_pszToken=m_pszPos-1;
				m_pszPos=psz;
				m_TypeQualifier=tqRgb;
				return tokenInt32Literal;
			}
		}


		SkipLineSpaceAndComments();

		// Get preprocessor type
		CUniString strDirective;
		if (!SkipIdentifier(m_pszPos, strDirective))
		{
			FormatError(ERR_SYNTAX, L"expected preprocesor directive, or rgb value");
			return tokenError;
		}

		// Preprocessor directives must be at start of line
		if (!bIsStartOfLine)
		{
			FormatError(ERR_PREPROCNOTATSTARTOFLINE);
			return tokenError;
		}

		// Skip more stuff
		SkipLineSpaceAndComments();

		// ifdef
		int iPPDir=ParsePPDir(strDirective);
		switch (iPPDir)
		{
			case PPDIR_DEFINE:
			{
				ParseMacro();
				goto StartAgain;
			}

			case PPDIR_INCLUDE:
			{
				ParseInclude();
				goto StartAgain;
			}

			case PPDIR_UNDEF:
			{
				SkipLineSpaceAndComments();
				CUniString strMacro;
				if (!SkipIdentifier(m_pszPos, strMacro))
				{
					FormatError(ERR_SYNTAX, "#undef expected an identifier");
					return tokenError;
				}

				CheckEndOfLine();

				m_mapMacros.Remove(strMacro);
				goto StartAgain;
			}

			case PPDIR_IFDEF:
			case PPDIR_IFNDEF:
			{
				SkipLineSpaceAndComments();

				CUniString strMacro;
				if (!SkipIdentifier(m_pszPos, strMacro))
				{
					FormatError(ERR_SYNTAX, L"#if[n]def expected an identifier");
					return tokenError;
				}

				// Work out whether to include this block or not
				bool bInclude=m_mapMacros.HasKey(strMacro);
				if (IsEqualString(strDirective, L"ifndef"))
					bInclude=!bInclude;

				// Process condition block
				HandleIf(bInclude);
				goto StartAgain;
			}

			case PPDIR_ELSE:
				IgnoreToEndOfLine();
				HandleElse();
				goto StartAgain;

			case PPDIR_ENDIF:
				IgnoreToEndOfLine();
				HandleEndif();
				goto StartAgain;

		}

		// What was that?
		FormatError(ERR_INVALIDPREPROCDIRECTIVE, strDirective.sz());
		return tokenError;
	}

	// Remember start of token
	m_pszToken=m_pszPos;

	// Unicode string?
	if (m_pszPos[0]=='L' && m_pszPos[1]=='\"')
	{
		m_TypeQualifier=tqUnicodeString;
		m_pszPos++;
	}

	// String?
	if (ParseStringLiteral(false, m_strString))
	{
		return tokenStringLiteral;
	}

	// Identifier?
	if (SkipIdentifier(m_pszPos, m_strString, m_strExtraIdentifierLeadChars, m_strExtraIdenfifierChars))
	{
		// Is it a macro?
		CMacro* pMacro;
		if (m_mapMacros.Find(m_strString, pMacro))
		{
			// Expand it
			if (ExpandMacro(pMacro))
			{
				// Continue parsing...
				goto StartAgain;
			}
			else
			{
				// Quit if error
				if (m_iToken==tokenError)
					return m_iToken;
			}
		}

		int iKeyword;
		if (m_mapKeywords.Find(m_strString, iKeyword))
			return (Token)(iKeyword);

		return tokenIdentifier;
	}

	// Operators?
	int iOp=m_OpMap.Parse(m_pszPos);
	if (iOp>0)
	{
		return iOp;
	}

	// Double?
	bool bError;
	if (ParseDouble(m_pszPos, m_dblVal, bError))
	{
		if (bError)
		{
			FormatError(ERR_SYNTAX, L"Error in floating point number format");
			return tokenError;
		}

		if (m_pszPos[0]=='f')
		{
			m_pszPos++;
			m_TypeQualifier=tqFloat;
		}

		return tokenDoubleLiteral;
	}

	// Integer?
	if (ParseInt64(m_pszPos, m_iVal, bError))
	{
		if (bError)
		{
			FormatError(ERR_SYNTAX, L"Error or overflow in integer number format");
			return tokenError;
		}

		return (m_iVal>>32)==0 ? tokenInt32Literal : tokenInt64Literal;
	}

	// Unknown...
	FormatError(ERR_SYNTAX, Format(L"unexpected character '%c'", m_pszPos[0]).sz());
	return tokenError;
}

void CCppTokenizer::ParseMacro()
{
	// Leading whitespace/comments
	SkipLineSpaceAndComments();

	// Macro name
	CUniString strMacroName;
	if (!SkipIdentifier(m_pszPos, strMacroName))
	{
		FormatError(ERR_MACROSYNTAX, L"expected macro name");
		return;
	}

	// Check for duplicate
	if (m_mapMacros.HasKey(strMacroName))
	{
		FormatError(ERR_DUPLICATEMACRO, strMacroName.sz());
		return;
	}

	// Parameters
	CVector<CUniString> vecParams;
	bool bHasParens=false;
	if (m_pszPos[0]=='(')
	{
		bHasParens=true;

		// Skip open bracket and whitespace
		m_pszPos++;
		SkipLineSpaceAndComments();

		// Empty parameter set?
		while (m_pszPos[0]!=')')
		{
			// No

			// Parse parameter name
			CUniString strParamName;
			if (!SkipIdentifier(m_pszPos, strParamName))
			{
				FormatError(ERR_MACROSYNTAX, L"expected parameter name");
				return;
			}
			vecParams.Add(strParamName);

			// Skip whitespace after parameter name
			SkipLineSpaceAndComments();

			// If its not a comma, we're done
			if (m_pszPos[0]!=',')
				break;

			// Skip comma and more whitespace
			m_pszPos++;
			SkipLineSpaceAndComments();

			// And continue with next parameter
		}

		// Skip closing bracket
		if (m_pszPos[0]!=')')
		{
			FormatError(ERR_MACROSYNTAX, L"expected ')'");
			return;
		}

		m_pszPos++;
	}

	// Parse the definition
	CUniString buf;
	if (m_pszPos[0] && !IsEOL(m_pszPos[0]))
	{
		// Don't need leading whitespace
		SkipLineSpaceAndComments();

		// Save start of definition
		const wchar_t* pszStartDef=m_pszPos;

		while (IsEmptyString(m_strError))
		{
			// End of line = end of macro
			if (!m_pszPos[0] || IsEOL(m_pszPos[0]))
				break;

			const wchar_t* pszPos=m_pszPos;

			SkipLineSpaceAndComments();

			if (m_pszPos==pszPos)
				m_pszPos++;
		}

		buf.Append(pszStartDef, int(m_pszPos-pszStartDef));
	}

	if (IsEOL(m_pszPos[0]))
	{
		m_iLineNumber++;
		SkipToNextLine(m_pszPos);
	}

	// Add new macro definition
	CMacro* pMacro=new CMacro;
	pMacro->m_bExpanding=false;
	pMacro->m_strDefinition=buf;
	pMacro->m_strName=strMacroName;
	pMacro->m_bHasParens=bHasParens;
	for (int i=0; i<vecParams.GetSize(); i++)
	{
		pMacro->m_vecParameters.Add(vecParams[i]);
	}

	// Add defintion
	m_mapMacros.Add(strMacroName, pMacro);
}

bool CCppTokenizer::ExpandMacro(CMacro* pMacro)
{
	// Don't double expand
	if (pMacro->m_bExpanding)
		return false;

	// Parse all parameters
	CVector<CUniString> vecParams;
	if (pMacro->m_bHasParens)
	{
		// If not paren, return false as not a macro, but not an error
		if (m_pszPos[0]!='(')
			return false;

		// Skip opening bracket
		m_pszPos++;

		// Parse parameters
		while (IsEmptyString(m_strError))
		{
			// Skip white space
			SkipWhiteSpaceAndComments();

			// End of parameters?
			if (m_pszPos[0]==')')
			{
				break;
			}

			// Store start of the parameter
			int iNestDepth=0;
			CUniString buf;
			while (IsEmptyString(m_strError))
			{
				// EOF?
				if (!m_pszPos[0])
				{
					FormatError(ERR_EOFINMACRO);
					return false;
				}

				// Check for nested parens
				if (m_pszPos[0]==')' && iNestDepth)
				{
					buf.Append(m_pszPos,1);
					m_pszPos++;
					iNestDepth--;
					continue;
				}
				if (m_pszPos[0]=='(')
				{
					buf.Append(m_pszPos,1);
					m_pszPos++;
					iNestDepth++;
					continue;
				}

				// End of parameter
				if (iNestDepth==0)
				{
					if (m_pszPos[0]==',' || m_pszPos[0]==')')
						break;
				}

				// Skip, but be aware of string literals
				CUniString str;
				const wchar_t* pszString=m_pszPos;
				if (ParseStringLiteral(false, str))
				{
					buf.Append(pszString, int(m_pszPos-pszString));
					continue;
				}
				else if (SkipComments())
				{
					continue;
				}
				else
				{
					buf.Append(m_pszPos,1);
					m_pszPos++;
				}
			}

			// Get a parameter?
			if (buf.GetLength())
			{
				vecParams.Add(buf);
			}

			// Another parameter
			if (m_pszPos[0]==',')
			{
				m_pszPos++;
				continue;
			}

			break;
		}

		if (m_pszPos[0]!=')')
		{
			FormatError(ERR_SYNTAX, L"expected ')'");
			return false;
		}
		m_pszPos++;
	}

	// Check number of parameters matches...
	if (vecParams.GetSize()<pMacro->m_vecParameters.GetSize())
	{
		FormatError(ERR_NOTENOUGHPARAMS, pMacro->m_strName.sz());
		return false;
	}
	if (vecParams.GetSize()>pMacro->m_vecParameters.GetSize())
	{
		FormatError(ERR_TOOMANYPARAMS, pMacro->m_strName.sz());
		return false;
	}

	// Now do the replacement
	CUniString buf;
	const wchar_t* p=pMacro->m_strDefinition;
	while (p[0])
	{
		CUniString strParam;
		if (SkipIdentifier(p, strParam))
		{
			int iParamIndex=pMacro->m_vecParameters.Find(strParam);
			if (iParamIndex<0)
			{
				buf.Append(strParam);
			}
			else
			{
				buf.Append(vecParams[iParamIndex]);
			}
		}
		else
		{
			buf.Append(p, 1);
			p++;
		}
	}

	if (buf.GetLength()==0)
		return true;

	PushState(pMacro, NULL);

	m_strScript=buf;
	m_pszScript=m_strScript;
	m_pszPos=m_pszScript;
	m_pszToken=m_pszScript;

	return true;
}

// Parse an include definition...
void CCppTokenizer::ParseInclude()
{
	SkipLineSpaceAndComments();

	wchar_t chEndChar;
	if (m_pszPos[0]=='\"')
	{
		chEndChar='\"';
	}
	else if (m_pszPos[0]=='<')
	{
		chEndChar='>';
	}
	else
	{
		FormatError(ERR_SYNTAX, L"expected #include filename as a string");
		return;
	}

	// Skip opening char
	m_pszPos++;

	// Store start of file name
	const wchar_t* pszStart=m_pszPos;

	// Find end of file name
	while (m_pszPos[0]!=chEndChar)
	{
		if (!m_pszPos[0] || IsEOL(m_pszPos[0]))
		{
			FormatError(ERR_SYNTAX, L"unexpected end of line found in filename string");
			return;
		}
		m_pszPos++;
	}

	// Get the file name
	CUniString strFileName(pszStart, int(m_pszPos-pszStart));

	// Skip end delimiter
	m_pszPos++;

	// Ignore rest of line
	CheckEndOfLine();

	// Quit if error
	if (!IsEmptyString(m_strError))
		return;


	ASSERT(m_pContentProvider!=NULL);

	// Get file content from content provider...
	CUniString strQualifiedFile;
	CUniString strContent;
	if (!m_pContentProvider->GetFileContent(m_strFileName, strFileName, chEndChar==L'>', strQualifiedFile, strContent))
	{
		if (m_strError.IsEmpty())
		{
			SetError(Format(L"Failed to load '%s'.", strFileName.sz()));
		}
		return;
	}

	// Save state
	PushState(NULL, strQualifiedFile);

	// Setup new state
	m_strScript=strContent;
	m_pszScript=m_strScript;
	m_pszPos=m_pszScript;
	m_pszToken=m_pszScript;
}

void CCppTokenizer::HandleIf(bool bInclude)
{
	// If including, increase conditional depth an continue
	if (bInclude)
	{
		m_ConditionalState.InsertAt(0, PPDIR_IFDEF);
		return;
	}

	SkipExcludedConditional(PPDIR_IFDEF);
}

void  CCppTokenizer::HandleElse()
{
	if (m_ConditionalState.GetSize()==0 || m_ConditionalState[0]==PPDIR_ELSE)
	{
		FormatError(ERR_UNEXPECTEDELSE);
		return;
	}

	SkipExcludedConditional(PPDIR_ELSE);
}

void CCppTokenizer::HandleEndif()
{
	if (m_ConditionalState.GetSize()==0)
	{
		FormatError(ERR_UNEXPECTEDENDIF);
		return;
	}

	m_ConditionalState.RemoveAt(0);
	return;

}

void CCppTokenizer::SkipExcludedConditional(int iState)
{
	int iDepth=0;
	while (IsEmptyString(m_strError))
	{
		int iCondDir=SkipToConditionalDirective();

		switch (iCondDir)
		{
			case PPDIR_NONE:
				return;

			case PPDIR_IFDEF:
			case PPDIR_IFNDEF:
			case PPDIR_IF:
				iDepth++;
				break;

			case PPDIR_ENDIF:
				if (iDepth==0)
				{
					IgnoreToEndOfLine();
					return;
				}
				else
				{
					iDepth--;
				}
				break;

			case PPDIR_ELSE:
				if (iDepth==0)
				{
					IgnoreToEndOfLine();

					// Already in else block?
					if (iState==PPDIR_ELSE)
					{
						FormatError(ERR_UNEXPECTEDELSE);
						return;
					}

					m_ConditionalState.InsertAt(0, PPDIR_ELSE);		// Remember were in an else block
					return;
				}
				break;
		}
	}
}

int CCppTokenizer::SkipToConditionalDirective()
{
StartAgain:
	while (m_pszPos[0]!='#')
	{
		// End of file...
		if (!m_pszPos[0])
		{
			FormatError(ERR_UNEXPECTEDEOF);
			return PPDIR_NONE;
		}

		// Skip whitespace and comments
		if (SkipWhiteSpaceAndComments())
			continue;

		// Handle strings...
		CUniString str;
		if (ParseStringLiteral(true, str))
			continue;

		// Next character
		m_pszPos++;
	}

	const wchar_t* pszSavePos=m_pszPos;

	// Skip the #
	m_pszPos++;

	// Skip space
	SkipLineSpaceAndComments();

	CUniString strDirective;
	if (!SkipIdentifier(m_pszPos, strDirective))
	{
		m_pszPos=pszSavePos+1;
		goto StartAgain;
	}

	// Skip more stuff
	SkipLineSpaceAndComments();

	// Which conditional directive is it?
	int iCondDir=ParsePPDir(strDirective);

	switch (iCondDir)
	{
		case PPDIR_ELSE:
		case PPDIR_IFDEF:
		case PPDIR_IFNDEF:
		case PPDIR_ENDIF:
		case PPDIR_IF:
		case PPDIR_ELIF:
			return iCondDir;
	}

	// Continue after the # sign
	m_pszPos=pszSavePos+1;
	goto StartAgain;
}

void CCppTokenizer::PushState(CMacro* pMacro, const wchar_t* pszFileName)
{
	CState* pState=new CState;
	pState->m_pszPos=m_pszPos;
	pState->m_pszScript=m_pszScript;
	pState->m_strScript=m_strScript;
	m_strScript.Empty();
	pState->m_pMacro=pMacro;
	pState->m_iLineNumber=m_iLineNumber;
	pState->m_strFileName=m_strFileName;
	m_vecStateStack.Add(pState);

	if (pState->m_pMacro)
	{
		pState->m_pMacro->m_bExpanding=true;
	}

	m_iLineNumber=1;
	m_strFileName=pszFileName;
	m_pszPos=NULL;
	m_pszScript=NULL;
	m_strScript.Empty();
}

bool CCppTokenizer::PopState()
{
	if (m_vecStateStack.GetSize()==0)
		return false;

	CState* pState=m_vecStateStack[m_vecStateStack.GetSize()-1];

	m_pszPos=pState->m_pszPos;
	m_pszScript=pState->m_pszScript;
	m_strScript=pState->m_strScript;
	m_strFileName=pState->m_strFileName;
	m_iLineNumber=pState->m_iLineNumber;

	if (pState->m_pMacro)
	{
		pState->m_pMacro->m_bExpanding=false;
	}

	m_vecStateStack.RemoveAt(m_vecStateStack.GetSize()-1);
	return true;
}

int CCppTokenizer::GetCurrentPosition(CUniString& strFileName)
{
	if (!IsEmptyString(m_strFileName))
	{
		strFileName=m_strFileName;
		return m_iLineNumber;
	}

	for (int i=0; i<m_vecStateStack.GetSize(); i++)
	{
		CState* pState=m_vecStateStack[i];
		if (!IsEmptyString(pState->m_strFileName))
		{
			strFileName=pState->m_strFileName;
			return pState->m_iLineNumber;
		}
	}

	strFileName=L"<unknown location>";
	return m_iLineNumber;
}

void CCppTokenizer::SetExtraIdentifierChars(const wchar_t* pszLeadChars, const wchar_t* pszOtherChars)
{
	m_strExtraIdentifierLeadChars=pszLeadChars;
	m_strExtraIdenfifierChars=pszOtherChars;

}

void CCppTokenizer::Define(const wchar_t* pszMacro, const wchar_t* pszDef)
{
	CMacro* pMacro=new CMacro;
	pMacro->m_bExpanding=false;
	pMacro->m_strDefinition=pszDef;
	pMacro->m_strName=pszMacro;
	pMacro->m_bHasParens=false;
	m_mapMacros.Add(pszMacro, pMacro);
}


void CCppTokenizer::FormatError(int iError, ...)
{
	if (!IsEmptyString(m_strError))
		return;

	va_list args;
	va_start(args, iError);
	SetError(Format(g_pszErrors[iError], args));
	va_end(args);
}

bool CCppTokenizer::IsStartOfLine()
{
	// Find start of line
	const wchar_t* p=m_pszPos-1;
	while (p>m_pszScript && IsLineSpace(p[-1]))
		p--;

	// Check first on line
	if (p>m_pszScript && !IsEOL(p[-1]))
	{
		return false;
	}

	return true;

}

void CCppTokenizer::CheckEndOfLine()
{
	SkipLineSpaceAndComments();

	if (!IsEOL(m_pszPos[0]))
		FormatError(ERR_PREPROCTRAILINGCHARACTERS);
}

void CCppTokenizer::IgnoreToEndOfLine()
{
	while (m_pszPos[0] && !IsEOL(m_pszPos[0]))
	{
		const wchar_t* pszOldPos=m_pszPos;

		SkipLineSpaceAndComments();

		if (pszOldPos==m_pszPos)
			m_pszPos++;
	}
}



bool CCppTokenizer::SkipComments()
{
	// Skip C++ comment?
	if (m_pszPos[0]=='/' && m_pszPos[1]=='/')
	{
		m_pszPos+=2;
		while (true)
		{
			if (m_pszPos[0]=='\\' && IsEOL(m_pszPos[1]))
			{
				m_pszPos++;
				m_iLineNumber++;
				SkipEOL(m_pszPos);
				continue;
			}

			if (!m_pszPos[0] || IsEOL(m_pszPos[0]))
				return true;

			m_pszPos++;
		}
	}


	const wchar_t* pszStart=m_pszPos;
	if (m_pszPos[0]=='/' && m_pszPos[1]=='*')
	{
		// Find end of comment
		while (m_pszPos[0] && !(m_pszPos[0]=='*' && m_pszPos[1]=='/'))
		{
			m_pszPos++;
		}

		// EOF?
		if (!m_pszPos[0])
		{
			m_pszPos=pszStart;
			FormatError(ERR_EOFINCOMMENT);
			return false;
		}
		else
		{
			// Skip terminator
			m_pszPos+=2;
		}

		return true;
	}

	return false;
}

bool CCppTokenizer::SkipLineSpace()
{
	const wchar_t* pszStart=m_pszPos;
	while (true)
	{
		// Line continuation?
		if (m_pszPos[0]=='\\' && IsEOL(m_pszPos[1]))
		{
			m_iLineNumber++;
			m_pszPos++;
			SkipEOL(m_pszPos);
			continue;
		}

		// Line space?
		if (IsLineSpace(m_pszPos[0]))
		{
			m_pszPos++;
			continue;
		}

		return m_pszPos!=pszStart;
	}
}

bool CCppTokenizer::SkipWhiteSpace()
{
	const wchar_t* pszStart=m_pszPos;
	while (true)
	{
		// Line continuation?
		if (m_pszPos[0]=='\\' && IsEOL(m_pszPos[1]))
		{
			m_iLineNumber++;
			m_pszPos++;
			SkipEOL(m_pszPos);
			continue;
		}

		// End of line?
		if (IsEOL(m_pszPos[0]))
		{
			if (!m_bReportLineBreaks)
			{
				m_iLineNumber++;
				SkipEOL(m_pszPos);
				continue;
			}
			else
			{
				return m_pszPos!=pszStart;
			}
		}

		// Line space?
		if (IsLineSpace(m_pszPos[0]))
		{
			m_pszPos++;
			continue;
		}

		// Done
		return m_pszPos!=pszStart;
	}
}

bool CCppTokenizer::SkipWhiteSpaceAndComments()
{
	const wchar_t* pszStart=m_pszPos;
	while (IsEmptyString(m_strError))
	{
		// Skip white space
		if (SkipWhiteSpace())
		{
			continue;
		}

		// C++ style comment?
		if (SkipComments())
		{
			continue;
		}

		return pszStart!=m_pszPos;
	}

	return false;
}

bool CCppTokenizer::SkipLineSpaceAndComments()
{
	const wchar_t* pszStart=m_pszPos;
	while (IsEmptyString(m_strError))
	{
		// Skip white space
		if (SkipLineSpace())
		{
			continue;
		}

		// C++ style comment?
		if (SkipComments())
		{
			continue;
		}

		return pszStart!=m_pszPos;
	}
	return false;
}

bool CCppTokenizer::Check(int token, const wchar_t* pszExpected)
{
	// If wanting double, allow integer too...
	if (CurrentToken()==tokenInt32Literal && token==tokenDoubleLiteral)
	{
		return true;
	}

	// If wanting double, allow integer too...
	if (CurrentToken()==tokenInt64Literal && token==tokenDoubleLiteral)
	{
		return true;
	}

	if (CurrentToken()==token)
	{
		return true;
	}


	// Nothing specified for error string?
	CUniString str;
	if (!pszExpected)
	{
		CUniString strToken;
		GetTokenDescription(token, strToken);
		str=Format(L"expected %s", strToken.sz());
		pszExpected=str;
	}

	FormatError(ERR_SYNTAX, pszExpected);
	return false;
}

bool CCppTokenizer::Skip(int token, const wchar_t* pszExpected)
{
	if (Check(token, pszExpected))
	{
		NextToken();
		return true;
	}
	return false;
}

void CCppTokenizer::Unexpected(const wchar_t* pszWhen)
{
	if (!pszWhen)
		pszWhen=L"";

	CUniString str;
	GetTokenDescription(m_iToken, str);

	FormatError(ERR_SYNTAX, Format(L"unexpected token %s- %s", pszWhen, str.sz()).sz());
}



// Returns false if error
bool CCppTokenizer::ParseStringLiteral(bool bIgnoreErrors, CUniString& str)
{
	// String?
	if (m_pszPos[0]!='\"')
		return false;

	// Parse the string
	CUniString buf;

ContinueString:
	// Skip opening quote
	m_pszPos++;

	// Parse string segment
	while (m_pszPos[0] && !IsEOL(m_pszPos[0]) && m_pszPos[0]!='\"')
	{
		if (m_pszPos[0]=='\\')
		{
			switch (m_pszPos[1])
			{
				case 'r': buf.Append(L"\r"); break;
				case 'n': buf.Append(L"\n"); break;
				case 't': buf.Append(L"\t"); break;
				case '\"': buf.Append(L"\""); break;
				case '\'': buf.Append(L"\'"); break;
				case '\\': buf.Append(L"\\"); break;
				case '0': buf.Append(L"\0"); break;

				default:
					str=Format(L"Unrecognised escape sequence in string '\\%c'", m_pszPos[1]);
					return true;
			}

			m_pszPos+=2;
		}
		else
		{
			// Append character
			buf.Append(m_pszPos, 1);
			m_pszPos++;
		}
	}

	// Unterminated?
	if (!m_pszPos[0] || IsEOL(m_pszPos[0]))
	{
		if (!bIgnoreErrors)
			FormatError(ERR_EOFINSTRING);
		return false;
	}

	// Skip terminator
	m_pszPos++;

	// Skip any white space
	SkipWhiteSpace();

	// Continued string?
	if (m_pszPos[0]=='\"')
	{
		goto ContinueString;
	}

	// Save string literal
	str=buf;
	return true;
}

CUniString CCppTokenizer::GetTokenText()
{
	return CUniString(m_pszToken, int(m_pszPos-m_pszToken));
}

bool CCppTokenizer::GetTokenDescription(int iToken, CUniString& str)
{
	// Is it a keyword?
	if (m_mapKeywordTokens.HasKey(iToken))
	{
		str=Format(L"Keyword: %s", m_mapKeywordTokens[iToken].Value);
		return true;
	}

	// Is it a operator?
	if (m_OpMap.GetToken(iToken, str))
	{
		str=Format(L"Operator: %s", str.sz());
		return true;
	}

	// Other...
	switch (iToken)
	{
		case tokenEOF:
			str=Format(L"EOF");
			return true;

		case tokenEOL:
			str=Format(L"EOL");
			return true;

		case tokenError:
			str=Format(L"%s", GetError());
			return true;

		case tokenIdentifier:
			str=Format(L"Identifier");
			return true;

		case tokenStringLiteral:
			str=Format(L"String");
			return true;

		case tokenDoubleLiteral:
			str=Format(L"Float literal");
			return true;

		case tokenInt32Literal:
			str=Format(L"Int32 literal");
			return true;

		case tokenInt64Literal:
			str=Format(L"Int64 literal");
			return true;
	}

	return false;

}

bool CCppTokenizer::GetTokenDescription(CUniString& str)
{
	// Is it a keyword?
	if (m_mapKeywordTokens.HasKey(m_iToken))
	{
		str=Format(L"Keyword: %s", m_mapKeywordTokens[m_iToken].Value);
		return true;
	}

	// Is it a operator?
	if (m_OpMap.GetToken(m_iToken, str))
	{
		str=Format(L"Operator: %s", str.sz());
		return true;
	}

	// Other...
	switch (m_iToken)
	{
		case tokenEOF:
			str=Format(L"EOF.");
			return true;

		case tokenEOL:
			str=Format(L"EOL");
			return true;

		case tokenError:
			str=Format(L"%s", GetError());
			return true;

		case tokenIdentifier:
			str=Format(L"Identifier: %s", GetStringLiteral());
			return true;

		case tokenStringLiteral:
			str=Format(L"String: '%s'", GetStringLiteral());
			return true;

		case tokenDoubleLiteral:
			str=Format(L"Float literal (%s)", Format(L"%f", GetDoubleLiteral()).sz());
			return true;

		case tokenInt32Literal:
			str=Format(L"Int32 literal (%i)", GetInt32Literal());
			return true;

		case tokenInt64Literal:
			str=Format(L"Int32 literal (%I64i)", GetInt64Literal());
			return true;
	}

	return false;
}


} // namespace Simple

