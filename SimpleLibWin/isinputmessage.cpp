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
// IsInputMessage.cpp - implementation of IsInputMessage

#include "stdafx.h"
#include "SimpleLibWinBuild.h"

#include "IsInputMessage.h"
#include "GeoOps.h"

namespace Simple
{

bool SIMPLEAPI IsMouseMessage(UINT msg)
{
	if ((msg>=WM_MOUSEFIRST) && (msg<=WM_MOUSELAST)) return true;
	if ((msg>=WM_NCMOUSEFIRST) && (msg<=WM_NCMOUSELAST)) return true;
	return false;
}

bool SIMPLEAPI IsKeyboardMessage(UINT msg)
{
	if ((msg>=WM_KEYFIRST) && (msg<=WM_KEYLAST)) return true;
	if ((msg>=WM_SYSKEYFIRST) && (msg<=WM_SYSKEYLAST)) return true;
	return false;
}

bool SIMPLEAPI IsInputMessage(UINT msg)
{
	return IsMouseMessage(msg) || IsKeyboardMessage(msg);
}


}	// namespace Simple
