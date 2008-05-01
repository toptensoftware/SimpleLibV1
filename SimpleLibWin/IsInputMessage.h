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
// IsInputMessage.h - declaration of IsInputMessage

#ifndef __ISINPUTMESSAGE_H
#define __ISINPUTMESSAGE_H

namespace Simple
{

#define WM_SYSKEYFIRST  WM_SYSKEYDOWN
#define WM_SYSKEYLAST   WM_SYSDEADCHAR
#define WM_NCMOUSEFIRST WM_NCMOUSEMOVE
#define WM_NCMOUSELAST  WM_NCMBUTTONDBLCLK

bool SIMPLEAPI IsMouseMessage(UINT msg);
bool SIMPLEAPI IsKeyboardMessage(UINT msg);
bool SIMPLEAPI IsInputMessage(UINT msg);

}	// namespace Simple

#endif	// __ISINPUTMESSAGE_H

