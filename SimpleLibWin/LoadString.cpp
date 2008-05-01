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
// LoadString.cpp - implementation of LoadString

#include "stdafx.h"
#include "SimpleLibWinBuild.h"
#include "LoadString.h"

namespace Simple
{

extern HINSTANCE SlxGetResourceInstance();

CUniString SIMPLEAPI LoadString(UINT nID)
{
	CUniString str;
	::LoadString(SlxGetResourceInstance(), nID, str.GetBuffer(MAX_PATH), MAX_PATH);
	return str;
}

}	// namespace Simple
