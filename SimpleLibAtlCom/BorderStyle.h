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
// BorderStyle.h - declaration of BorderStyle

#ifndef __BORDERSTYLE_H
#define __BORDERSTYLE_H

namespace Simple
{

// Changes a window's border style
//		0 = No border
//		1 = Flat border
//		2 = 3D border
void SIMPLEAPI ChangeWindowBorderStyle(HWND hWnd, int iBorderStyle);
int SIMPLEAPI GetWindowBorderStyle(HWND hWnd);

}	// namespace Simple

#endif	// __BORDERSTYLE_H

