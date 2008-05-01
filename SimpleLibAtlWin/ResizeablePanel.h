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
// ResizeablePanel.h - declaration of CResizeablePanel class

#ifndef __RESIZEABLEPANEL_H
#define __RESIZEABLEPANEL_H

#ifdef __ATLWIN_H__

#include "FrameWindow.h"

// Simple resizeable panel for use with CFrameWindow

namespace Simple
{

class CTrackMouseLeave
{
public:
	CTrackMouseLeave()
	{
		m_bTracking=false;

	}

	void OnMouseMove(HWND hWnd)
	{
		if (!m_bTracking)
			{
			TRACKMOUSEEVENT tme;
			tme.cbSize=sizeof(TRACKMOUSEEVENT);
			tme.dwFlags=TME_LEAVE;
			tme.hwndTrack=hWnd;
			tme.dwHoverTime=0;
			_TrackMouseEvent(&tme);
			m_bTracking=true;
			}
	}
	void OnMouseLeave()
	{
		m_bTracking=false;
	}

	bool m_bTracking;
};


// CResizeablePanel Class
class CResizeablePanel : public CWindowImpl<CResizeablePanel>
{
public:
// Construction
			CResizeablePanel();
	virtual ~CResizeablePanel();

	DECLARE_WND_CLASS_EX(_T("ResizeablePanel"), CS_DBLCLKS, COLOR_BTNFACE);

// Attributes
	bool Create(HWND hWndParent, LPCOLESTR pszTitle, UINT nID=0);

// Operations
	void OnFocusChanged();

// Implementation
protected:

BEGIN_MSG_MAP(CResizeablePanel)
	MESSAGE_HANDLER(WM_PAINT, OnPaint)
	MESSAGE_HANDLER(WM_SETCURSOR, OnSetCursor)
	MESSAGE_HANDLER(WM_LBUTTONDOWN, OnLButtonDown)
	MESSAGE_HANDLER(WM_MOUSEMOVE, OnMouseMove)
	MESSAGE_HANDLER(WM_LBUTTONUP, OnLButtonUp)
	MESSAGE_HANDLER(WM_CANCELMODE, OnCancelMode)
	MESSAGE_HANDLER(WM_CAPTURECHANGED, OnCaptureChanged)
	MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBkGnd)
	MESSAGE_HANDLER(WM_SIZE, OnSize)
	MESSAGE_HANDLER(WM_SETFOCUS, OnSetFocus)
	MESSAGE_HANDLER(WM_MOUSELEAVE, OnMouseLeave)
END_MSG_MAP()

	LRESULT OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnSetCursor(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnLButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnCancelMode(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnCaptureChanged(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnEraseBkGnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnSetFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnMouseLeave(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	// Handler Insert Pos(CResizeablePanel)

	void GetInternalRect(RECT* prc);
	
// Attributes
	void GetSizeBarRect(RECT* prc, DWORD* pdwPlacement);
	void GetCloseButtonRect(RECT* prc);
	POINT m_ptDragStart;
	DWORD m_dwDragPlacement;
	int m_iSizeStart;
	int m_iMaxSize;
	bool	m_bHasTitle;
	bool	m_bActive;
	int		m_iTitleHeight;
	int		m_iCloseButtonWidth;
	bool	m_bCloseButtonHot;
	bool	m_bTrackingCloseButton;
	CTrackMouseLeave	m_TrackMouseLeave;
};

}	// namespace Simple

#endif	// __ATLWIN_H__

#endif	// __RESIZEABLEPANEL_H

