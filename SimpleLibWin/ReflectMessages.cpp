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
// ReflectMessages.cpp - implementation of ReflectMessages

#include "stdafx.h"
#include "SimpleLibWinBuild.h"

#include "ReflectMessages.h"


namespace Simple
{

#ifndef OCM__BASE
#define OCM__BASE           (WM_USER+0x1c00)
#endif

// This is essentially a rip off of ATLHOST.H versions of ReflectNotifications
// but available outside of	CAxHostWindow and with a couple of bugs fixed (wParam
//	checks on owner draw notifications)
LRESULT SIMPLEAPI ReflectMessages(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	HWND hWndChild = NULL;

	switch(uMsg)
		{
		case WM_COMMAND:
			if(lParam != NULL)	// not from a menu
				hWndChild = (HWND)lParam;
			break;

		case WM_NOTIFY:
			hWndChild = ((LPNMHDR)lParam)->hwndFrom;
			break;

		case WM_PARENTNOTIFY:
			switch(LOWORD(wParam))
				{
				case WM_CREATE:
				case WM_DESTROY:
					hWndChild = (HWND)lParam;
					break;

				default:
					hWndChild = GetDlgItem(hWnd, HIWORD(wParam));
					break;
				}
			break;

		case WM_DRAWITEM:
	//		if(wParam)	// not from a menu
			hWndChild = ((LPDRAWITEMSTRUCT)lParam)->hwndItem;
			break;

		case WM_MEASUREITEM:
	//		if(wParam)	// not from a menu
			hWndChild = GetDlgItem(hWnd, ((LPMEASUREITEMSTRUCT)lParam)->CtlID);
			break;

		case WM_COMPAREITEM:
	//		if(wParam)	// not from a menu
			hWndChild = GetDlgItem(hWnd, ((LPCOMPAREITEMSTRUCT)lParam)->CtlID);
			break;

		case WM_DELETEITEM:
	//		if(wParam)	// not from a menu
			hWndChild = GetDlgItem(hWnd, ((LPDELETEITEMSTRUCT)lParam)->CtlID);
			break;

		case WM_VKEYTOITEM:
		case WM_CHARTOITEM:
		case WM_HSCROLL:
		case WM_VSCROLL:
			hWndChild = (HWND)lParam;
			break;

		case WM_CTLCOLORBTN:
		case WM_CTLCOLORDLG:
		case WM_CTLCOLOREDIT:
		case WM_CTLCOLORLISTBOX:
		case WM_CTLCOLORMSGBOX:
		case WM_CTLCOLORSCROLLBAR:
		case WM_CTLCOLORSTATIC:
			hWndChild = (HWND)lParam;
			break;

		default:
			break;
		}

	if(hWndChild == NULL)
		{
		bHandled = FALSE;
		return 1;
		}

	ASSERT(::IsWindow(hWndChild));
	return ::SendMessage(hWndChild, OCM__BASE + uMsg, wParam, lParam);
}




}	// namespace Simple
