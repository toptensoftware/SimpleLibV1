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
// SplitCommandLine.cpp - implementation of SplitCommandLine

#include "StdAfx.h"
#include "SimpleLibUtilsBuild.h"

#include "SplitCommandLine.h"
#include "LoadSaveTextFile.h"
#include "SplitString.h"

namespace Simple
{

// Split a command line, removing quotes as we go...
void SIMPLEAPI SplitCommandLine(const wchar_t* pszCommandLine, CVector<CUniString>& Args)
{
	const wchar_t* p=pszCommandLine;
	while (p[0])
		{
		// Skip white space
		while (iswspace(p[0]))
			p++;

		// Read next argument, handling quoted strings
		CUniString strArg;
		bool bInQuotes=false;
		while (p[0] && (!iswspace(p[0]) || bInQuotes))
			{
			if (p[0]==L'\"')
				{
				if (p[1]==L'\"')
					{
					strArg+=L"\"";
					p++;
					}
				else
					{
					bInQuotes=!bInQuotes;
					}
				}
			else
				{
				// Append character
				strArg.Append(p,1);
				}

			p++;
			}

		// Only interested in non-blank args
		if (!strArg.IsEmpty())
			Args.Add(strArg);
		}
}

bool SIMPLEAPI ExpandResponseFiles(CVector<CUniString>& args, CUniString& strError)
{
	for (int i=0; i<args.GetSize(); i++)
	{
		const wchar_t* pszFile=args[i];

		// Commented switch
		if (pszFile[0]=='#')
		{
			args.RemoveAt(i);
			i--;
			continue;
		}

		// Response file?
		if (pszFile[0]=='@')
		{
			pszFile++;

			bool bOptional=false;
			if (pszFile[0]=='@')
			{
				bOptional++;
				pszFile++;
			}

			// Load response file
			CUniString strResponseText;
			if (!LoadTextFile<wchar_t>(pszFile, strResponseText))
			{
				args.RemoveAt(i);
				i--;
				if (bOptional)
					continue;

				strError=Format(L"Failed to load response file '%s'", pszFile);
				return false;
			}

			// Parse args
			CVector<CUniString> vecFile;
			SplitCommandLine(strResponseText, vecFile);

			args.RemoveAt(i);
			args.InsertAt(i, vecFile);
			i--;
		}
	}

	return true;
}

bool SIMPLEAPI ParseArg(const wchar_t* pszArg, CUniString& strName, CUniString& strValue)
{
	// Is it a switch?
	if (pszArg[0]!='/' && pszArg[0]!='-')
	{
		strName=L"";
		strValue=pszArg;
		return false;
	}
	else
	{
		// Skip switch
		pszArg++;

		// Split it
		SplitString(pszArg, L":", strName, strValue);

		return true;
	}
}



}	// namespace Simple
