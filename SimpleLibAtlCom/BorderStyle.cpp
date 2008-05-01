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
// BorderStyle.cpp - implementation of BorderStyle

#include "stdafx.h"
#include "SimpleLibAtlComBuild.h"

#include "BorderStyle.h"


namespace Simple
{



// Changes a window's border style
//		0 = No border
//		1 = Flat border
//		2 = 3D border
void SIMPLEAPI ChangeWindowBorderStyle(HWND hWnd, int iBorderStyle)
{
	// Change window style
	if (hWnd==NULL)
		return;

	// toggle border style 
	::SetWindowLong(hWnd, GWL_STYLE, 
		(::GetWindowLong(hWnd, GWL_STYLE) & ~WS_BORDER) | ((iBorderStyle==0) ? 0 : WS_BORDER));

	// toggle appearance style 
	::SetWindowLong(hWnd, GWL_EXSTYLE, 
		(::GetWindowLong(hWnd, GWL_EXSTYLE) & ~WS_EX_CLIENTEDGE) | (iBorderStyle==2 ? WS_EX_CLIENTEDGE : 0));

	// Redraw border
	::SetWindowPos(hWnd, NULL, 0, 0, 0, 0, SWP_DRAWFRAME | SWP_NOSIZE | SWP_NOMOVE | SWP_NOACTIVATE | SWP_FRAMECHANGED);

	// Done
}

int SIMPLEAPI GetWindowBorderStyle(HWND hWnd)
{
	DWORD dwExStyle=::GetWindowLong(hWnd, GWL_EXSTYLE);
	if (dwExStyle & WS_EX_CLIENTEDGE)
		return 2;

	DWORD dwStyle=::GetWindowLong(hWnd, GWL_STYLE);
	if (dwStyle & WS_BORDER)
		return 1;
	else
		return 0;
}


}	// namespace Simple
