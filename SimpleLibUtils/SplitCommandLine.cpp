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

#include "stdafx.h"
#include "SimpleLibUtilsBuild.h"

#include "SplitCommandLine.h"

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


}	// namespace Simple
