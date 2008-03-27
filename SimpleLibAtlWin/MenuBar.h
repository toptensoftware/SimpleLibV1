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

#ifndef __MENUBAR_H
#define __MENUBAR_H

#include "MenuBarApi.h"
#include "RegisteredWindow.h"

namespace Simple
{

// CMenuBar Class
class CMenuBar : public CRegisteredWindowImpl<CMenuBar>
{
public:
// Construction
			CMenuBar();
	virtual ~CMenuBar();

// Class definition
START_DECLARE_WND_CLASS(WC_TTSMENUBAR)
	wc.m_wc.style=CS_DBLCLKS|CS_HREDRAW;
	wc.m_wc.hbrBackground=(HBRUSH)((COLOR_BTNFACE) + 1);
END_DECLARE_WND_CLASS()

BEGIN_MSG_MAP(CMenuBar)
	MESSAGE_HANDLER(WM_CREATE, OnCreate)
	MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
	MESSAGE_HANDLER(WM_PAINT, OnPaint)
	MESSAGE_HANDLER(WM_SETFONT, OnSetFont)
	MESSAGE_HANDLER(WM_GETFONT, OnGetFont)
	MESSAGE_HANDLER(WM_GETDLGCODE, OnGetDlgCode)
	MESSAGE_HANDLER(MBM_SETMENU, OnSetMenu)
	MESSAGE_HANDLER(MBM_GETMENU, OnGetMenu)
	MESSAGE_HANDLER(WM_SIZE, OnSize)
	MESSAGE_HANDLER(MBM_REDRAW, OnRedraw)
	MESSAGE_HANDLER(WM_LBUTTONDOWN, OnLButtonDown)
	MESSAGE_HANDLER(TPM_SETPARENT, OnSetParent)
	MESSAGE_HANDLER(WM_MOUSEMOVE, OnMouseMove)
	MESSAGE_HANDLER(WM_MOUSELEAVE, OnMouseLeave)
	MESSAGE_HANDLER(MBM_PRETRANSLATEMESSAGE, OnPreTranslateMessage)
	MESSAGE_HANDLER(MBM_GETHEIGHT, OnGetHeight)
	MESSAGE_HANDLER(MBM_SETMDIMODE, OnSetMdiMode)
	MESSAGE_HANDLER(WM_LBUTTONUP, OnLButtonUp)
	MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBkGnd)
	MESSAGE_HANDLER(WM_LBUTTONDBLCLK, OnLButtonDblClk)
ALT_MSG_MAP(1)
	MESSAGE_HANDLER(WM_ACTIVATE, OnParentActivate)
	MESSAGE_HANDLER(WM_MENUSELECT, OnParentMenuSelect)
	MESSAGE_HANDLER(WM_INITMENUPOPUP, OnParentInitMenuPopup)
END_MSG_MAP()

// Message handlers
	LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnSetFont(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnGetFont(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnGetDlgCode(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnSetMenu(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnGetMenu(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnRedraw(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnSetParent(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnMouseLeave(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnParentMenuSelect(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnPreTranslateMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnParentActivate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnGetHeight(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnSetMdiMode(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnLButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnEraseBkGnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnLButtonDblClk(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnParentInitMenuPopup(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	// Handler Insert Pos(CMenuBar)

// Implementation
protected:
// Attributes
	HWND		m_hWndParent;
	HFONT		m_hFont;
	CSmartHandle<HFONT> m_hMenuFont;
	HMENU		m_hMenuBar1;
	bool		m_bReleaseMenu;
	int			m_iTrackingItem;
	bool		m_bCollapsedTrack;
	bool		m_bTrackingMouseLeave;
	int			m_iSelectedItem;
	int			m_iNextTrackItem;
	bool		m_bNextTrackCollapsed;
	bool		m_bTrackingSubMenu;
	bool		m_bOnSubMenu;
	bool		m_bGotMenuPress;
	POINT		m_ptMouseLast;
	CContainedWindow	m_wndParent;
	HWND		m_hWndMdiClient;
	int			m_iMdiButton;
	bool		m_bCloseByClick0;

	static HHOOK		m_hMsgHook;
	static CMenuBar*	m_pTrackingInstance;
	static LRESULT CALLBACK TrackingHookProc(int code, WPARAM wp, LPARAM lp);

	void GetMdiButtonRect(int iButton, RECT* prc);
	int HitTestMdiButton(POINT pt);
	void InvalidateMdiButton(int iButton);


	CVector<RECT*, SOwnedPtr>	m_ItemPlacement;
	void RecalcLayout();
	int HitTest(POINT pt);
	void TrackMenu(int iIndex, bool bCollapsed);
	bool TrackProc(MSG* pMsg);
	void PostKey(int iKey);
	int ItemFromMnemonic(TCHAR ch);
	HFONT GetMenuFont();

	int GetMenuItemCount();
	int GetMenuString(UINT nIDItem, LPTSTR lpString, int nMaxCount, UINT uFlag);
	HMENU GetSubMenu(int iIndex);

// Operations
};

}	// namespace Simple

#endif	// __MENUBAR_H

