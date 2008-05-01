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
// SetLayeredWindowAttributesEx.h - declaration of SetLayeredWindowAttributesEx

#ifndef __SETLAYEREDWINDOWATTRIBUTESEX_H
#define __SETLAYEREDWINDOWATTRIBUTESEX_H

namespace Simple
{

// Pass rgbKeyColor==0xFFFFFFFF for non keyed
// Pass bOpacity=255 for opaque
BOOL SIMPLEAPI SetLayeredWindowAttributesEx(HWND hWnd, COLORREF rgbKeyColor, BYTE bOpacity, BOOL bRedraw);


}	// namespace Simple

#endif	// __SETLAYEREDWINDOWATTRIBUTESEX_H

