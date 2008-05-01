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
// MessageBox.h - declaration of MessageBox

#ifndef __MESSAGEBOX_H
#define __MESSAGEBOX_H

namespace Simple
{

int SIMPLEAPI SlxMessageBox(HWND hWndParent, const wchar_t* pszMessage, UINT nFlags=MB_OK|MB_ICONINFORMATION, const wchar_t* pszCaption=NULL);
int SIMPLEAPI SlxMessageBox(const wchar_t* pszMessage, UINT nFlags=MB_OK|MB_ICONINFORMATION, const wchar_t* pszCaption=NULL);
int SIMPLEAPI SlxMessageBox(UINT nResID, UINT nFlags=MB_OK|MB_ICONINFORMATION, const wchar_t* pszCaption=NULL);


}	// namespace Simple

#endif	// __SLMESSAGEBOX_H

