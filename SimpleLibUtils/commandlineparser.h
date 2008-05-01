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
// CommandLineParser.h - declaration of CommandLineParser

#ifndef __COMMANDLINEPARSER_H
#define __COMMANDLINEPARSER_H

#include "LoadSaveTextFile.h"
#include "SplitString.h"
#include "StringReplace.h"

namespace Simple
{


// Flags for building help
enum clArgHelpFlags
{
	clOptional		= 0x00000001,
	clValue			= 0x00000002,
	clMulti			= 0x00000004,
	clPlaced		= 0x00000008,
	clStartGroup	= 0x00000010,
	clEndGroup		= 0x00000020,
};

// CCommandLineParser Class
class CCommandLineParser
{
public:
// Construction
			CCommandLineParser();
	virtual ~CCommandLineParser();

// Operations
	// Parse a command line from string
	bool Parse(const wchar_t* pszCommandLine, bool bHasExe=true);

	// Parse commands from a text file
	bool ParseFile(const wchar_t* pszFileName);

	// Parse commands from an environment variable, of same name as exe (with path/extension removed)
//	bool ParseEnvironment(const wchar_t* pszVariable=NULL);

	// Add an alias for a switch eg: AddAlias(L"help", L"?")
	// pszArg should be the full name, pszAlias the short name
	void AddAlias(const wchar_t* pszArg, const wchar_t* pszAlias);

	// Check zero or one of arg is specified. eg: CheckExclusive("reg|unreg");
	bool CheckExclusive(const wchar_t* pszArgs);

	// Keep the most recently specified argument
	// Eg: MakeExclusive("Y|N")
	//		commandline: /y /n would keep /n and remove /y (because /n was last specified)
	bool MakeExclusive(const wchar_t* pszArgs);

	// Check only one of an argument is specified eg: CheckOneOf("yes|no");
	bool CheckOneOf(const wchar_t* pszArgs);

	// Check if a switch is specified. eg: GetSwitch(L"unreg")
	bool GetSwitch(const wchar_t* pszArg, bool bDefault=false);

	// Get a named value eg: GetValue(L"target", &bstrTarget) where command line is /target:<value>
	bool GetValue(const wchar_t* pszArg, CUniString& str, const wchar_t* pszDefault=NULL, bool bCanOverride=true);

	// Get multiple named values. eg: /target:value1 /target:value2 etc...
	bool GetMultiValue(const wchar_t* pszArg, CVector<CUniString>& vec);

	// Get an unnamed placed value from command line eg: GetPlacedValue(0, "filename", &bstrFileName);
	// pszArg is used for errors and GetSettings
	bool GetPlacedValue(int iPlace, const wchar_t* pszArg, CUniString& str);

	// Get an optional unnamed placed value from command line
	// pszArg is used GetSettings
	bool GetOptionalPlacedValue(int iPlace, const wchar_t* pszArg, const wchar_t* pszDefault, CUniString& str);

	// Get an enumerated option eg: GetOption(L"mode", L"fast|slow", 1) where command line is /mode:fast
	int GetOption(const wchar_t* pszArg, const wchar_t* pszOptions, int iDefault=-1);

	// Check all arguments were processed
	bool CheckNoUnknownArgs();

	// Check if an error
	bool HasError();

	// Set user specified error
	void SetError(const wchar_t* psz);

	// Get error message
	const wchar_t* GetErrorMessage();

	// Get a string representing all argument values...
	void GetSettings(CUniString& str);

	// Helpers for build usage help messages
	void SetHelpTabs(int iTabs);
	void BuildHelp(const wchar_t* pszArg, const wchar_t* pszDescription, const wchar_t* pszValue, unsigned int dwFlags);
	CUniString GetHelp(const wchar_t* pszAppName);

// Implementation
protected:
// Attributes

	// The error message
	CUniString	m_strErrorMessage;

	// Information about a single parsed argument
	class CArg
	{
	public:
		CArg(const wchar_t* pszName, int iOrder) :
			m_strName(pszName),
			m_bUsed(false),
			m_iOrder(iOrder)
		{
		};

		bool IsSwitch() { return m_vecValues.GetSize()==0; };
		bool IsMulti() { return m_vecValues.GetSize()>1; };

		bool				m_bUsed;
		int					m_iOrder;
		CUniString			m_strName;
		CVector<CUniString>	m_vecValues;
	};

// Attributes
	CIndex<CUniString, CArg*, SValue, SOwnedPtr>	m_Args;
	CIndex<CUniString, CUniString>	m_Aliases;
	CVector<CUniString>				m_vecPlacedValues;
	CIndex<int, CUniString>			m_PlacedValueNames;
	bool							m_bCaseSensitive;
	int								m_iHelpTabs;
	bool							m_bInHelpGroup;
	CUniString						m_bufCommandLine;
	CUniString						m_bufArgInfo;

// Operations
	CUniString MapArgName(const wchar_t* psz);
	CUniString ApplyCase(const wchar_t* psz);
	CUniString FormatAlias(const wchar_t* pszArg);
	bool AddArgs(CVector<CUniString>& Args, bool bIgnoreFirst=false);
};


}	// namespace Simple

#endif	// __COMMANDLINEPARSER_H

