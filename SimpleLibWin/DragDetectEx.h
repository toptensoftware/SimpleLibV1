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
// DragDetectEx.h - declaration of DragDetectEx

#ifndef __DRAGDETECTEX_H
#define __DRAGDETECTEX_H

namespace Simple
{

#define DDXF_NOTIMER	0x00000001
BOOL DragDetectEx(HWND hWnd, POINT pt, DWORD dwFlags);


}	// namespace Simple

#endif	// __DRAGDETECTEX_H

