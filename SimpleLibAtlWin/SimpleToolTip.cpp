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
// SimpleToolTip.cpp - implementation of CSimpleToolTip class

#include "stdafx.h"
#include "SimpleLibAtlWinBuild.h"

#include "SimpleToolTip.h"

namespace Simple
{

/////////////////////////////////////////////////////////////////////////////
// CSimpleToolTip

CSimpleToolTip::CSimpleToolTip()
{
	m_hFont=(HFONT)GetStockObject(DEFAULT_GUI_FONT);
}

CSimpleToolTip::~CSimpleToolTip()
{
	if (m_bTimerSet && m_hWnd)
		{
		KillTimer(1);
		m_bTimerSet=false;
		}

	if (m_hWnd)
		DestroyWindow();
}

bool CSimpleToolTip::Create(HWND hWndParent)
{
	m_hWndParent=hWndParent;

	// Create tool tip window
	return !!CWindowImpl<CSimpleToolTip>::Create(hWndParent, SRect(0,0,0,0), NULL, WS_POPUP|WS_BORDER, WS_EX_TOPMOST|WS_EX_TOOLWINDOW, 0U, NULL);
}

void CSimpleToolTip::Update(const wchar_t* pszText, POINT pt, bool bShow)
{
	if (bShow)
		{
		Show(pszText, pt);
		return;
		}

	// Quit if not visible
	if (!IsWindowVisible())
		return;

	// Quit if same string
	if (IsEqualString(pszText, m_bstrText))
		return;

	if (IsEmptyString(pszText))
		Hide();
	else
		Show(pszText, pt);
}

void CSimpleToolTip::Show(const wchar_t* pszText, POINT pt, RECT* prcBounds)
{
	ASSERT(m_hWnd);

	if (!IsParentActive())
		return;

	// Store text...
	m_bstrText=pszText;

	SIZE size=MeasureDrawText(NULL, m_hFont, m_bstrText);

	RECT rc=SRect(pt, size);
	
	if (prcBounds)
		m_rcBounds=*prcBounds;
	else
		{
		::GetWindowRect(m_hWndParent, &m_rcBounds);
		}

	if (IsEmptyString(pszText))
		OffsetRect(&rc, -1000, -1000);

	rc.right+=4;
	rc.bottom+=4;
	SetWindowPos(NULL, &rc, SWP_SHOWWINDOW|SWP_NOACTIVATE|SWP_NOZORDER);

	Invalidate();

	SetTimer(1, 100);
	m_bTimerSet=true;

}

void CSimpleToolTip::Hide()
{
	if (m_hWnd)
		ShowWindow(SW_HIDE);

	if (m_bTimerSet)
		{
		KillTimer(1);
		m_bTimerSet=false;
		}
}

// WM_ERASEBKGND Handler
LRESULT CSimpleToolTip::OnEraseBkGnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	HDC hDC=(HDC)wParam;
	SetBkMode(hDC, TRANSPARENT);

	CSelectObject hOldFont(hDC, m_hFont);

	RECT rc;
	GetClientRect(&rc);

	FillRect(hDC, &rc, GetSysColorBrush(COLOR_INFOBK));

	SetTextColor(hDC, GetSysColor(COLOR_INFOTEXT));
	DrawText(hDC, m_bstrText, -1, &rc, DT_SINGLELINE|DT_VCENTER|DT_CENTER);

	return TRUE;
}



// WM_SETFONT Handler
LRESULT CSimpleToolTip::OnSetFont(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	m_hFont=(HFONT)wParam;
	return 0;
}

// WM_NCHITTEST Handler
LRESULT CSimpleToolTip::OnNCHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return HTTRANSPARENT;
}

// Work out if parent window active
bool CSimpleToolTip::IsParentActive()
{
	HWND hWnd=m_hWndParent;
	while (::GetWindowLong(hWnd, GWL_STYLE) & WS_CHILD)
		{
		hWnd=::GetParent(hWnd);
		}

	if (::IsWindow(hWnd))
		return GetForegroundWindow()==hWnd;

	return false;
}

// WM_TIMER Handler
LRESULT CSimpleToolTip::OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (!IsParentActive())
		{
		Hide();
		return 0;
		}

	if (!IsRectEmpty(&m_rcBounds))
		{
		POINT pt;
		GetCursorPos(&pt);
		if (!PtInRect(&m_rcBounds, pt))
			{
			Hide();
			return 0;
			}
		}

	return 0;
}




}	// namespace Simple
