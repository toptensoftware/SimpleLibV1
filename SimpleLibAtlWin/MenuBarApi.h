//////////////////////////////////////////////////////////////////////
//
// SimpleLib ATL Win Version 1.0
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
// MenuBar.h - declaration of CMenuBar class

#ifndef __MENUBARAPI_H
#define __MENUBARAPI_H

#include "MenuBarApi.h"

#ifdef __ATLWIN_H__

namespace Simple
{

//////////////////////////////////////////////////////////////////////////
// MenuBar

// MenuBar class name
#define WC_TTSMENUBAR	 _T("ttsMenuBar")


// MBM_SETMENU
#define MBM_SETMENU		(WM_USER + 1)
#define MenuBar_SetMenu(hwnd, hMenu, bRelease) \
	(BOOL)SNDMSG((hwnd), MBM_SETMENU, (WPARAM)(HMENU)(hMenu), (LPARAM)(BOOL)(bRelease))

// MBM_GETMENU
#define MBM_GETMENU		(WM_USER + 2)
#define MenuBar_GetMenu(hwnd) \
	(HMENU)SNDMSG((hwnd), MBM_GETMENU, 0, 0)

// MBM_REDRAW
#define MBM_REDRAW		(WM_USER + 3)
#define MenuBar_Redraw(hwnd) \
	(BOOL)SNDMSG((hwnd), MBM_REDRAW, 0, 0)

// TPM_SETPARENT
#define TPM_SETPARENT		(WM_USER + 4)
#define MenuBar_SetParent(hwnd, hWndParent) \
	(BOOL)SNDMSG((hwnd), TPM_SETPARENT, (WPARAM)(HWND)(hWndParent), (LPARAM)(0))

// MBM_PRETRANSLATEMESSAGE
#define MBM_PRETRANSLATEMESSAGE		(WM_USER + 5)
#define MenuBar_PreTranslateMessage(hwnd, pMessage) \
	(BOOL)SNDMSG((hwnd), MBM_PRETRANSLATEMESSAGE, (WPARAM)(0), (LPARAM)(MSG*)(pMessage))

// MBM_GETHEIGHT
#define MBM_GETHEIGHT		(WM_USER + 6)
#define MenuBar_GetHeight(hwnd) \
	(int)SNDMSG((hwnd), MBM_GETHEIGHT, 0, 0)

// MBM_SETMDIMODE
#define MBM_SETMDIMODE		(WM_USER + 7)
#define MenuBar_SetMdiMode(hwnd, hMdiClient) \
	(BOOL)SNDMSG((hwnd), MBM_SETMDIMODE, (WPARAM)(hMdiClient), 0)

void SIMPLEAPI RegisterMenuBar();
void SIMPLEAPI UnregisterMenuBar();

/////////////////////////////////////////////////////////////////////////////
// CMenuBarCtrlT - ATL wrapper for MenuBar

template <class Base>
class CMenuBarCtrlBase : public Base
{
public:

	static LPCTSTR GetWndClassName()
	{
		return WC_TTSMENUBAR;
	}

	BOOL SetMenu(HMENU hMenu, BOOL bRelease)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return MenuBar_SetMenu(m_hWnd, hMenu, bRelease);
	}

	HMENU GetMenu()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return MenuBar_GetMenu(m_hWnd);
	}

	BOOL Redraw()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return MenuBar_Redraw(m_hWnd);
	}

	BOOL SetParent(HWND hWndParent)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return MenuBar_SetParent(m_hWnd, hWndParent);
	}

	BOOL PreTranslateMessage(MSG* pMessage)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return MenuBar_PreTranslateMessage(m_hWnd, pMessage);
	}

	int GetHeight()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return MenuBar_GetHeight(m_hWnd);
	}

	BOOL SetMdiMode(HWND hWndMdiClient)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return MenuBar_SetMdiMode(m_hWnd, hWndMdiClient);
	}
};

template <class Base>
class CMenuBarCtrlT : public CMenuBarCtrlBase<Base>
{
public:
// Constructors
	CMenuBarCtrlT(){ }

	CMenuBarCtrlT< Base >& operator=(HWND hWnd)
	{
		m_hWnd = hWnd;
		return *this;
	}

	HWND Create(HWND hWndParent, RECT& rcPos, LPCTSTR szWindowName = NULL,
			DWORD dwStyle = 0, DWORD dwExStyle = 0,
			UINT nID = 0, LPVOID lpCreateParam = NULL)
	{
		return CWindow::Create(GetWndClassName(), hWndParent, rcPos, szWindowName, dwStyle, dwExStyle, nID, lpCreateParam);
	}
	HWND Create(HWND hWndParent, LPRECT lpRect = NULL, LPCTSTR szWindowName = NULL,
			DWORD dwStyle = 0, DWORD dwExStyle = 0,
			UINT nID = NULL, LPVOID lpCreateParam = NULL)
	{
		return CWindow::Create(GetWndClassName(), hWndParent, lpRect, szWindowName, dwStyle, dwExStyle, (HMENU)nID, lpCreateParam);
	}
};

typedef CMenuBarCtrlT<CWindow>   CMenuBarCtrl;

}	// namespace Simple

#endif // __ATLWIN_H__

#endif	// __MENUBARAPI_H

