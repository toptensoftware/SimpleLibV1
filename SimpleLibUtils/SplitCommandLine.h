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
// SplitCommandLine.h - declaration of SplitCommandLine

#ifndef __SPLITCOMMANDLINE_H
#define __SPLITCOMMANDLINE_H

namespace Simple
{

void SIMPLEAPI SplitCommandLine(const wchar_t* pszCommandLine, CVector<CUniString>& Args);

}	// namespace Simple

#endif	// __SPLITCOMMANDLINE_H

