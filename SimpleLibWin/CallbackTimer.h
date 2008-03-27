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
// CallbackTimer.h - declaration of TimerCallback

#ifndef __TIMERCALLBACK_H
#define __TIMERCALLBACK_H

namespace Simple
{

typedef void (CALLBACK* CALLBACKTIMERPROC)(LPARAM lUser);
typedef void* HCALLBACKTIMER;

#define CBT_ONESHOT		0x00000001
#define CBT_AUTORELEASE	0x00000002

HCALLBACKTIMER SetCallbackTimer(UINT nPeriod, DWORD dwFlags, CALLBACKTIMERPROC pfnCallback, LPARAM lUserData);
void KillCallbackTimer(HCALLBACKTIMER hTimer);

}	// namespace Simple

#endif	// __TIMERCALLBACK_H

