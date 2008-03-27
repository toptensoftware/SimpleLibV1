//////////////////////////////////////////////////////////////////////
//
// SimpleLib ATL COM Version 1.0
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
// AreFontsEqual.cpp - implementation of AreFontsEqual

#include "stdafx.h"
#include "SimpleLibAtlComBuild.h"

#include "AreFontsEqual.h"

namespace Simple
{


bool AreFontsEqual(IFont* p1, IFont* p2)
{
	if (!p1 || !p2)
		return false;
	TEXTMETRICOLE tm1, tm2;
	p1->QueryTextMetrics( &tm1 );
	p2->QueryTextMetrics( &tm2 );
	if (tm1.tmHeight!=tm2.tmHeight)
		return false;
	if (tm1.tmItalic!=tm2.tmItalic)
		return false;
	if (tm1.tmUnderlined!=tm2.tmUnderlined)
		return false;
	if (tm1.tmWeight != tm2.tmWeight)
		return false;
	if (tm1.tmAveCharWidth!=tm2.tmAveCharWidth)
		return false;

	CComBSTR str1, str2;
	p1->get_Name( &str1 );
	p2->get_Name( &str2 );
	return str1==str2;
}


}