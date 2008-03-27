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
// RunModalLoop.h - declaration of RunModalLoop

#ifndef __RUNMODALLOOP_H
#define __RUNMODALLOOP_H

namespace Simple
{

#define WM_PRETRANSLATEMESSAGE		(WM_USER+100)
#define WM_IDLE						(WM_USER+101)

int RunModalLoop(HWND hWndOwner, HWND hWndDialog);
void EndModalLoop(HWND hWndDialog, int iResult);


}	// namespace Simple

#endif	// __RUNMODALLOOP_H

