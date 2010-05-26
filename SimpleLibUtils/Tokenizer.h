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
// Tokenizer.h - declaration of CTokenizer class

#ifndef __TOKENIZER_H
#define __TOKENIZER_H


namespace Simple
{

// Miscellaneous parser utilities
int SIMPLEAPI ParseHexChar(wchar_t ch);
bool SIMPLEAPI ParseHexU(const wchar_t*& p, uint64_t& iVal, bool& bOverflow);
bool SIMPLEAPI ParseIntU(const wchar_t*& p, uint64_t& iVal, bool& bOverflow);
bool SIMPLEAPI IsDoubleU(const wchar_t* p);
bool SIMPLEAPI ParseDoubleU(const wchar_t*& p, double& dblVal, bool& bOverflow);
bool SIMPLEAPI ParseDouble(const wchar_t*& psz, double& dblVal, bool& bError);
bool SIMPLEAPI ParseInt64(const wchar_t*& psz, int64_t& iVal, bool& bError);


// Tokens
enum Token
{
	tokenEOF,
	tokenEOL,					// Only if ReportLineBreaks(true)
	tokenError,
	tokenIdentifier,
	tokenStringLiteral,
	tokenDoubleLiteral,
	tokenInt32Literal,
	tokenInt64Literal,
	tokenScannedText,

	tokenOperatorBase=1000,
	tokenKeywordBase=2000,
	tokenPreprocessorBase=3000,
};

class COperatorChar;

// A map of characters to operator character entries...
class COperatorMap : public CIndex<wchar_t, COperatorChar*, SValue, SOwnedPtr >
{
	typedef CIndex<wchar_t, COperatorChar*, SValue, SOwnedPtr> base;
public:

	void Add(const wchar_t* psz, int iToken);
	int Parse(const wchar_t*& p);
	bool GetToken(int iToken, CUniString& str);
};

// Represents a single character in an operator, with a map of subsequent possible characters
class COperatorChar
{
public:
	wchar_t			m_ch;
	int				m_iToken;		// 0 if not a valid token by itself
	COperatorMap	m_mapNext;

	bool GetToken(int iToken, CUniString& str);
};

enum TypeQualifier
{
	tqNone,					// No qualifier
	tqUnicodeString,		// String was encoded as UNICODE (ie: L prefix)
	tqFloat,				// floating pointer number was followed by 'f'
	tqRgb,					// integer was specified in #rrggbb format
};


// Abstract tokenizer class
class CTokenizer
{
public:
	virtual int CurrentToken()=0;;
	virtual int NextToken()=0;;
	virtual void SetError(const wchar_t* pszMessage, const wchar_t* pszFileName=NULL, int iLine=-1)=0;;
	virtual const wchar_t* GetError()=0;
	virtual const wchar_t* GetStringLiteral()=0;;
	virtual int GetInt32Literal()=0;;
	virtual int64_t GetInt64Literal()=0;;
	virtual double GetDoubleLiteral()=0;;
	virtual TypeQualifier GetTypeQualifier()=0;
};

class CContentProvider
{
public:
	virtual bool GetFileContent(
				const wchar_t* pszCurrentFileName,
				const wchar_t* pszFileName,
				bool bSysPath,
				CUniString& strQualifiedFileName,
				CUniString& strContent
				)=0;
};

class CFileContentProvider : public CContentProvider
{
public:
// Construction
			CFileContentProvider();
	virtual ~CFileContentProvider();

// CContentProvider
	virtual bool GetFileContent(
				const wchar_t* pszCurrentFileName,
				const wchar_t* pszFileName,
				bool bSysPath,
				CUniString& strQualifiedFileName,
				CUniString& strContent
				);

// Operations
	void AddIncludePath(const wchar_t* pszPath, bool bAddToHead=false);
	void SetNextContentProvider(CContentProvider* pContentProvider);

protected:
	CVector<CUniString>	m_vecIncludePath;
	CContentProvider*	m_pNext;
};


// CCppTokenizer Class
class CCppTokenizer : public CTokenizer
{
public:
// Construction
			CCppTokenizer();
	virtual ~CCppTokenizer();

// Attributes
	bool ParseString(const wchar_t* pszScript, const wchar_t* pszFileName);
	bool ParseFile(const wchar_t* pszFile);
	void AddKeyword(const wchar_t* pszKeyword, int iToken);
	void AddKeywords(const wchar_t* pszKeywords, int iBaseToken=tokenKeywordBase);
	void AddOperator(const wchar_t* pszOperator, int iToken);
	void AddOperators(const wchar_t* pszOperators, int iBaseToken=tokenOperatorBase);
	void AddPreprocessorDirective(const wchar_t* pszDirective, int iToken);
	void AddPreprocessorDirectives(const wchar_t* pszDirectives, int iBaseToken=tokenPreprocessorBase);
	void SetContentProvider(CContentProvider* pContentProvider);
	CFileContentProvider* GetDefaultContentProvider();
	void ReportLineBreaks(bool bNewVal);
	CUniString GetTokenText();
	bool GetTokenDescription(CUniString& str);
	bool GetTokenDescription(int iToken, CUniString& pVal);
	int GetCurrentPosition(CUniString& strFileName);
	void SetExtraIdentifierChars(const wchar_t* pszLeadChars, const wchar_t* pszOtherChars);
	void Define(const wchar_t* pszMacro, const wchar_t* pszDef);

// CTokenizer
	virtual int CurrentToken();
	virtual int NextToken();
	virtual void SetError(const wchar_t* pszMessage, const wchar_t* pszFileName=NULL, int iLine=-1);
	virtual const wchar_t* GetError();
	virtual const wchar_t* GetStringLiteral();
	virtual int GetInt32Literal();
	virtual int64_t GetInt64Literal();
	virtual double GetDoubleLiteral();
	virtual TypeQualifier GetTypeQualifier();

	bool Check(int token, const wchar_t* pszError=NULL);
	bool Skip(int token, const wchar_t* pszError=NULL);
	void Unexpected(const wchar_t* pszWhen=NULL);

	bool ScanForward(const wchar_t** ppszEndCondition, int iCount);

// Implementation
protected:
// Types
	class CMacro
	{
	public:
		CUniString			m_strName;
		CVector<CUniString>	m_vecParameters;
		CUniString			m_strDefinition;
		bool				m_bHasParens;
		bool				m_bExpanding;
	};
	class CState
	{
	public:
		const wchar_t*	m_pszScript;
		const wchar_t*	m_pszPos;
		CUniString	m_strScript;
		int			m_iLineNumber;
		CUniString	m_strFileName;
		CMacro*		m_pMacro;			// Macro being expanded
	};


// Attributes
	const wchar_t*			m_pszScript;
	const wchar_t*			m_pszPos;
	CUniString			m_strScript;
	CUniString			m_strString;
	int					m_iToken;
	CUniString			m_strError;
	const wchar_t*			m_pszToken;
	int64_t				m_iVal;
	double				m_dblVal;
	TypeQualifier		m_TypeQualifier;
	int					m_iLineNumber;
	CUniString			m_strFileName;
	CVector<int>		m_ConditionalState;
	CIndex<const wchar_t*, CMacro*, SValue, SOwnedPtr >	m_mapMacros;
	CIndex<const wchar_t*, int>							m_mapKeywords;
	CIndex<int, const wchar_t*>							m_mapKeywordTokens;
	CIndex<const wchar_t*, int>							m_mapPreprocessorDirectives;
	CIndex<int, const wchar_t*>							m_mapPreprocessorDirectiveTokens;
	CVector<CState*, SOwnedPtr>						m_vecStateStack;
	COperatorMap		m_OpMap;
	bool				m_bReportLineBreaks;
	CContentProvider*	m_pContentProvider;
	CFileContentProvider	m_DefaultProvider;
	CUniString			m_strExtraIdentifierLeadChars;
	CUniString			m_strExtraIdenfifierChars;

	int NextTokenInternal();
	void ParseMacro();
	bool ExpandMacro(CMacro* pMacro);
	void ParseInclude();
	void HandleIf(bool bInclude);
	void HandleElse();
	void HandleEndif();
	void SkipExcludedConditional(int iState);
	int SkipToConditionalDirective();
	void PushState(CMacro* pMacro, const wchar_t* pszFileName);
	bool PopState();
	void FormatError(int iError, ...);
	bool IsStartOfLine();
	void CheckEndOfLine();
	void IgnoreToEndOfLine();
	bool SkipComments();
	bool SkipLineSpace();
	bool SkipWhiteSpace();
	bool SkipLineSpaceAndComments();
	bool SkipWhiteSpaceAndComments();
	bool ParseStringLiteral(bool bIgnoreErrors, CUniString& str);
};

}	// namespace Simple

#endif	// __TOKENIZER_H

