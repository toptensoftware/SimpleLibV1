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
// CommandLineParser.cpp - implementation of CCommandLineParser

#include "stdafx.h"
#include "SimpleLibUtilsBuild.h"

#include "CommandLineParser.h"
#include "PathLibrary.h"
#include "SplitCommandLine.h"

namespace Simple
{

#pragma warning(disable:4996)

// Check if a character is a valid switch character
static bool IsSwitchChar(wchar_t ch)
{
	return ch==L'/' || ch==L'-';
}


// Helper to wrap and indent a string
static CUniString WrapString(const wchar_t* psz, int iTabs, int iTabSize, int iWrapWidth)
{
	CUniString buf;

	int iPos=0;

	// Find first tab...
	const wchar_t* pszTab=wcschr(psz, L'\t');
	const wchar_t* p=psz;
	if (pszTab)
		{
		// How many chars before the tab
		iPos=int(pszTab-psz);

		// Write everything before the first tab
		buf.Append(psz, iPos);

		// Write the tabs...
		p=pszTab;
		while (p[0]=='\t')
			p++;

		buf.Append(pszTab, int(p-pszTab));

		// Calculate character position
		iPos+=int(p-pszTab)*iTabSize;
		iPos-=iPos%iTabSize;
		}

	// Skip spaces...
	while (p[0]==' ')
		p++;

	// Store start of word
	const wchar_t* pszWord=p;
	while (p[0])
		{
		// Find next space...
		while (p[0] && p[0]!=' ')
			p++;

		// Work out word length
		int iLen=int(p-pszWord);

		// Start new line if doesn't fit...
		if (iPos!=0 && iPos+iLen>iWrapWidth)
			{
			buf.Append(L"\r\n");
			for (int i=0; i<iTabs; i++)
				buf.Append(L"\t");
			iPos=0;
			}

		// Append the word
		buf.Append(pszWord, iLen);
		iPos+=iLen;

		// Skip spaces....
		if (!p[0])
			break;
		const wchar_t* pszSpaces=p;
		while (p[0]==L' ')
			p++;
		
		// Append either spaces or a carriage return		
		iLen=int(p-pszSpaces);
		if (iPos+iLen>iWrapWidth)
			{
			buf.Append(L"\r\n");
			for (int i=0; i<iTabs; i++)
				buf.Append(L"\t");
			iPos=0;
			}
		else
			{
			buf.Append(pszSpaces, iLen);
			iPos+=iLen;
			}


		pszWord=p;
		}


	return buf;
}


//////////////////////////////////////////////////////////////////////////
// CCommandLineParser

// Constructor
CCommandLineParser::CCommandLineParser()
{
	m_bCaseSensitive=false;
	m_iHelpTabs=2;
	m_bInHelpGroup=false;
}

// Destructor
CCommandLineParser::~CCommandLineParser()
{
}

// Parse a command line from string
bool CCommandLineParser::Parse(const wchar_t* pszCommandLine, bool bHasExe)
{
	// Split command line
	CVector<CUniString> vecArgs;
	SplitCommandLine(pszCommandLine, vecArgs);
	return AddArgs(vecArgs, bHasExe);
}


// Parse commands from a text file
bool CCommandLineParser::ParseFile(const wchar_t* pszFileName)
{
	// Fully qualify the path
	CUniString strResponseFile=QualifyPath(pszFileName);

	// Load text file
	CUniString strResponseText;
	if (LoadTextFile<wchar_t>(strResponseFile, strResponseText))
		{
		m_strErrorMessage=Format(L"Failed to load response file '%s'", strResponseFile);
		return false;
		}
	else
		{
		// Recurse...
		Parse(strResponseText, false);
		}

	return true;
}

/*
// Parse commands from an environment variable, of same name as exe (with path/extension removed)
bool CCommandLineParser::ParseEnvironment(const wchar_t* pszVariable)
{
USES_CONVERSION;

	// Work out environment variable name
	CComBSTR bstr;
	if (!pszVariable)
		{
		TCHAR szTemp[MAX_PATH];
		GetModuleFileName(NULL, szTemp, MAX_PATH);
		ExtractFileTitle(T2COLE(szTemp), &bstr);
		pszVariable=bstr;
		}

	// Get environment variable
	TCHAR szTemp[MAX_PATH];
	if (GetEnvironmentVariable(OLE2CT(pszVariable), szTemp, MAX_PATH))
		{
		// Parse it
		Parse(T2COLE(szTemp), false);
		}

	// Done
	return true;
}

*/


// Add an alias for a switch eg: AddAlias(L"help", L"?")
void CCommandLineParser::AddAlias(const wchar_t* pszArg, const wchar_t* pszAlias)
{
	// Add to alias map
	ASSERT(m_Args.GetSize()==0);
	m_Aliases.Add(pszAlias, pszArg);
}

// Check zero or one of arg is specified. eg: CheckExclusive("reg|unreg");
bool CCommandLineParser::CheckExclusive(const wchar_t* pszArgs)
{
	// Check not already in error
	if (HasError())
		return false;

	// Split arg names
	CVector<CUniString> vec;
	SplitString(pszArgs, L";|", vec);

	// Check only one is used
	CUniString strUsed;
	for (int i=0; i<vec.GetSize(); i++)
		{
		// Find it
		CArg* pArg;
		if (!m_Args.Find(MapArgName(vec[i]), pArg))
			continue;

		// Multiple ones used?
		if (strUsed)
			{
			m_strErrorMessage=Format(L"Inconsistant use of args: '%s' and '%s'", strUsed, pArg->m_strName);
			return false;
			}

		// Remember which has been used
		strUsed=pArg->m_strName;
		}

	// Done
	return true;
}

// Keep the most recently specified argument
bool CCommandLineParser::MakeExclusive(const wchar_t* pszArgs)
{
	// Split arg names
	CVector<CUniString> vec;
	SplitString(pszArgs, L";|", vec);

	// Check all args...
	CArg* pPrev=NULL;
	for (int i=0; i<vec.GetSize(); i++)
		{
		// Find it
		CArg* pArg;
		if (!m_Args.Find(MapArgName(vec[i]), pArg))
			continue;

		if (pPrev)
			{
			// Keep the most recently specified one...
			if (pPrev->m_iOrder > pArg->m_iOrder)
				{
				m_Args.Remove(pArg->m_strName);
				}
			else
				{
				m_Args.Remove(pPrev->m_strName);
				pPrev=pArg;
				}
			}
		else	
			{
			// Remember the previous
			pPrev=pArg;
			}

		// Remember that this arg has been used
		pPrev->m_bUsed=true;
		}

	return true;
}

// Check only one of an argument is specified eg: CheckOneOf("yes|no");
bool CCommandLineParser::CheckOneOf(const wchar_t* pszArgs)
{
	// Check not already in error
	if (HasError())
		return false;

	// Split args
	CVector<CUniString> vec;
	SplitString(pszArgs, L";|", vec);

	// Check only one is used...
	CUniString strUsed;
	for (int i=0; i<vec.GetSize(); i++)
		{
		CArg* pArg;
		if (!m_Args.Find(MapArgName(vec[i]), pArg))
			continue;

		if (strUsed)
			{
			m_strErrorMessage=Format(L"Inconsistant use of args: '%s' and '%s'", strUsed, pArg->m_strName);
			return false;
			}

		strUsed=pArg->m_strName;
		}

	// If none are used, generate an error
	if (strUsed.IsEmpty())
		{
		m_strErrorMessage=L"Required argument missing, please specify one of ";
		for (int i=0; i<vec.GetSize(); i++)	
			{
			if (i==vec.GetSize()-1)
				m_strErrorMessage+=L" or ";
			else if (i>0)
				m_strErrorMessage+=L", ";

			m_strErrorMessage+=Format(L"'%s'", MapArgName(vec[i]));
			}
		return false;
		}

	return true;
}

// Check if a switch is specified. eg: GetSwitch(L"unreg")
bool CCommandLineParser::GetSwitch(const wchar_t* pszArg, bool bDefault)
{
	// Get the arg
	CArg* pArg;
	if (!m_Args.Find(MapArgName(pszArg), pArg))
		return bDefault;

	// Mark as used...
	pArg->m_bUsed=true;

	// Check it is a switch
	if (!pArg->IsSwitch())
		{
		if (!HasError())
			{
			m_strErrorMessage=Format(L"Invalid argument - switch '%s' doesn't accept values", pArg->m_strName);
			}
		return false;
		}

	// Done
	return true;
}

// Get a named value eg: GetValue(L"target", &bstrTarget) where command line is /target:<value>
bool CCommandLineParser::GetValue(const wchar_t* pszArg, CUniString& str, const wchar_t* pszDefault, bool bCanOverride)
{
	// Look up arg...
	CArg* pArg;
	if (!m_Args.Find(MapArgName(pszArg), pArg))
		{
		// Not found, generate error or use default if specified...
		if (pszDefault)
			{
			str=pszDefault;
			return true;
			}

		if (!HasError())
			{
			m_strErrorMessage=Format(L"Missing required argument '%s'", MapArgName(pszArg));
			}
		return false;
		}

	// Mark as used
	pArg->m_bUsed=true;

	// Check compatible type...
	if (pArg->IsSwitch())
		{
		if (!HasError())
			{
			m_strErrorMessage=Format(L"Missing argument value - '%s'", pArg->m_strName);
			}
		return false;
		}
			
	// If multi, either allow override and use latest one, or generate error
	if (pArg->IsMulti())
		{
		if (bCanOverride)
			{
			while (pArg->IsMulti())
				{
				pArg->m_vecValues.RemoveAt(0);
				}
			}
		else
			{
			if (!HasError())
				{
				m_strErrorMessage=Format(L"Multiple values specified for argument '%s'", pArg->m_strName);
				}
			return false;
			}
		}

	// Return value...
	str=pArg->m_vecValues[pArg->m_vecValues.GetSize()-1];
	return true;
}

// Get multiple named values. eg: /target:value1 /target:value2 etc...
bool CCommandLineParser::GetMultiValue(const wchar_t* pszArg, CVector<CUniString>& vec)
{
	// Check not already in error
	if (HasError())
		return false;

	// Find arg
	CArg* pArg;
	if (!m_Args.Find(MapArgName(pszArg), pArg))
		return false;

	// Mark as used...
	pArg->m_bUsed=true;

	// Check compatible type...
	if (pArg->IsSwitch())
		{
		if (!HasError())
			{
			m_strErrorMessage=Format(L"Missing argument value - '%s'", pArg->m_strName);
			}
		return false;
		}

	// Return all values...
	for (int i=0; i<pArg->m_vecValues.GetSize(); i++)
		{
		vec.Add(pArg->m_vecValues[i]);
		}

	// Done
	return true;
}


// Get an unnamed placed value from command line eg: GetPlacedValue(0, "filename", &bstrFileName);
bool CCommandLineParser::GetPlacedValue(int iPlace, const wchar_t* pszName, CUniString& str)
{
	ASSERT(iPlace>=0);
	
	// Store arg name for GetSettings
	m_PlacedValueNames.Add(iPlace, pszName);

	// Check specified
	if (iPlace>=m_vecPlacedValues.GetSize())
		{
		if (!HasError())
			{
			m_strErrorMessage=Format(L"Missing argument - '%s'", MapArgName(pszName));
			}
		return false;
		}

	// Return it
	str=m_vecPlacedValues[iPlace];
	return true;
}

// Get an optional unnamed placed value from command line
bool CCommandLineParser::GetOptionalPlacedValue(int iPlace, const wchar_t* pszName, const wchar_t* pszDefault, CUniString& str)
{
	// Store arg name for GetSettings
	m_PlacedValueNames.Add(iPlace, pszName);

	// Check specified...
	if (iPlace>=m_vecPlacedValues.GetSize())
		{
		str=pszDefault;
		return true;
		}

	// Return it
	str=m_vecPlacedValues[iPlace];
	return true;
}

int MyCompareI(const CUniString& a, CUniString b)
{
	return CompareI(a,b);
}

// Get an enumerated option eg: GetOption(L"mode", L"fast|slow", 1) where command line is /mode:fast
int CCommandLineParser::GetOption(const wchar_t* pszArg, const wchar_t* pszOptions, int iDefault)
{
	// Check not already in error
	if (HasError())
		return -1;

	// Split options
	CVector<CUniString> vec;
	SplitString(pszOptions, L"|;", vec);
	ASSERT(vec.GetSize()>1);

	// Get value...
	CUniString strValue;
	if (!GetValue(pszArg, strValue, iDefault<0 ? NULL : vec[iDefault]))
		return -1;

	// Look up value name
	int iValue;
	if (m_bCaseSensitive)
		{
		iValue=vec.Find(strValue);
		}
	else
		{
		iValue=vec.FindKey(strValue, MyCompareI);
		}

	// Check valid
	if (iValue<0)
		{
		m_strErrorMessage=Format(L"Invalid value for argument '%s', please specify one of ", MapArgName(pszArg));
		for (int i=0; i<vec.GetSize(); i++)	
			{
			if (i==vec.GetSize()-1)
				m_strErrorMessage+=L" or ";
			else if (i>0)
				m_strErrorMessage+=L", ";

			m_strErrorMessage+=Format(L"'%s'", MapArgName(vec[i]));
			}
		return false;
		}

	// Done
	return iValue;
}

// Check all arguments were processed
bool CCommandLineParser::CheckNoUnknownArgs()
{
	// Quit if already in error
	if (HasError())
		return false;

	// Check named args...
	for (int i=0; i<m_Args.GetSize(); i++)
		{
		CArg* pArg=m_Args[i].Value;
		if (!pArg->m_bUsed)
			{
			m_strErrorMessage=Format(L"Unrecognised argument '%s'", pArg->m_strName);
			return false;
			}
		}

	// Check unnamed args
	for (int i=0; i<m_vecPlacedValues.GetSize(); i++)
		{
		if (!m_PlacedValueNames.HasKey(i))
			{
			m_strErrorMessage=Format(L"Unexpected argument - '%s'", m_vecPlacedValues[i]);
			return false;
			}
		}

	// Done
	return true;
}

// Check if an error
bool CCommandLineParser::HasError()
{
	return !IsEmptyString(m_strErrorMessage);
}

// Set user specified error
void CCommandLineParser::SetError(const wchar_t* psz)
{
	// Store error unless already have an error...
	if (!HasError())
		{
		m_strErrorMessage=psz;
		}
}

// Get error message
const wchar_t* CCommandLineParser::GetErrorMessage()
{
	return m_strErrorMessage;
}

// Get a string representing all argument values...
void CCommandLineParser::GetSettings(CUniString& buf)
{
	// Format all named args	
	for (int i=0; i<m_Args.GetSize(); i++)	
		{
		CArg* pArg=m_Args[i].Value;

		if (pArg->IsSwitch())
			{
			buf.Append(L"/");
			buf.Append(pArg->m_strName);
			}
		else
			{
			for (int i=0; i<pArg->m_vecValues.GetSize(); i++)
				{
				buf.Append(L"/");
				buf.Append(pArg->m_strName);
				buf.Append(L":");
				buf.Append(pArg->m_vecValues[i]);
				}
			}
	
		buf.Append(L"\r\n");
		}

	// Format all placed args
	for (int i=0; i<m_vecPlacedValues.GetSize(); i++)
		{
		CUniString strName;
		if (m_PlacedValueNames.Find(i, strName))
			{
			buf+=Format(L"%s: %s\n", strName, m_vecPlacedValues[i]);
			}
		else
			{
			buf+=Format(L"arg%i: %s\n", i, m_vecPlacedValues[i]);
			}
		}
}


// Set number of tabs to indent help descriptions by
void CCommandLineParser::SetHelpTabs(int iTabs)
{
	m_iHelpTabs=iTabs;
}

// Build help string
void CCommandLineParser::BuildHelp(const wchar_t* pszArg, const wchar_t* pszDescription, const wchar_t* pszValue, unsigned int dwFlags)
{
	// Start with a space
	m_bufCommandLine.Append(L" ");

	// Arg info is indented by 2 spaces
	CUniString buf(L"  ");

	// Handle start and continued groups
	if (dwFlags & clStartGroup)
		{
		m_bufCommandLine.Append(L"[ ");
		m_bInHelpGroup=true;
		}
	else if (m_bInHelpGroup)
		{
		m_bufCommandLine.Append(L"| ");
		}

	// Optional?
	if (dwFlags & clOptional)
		{
		m_bufCommandLine.Append(L"[");
		}

	// For switches and named args, include "/"
	if ((dwFlags & clPlaced)==0)
		{
		m_bufCommandLine.Append(L"/");
		buf.Append(L"/");
		}

	// Insert the argument name...
	m_bufCommandLine.Append(FormatAlias(pszArg));		// FormatAlias includes the arg aliases
	buf.Append(pszArg);

	// Tab to m_iHelpTabs
	int iLen=int(buf.GetLength());
	while (iLen < 8 * m_iHelpTabs)
		{
		buf.Append(L"\t");
		iLen+=8;
		}
	buf.Append(pszDescription);

	// Wrap long descriptions over multiple lines and indent appropriately
	m_bufArgInfo.Append(WrapString(buf, m_iHelpTabs, 8, 79));
	m_bufArgInfo.Append(L"\r\n");

	// Include value placeholder...
	if (dwFlags & clValue)
		{
		m_bufCommandLine.Append(L":");
		m_bufCommandLine.Append(pszValue ? pszValue : L"value");
		}

	// Close optional brackets
	if (dwFlags & clOptional)
		{
		m_bufCommandLine.Append(L"]");
		}

	// Handle end of group
	if (dwFlags & clEndGroup)
		{
		m_bufCommandLine.Append(L" ]");
		m_bInHelpGroup=false;
		}
}

// Build a string representing a command line arg and all its alias.
CUniString CCommandLineParser::FormatAlias(const wchar_t* pszArg)
{
	CUniString buf;
	buf.Append(pszArg);

	for (int i=0; i<m_Aliases.GetSize(); i++)
		{
		if (IsEqualString(m_Aliases[i].Value, ApplyCase(pszArg)))
			{
			buf.Append(L"|");
			buf.Append(m_Aliases[i].Key);
			}
		}

	return buf;
}


// Create the full help string
CUniString CCommandLineParser::GetHelp(const wchar_t* pszAppName)
{
	CUniString str;
	str=L"Usage: ";
	str+=pszAppName;
	str+=m_bufCommandLine;
	str=WrapString(str, 1, 8, 79);
	str+=L"\r\n\r\n";
	str+=m_bufArgInfo;
	str+=L"\r\n";
	return str;
}

// Add arguments to the command line
bool CCommandLineParser::AddArgs(CVector<CUniString>& Args, bool bIgnoreFirst)
{
	// Quit if already have error
	if (HasError())
		return false;

	// Add each argument...
	for (int i=bIgnoreFirst ? 1 : 0; i<Args.GetSize(); i++)
		{
		// Get the arg...
		const wchar_t* pszArg=Args[i];

		// Response file?
		if (pszArg[0]==L'@')
			{
			// Response file!
			pszArg++;

			// Load it
			ParseFile(pszArg);

			continue;
			}
		
		// Is it a switch
		if (IsSwitchChar(pszArg[0]))
			{
			pszArg++;

			const wchar_t* pszValueSep=wcschr(pszArg, L':');

			CUniString strName;
			CUniString strValue;
			if (!pszValueSep)
				{
				// Its a switch
				strName=MapArgName(pszArg);
				}
			else
				{
				// Its a named value...
				strName=MapArgName(CUniString(pszArg,int(pszValueSep-pszArg)));
				strValue=pszValueSep+1;
				}

			// Existing argument name?
			CArg* pArg;
			if (m_Args.Find(strName, pArg))
				{
				if (pArg->IsSwitch()!=!pszValueSep)
					{
					m_strErrorMessage=Format(L"Conflicting use of argument '%s'", strName);
					continue;
					}
				}
			else
				{
				// Create a new argument
				pArg=new CArg(strName, m_Args.GetSize());

				// Add to map
				m_Args.Add(pArg->m_strName, pArg);
				}

			// Store value
			if (pszValueSep)
				{
				pArg->m_vecValues.Add(strValue);
				}

			continue;
			}

		// Placed value...
		m_vecPlacedValues.Add(pszArg);
		}

	return true;
}

// Map alias names back to real argument names
CUniString CCommandLineParser::MapArgName(const wchar_t* psz)
{
	CUniString str=ApplyCase(psz);
	CUniString strAlias;
	if (m_Aliases.Find(str, strAlias))
		return strAlias;
	else
		return str;
}

// For case insensitive mode, map strings to lower case...
CUniString CCommandLineParser::ApplyCase(const wchar_t* psz)
{
	if (!m_bCaseSensitive)
	{
		CUniString str(psz);
		_wcslwr(str.GetBuffer(0));
		return str;
	}

	return psz;
}



}	// namespace Simple
