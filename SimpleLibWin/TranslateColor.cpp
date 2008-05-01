//////////////////////////////////////////////////////////////////////
//
// SimpleLib Win Version 1.0
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
// TranslateColor.cpp - implementation of TranslateColor

#include "stdafx.h"
#include "SimpleLibWinBuild.h"

#include "TranslateColor.h"

namespace Simple
{


COLORREF SIMPLEAPI TranslateColor(OLE_COLOR oc)
{
	if (oc & 0x80000000)
		{
		return GetSysColor(oc & 0x0000FFFF);
		}
	else
		{
		return oc;
		}
}



}	// namespace Simple
