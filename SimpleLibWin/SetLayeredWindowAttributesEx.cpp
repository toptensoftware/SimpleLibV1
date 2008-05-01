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
// SetLayeredWindowAttributesEx.cpp - implementation of SetLayeredWindowAttributesEx

#include "stdafx.h"
#include "SimpleLibWinBuild.h"

#include "SetLayeredWindowAttributesEx.h"


namespace Simple
{



typedef WINUSERAPI BOOL (WINAPI* PFNSLWA)(HWND hwnd, COLORREF crKey, BYTE bAlpha, DWORD dwFlags);

#ifndef LWA_ALPHA
#define LWA_COLORKEY            0x00000001
#define LWA_ALPHA               0x00000002
#endif
#ifndef WS_EX_LAYERED
#define WS_EX_LAYERED           0x00080000
#endif



BOOL SIMPLEAPI SetLayeredWindowAttributesEx(HWND hWnd, COLORREF rgbKeyColor, BYTE bOpacity, BOOL bRedraw)
{
	// Doesn't work on child windows
	ASSERT((GetWindowLong(hWnd, GWL_STYLE) & WS_CHILD)==0);

	// Get address of SetLayeredWindowAttributes
	HMODULE hModule=GetModuleHandle(L"user32.dll");
	if (!hModule)
		return FALSE;
	PFNSLWA pfn=(PFNSLWA)GetProcAddress(hModule, "SetLayeredWindowAttributes");
	if (!pfn)
		return FALSE;

	// Get current style and work out new style
	DWORD dwStyle=GetWindowLong(hWnd, GWL_EXSTYLE);
	DWORD dwNewStyle = dwStyle & ~WS_EX_LAYERED;
	if (bOpacity!=255 || rgbKeyColor!=0xFFFFFFFF)
		{
		dwNewStyle |= WS_EX_LAYERED;
		}

	// Change style if necessary
	if (dwNewStyle!=dwStyle)
		SetWindowLong(hWnd, GWL_EXSTYLE, dwNewStyle);

	// Setup key and opacity value
	if (dwNewStyle & WS_EX_LAYERED)
		{
		pfn(hWnd, rgbKeyColor, bOpacity, 
				(rgbKeyColor==0xFFFFFFFF ? 0 : LWA_COLORKEY) |  
				(bOpacity==255 ? 0 : LWA_ALPHA));
		}

	// Redraw it
	if (IsWindowVisible(hWnd) && bRedraw)
		{
		RedrawWindow(hWnd, NULL, NULL, RDW_ERASE|RDW_INVALIDATE|RDW_FRAME|RDW_ALLCHILDREN);
		}

	return TRUE;
}


}	// namespace Simple
