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
// ResizeablePanel.cpp - implementation of CResizeablePanel class

#include "stdafx.h"
#include "SimpleLibAtlWinBuild.h"

#include "ResizeablePanel.h"
#include "FrameWindow.h"

#define SIZEBAR_WIDTH	5

namespace Simple
{

//////////////////////////////////////////////////////////////////////////
// CResizeablePanel

// Constructor
CResizeablePanel::CResizeablePanel()
{
	m_bHasTitle=false;
	m_bActive=false;

	// Work out title height
	m_iTitleHeight=MeasureDrawText(NULL, (HFONT)GetStockObject(DEFAULT_GUI_FONT), L"X", 1, 100, 0).cy+6;

	// Work out close button width
	CSmartHandle<HFONT> hFont=CreateFontEasy(L"Marlett", 10);
	m_iCloseButtonWidth=MeasureDrawText(NULL, hFont, L"r", 1, 100, DT_LEFT).cx;
	m_bCloseButtonHot=false;
	m_bTrackingCloseButton=false;
}

// Destructor
CResizeablePanel::~CResizeablePanel()
{
}

bool CResizeablePanel::Create(HWND hWndParent, LPCOLESTR pszTitle, UINT nID)
{
USES_CONVERSION;

	RECT rc=SRect(0,0,0,0);
	m_bHasTitle=pszTitle!=NULL;
	return !!CWindowImpl<CResizeablePanel>::Create(hWndParent, rc, OLE2CT(pszTitle), WS_CHILD|WS_VISIBLE|WS_CLIPCHILDREN, 0, nID, NULL);
}

void CResizeablePanel::OnFocusChanged()
{
	bool bActive=!!IsChild(GetFocus());
	if (bActive==m_bActive)
		return;

	m_bActive=bActive;
	Invalidate();
}

// WM_PAINT Handler
LRESULT CResizeablePanel::OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	PAINTSTRUCT ps;
	HDC hDC=BeginPaint(&ps);

	RECT rc;
	GetClientRect(&rc);

	::FillRect(hDC, &rc, GetSysColorBrush(COLOR_BTNFACE));

	HBRUSH hbrDark=GetSysColorBrush(COLOR_BTNSHADOW);
	HBRUSH hbrLight=GetSysColorBrush(COLOR_BTNHILIGHT);

	DWORD dwFlags=(DWORD)::SendMessage(GetParent(), WM_BAR_GETPLACEMENT, 0, (LPARAM)m_hWnd);
	RECT rcAvailable=rc;
	switch (dwFlags)
	{
		case ASF_LEFT:
			FillVLine(hDC, 0, rc.bottom, rc.right-SIZEBAR_WIDTH-1, hbrLight);
			rcAvailable.right-=SIZEBAR_WIDTH+1;
			break;

		case ASF_RIGHT:
			FillVLine(hDC, 0, rc.bottom, SIZEBAR_WIDTH, hbrDark);
			rcAvailable.left+=SIZEBAR_WIDTH+1;
			break;

		case ASF_TOP:
			FillHLine(hDC, 0, rc.right, rc.bottom-SIZEBAR_WIDTH-1, hbrLight);
			rcAvailable.bottom-=SIZEBAR_WIDTH+1;
			break;

		case ASF_BOTTOM:
			FillHLine(hDC, 0, rc.right, SIZEBAR_WIDTH, hbrDark);
			rcAvailable.top+=SIZEBAR_WIDTH+1;
			break;
	}

	FillVLine(hDC, 0, rc.bottom, 0, hbrLight);
	FillVLine(hDC, 0, rc.bottom, rc.right-1, hbrDark);
	FillHLine(hDC, 0, rc.right, 0, hbrLight);
	FillHLine(hDC, 0, rc.right, rc.bottom-1, hbrDark);

	if (m_bHasTitle)
	{
		RECT rc=rcAvailable;

		rc.bottom=rc.top+m_iTitleHeight;
		InflateRect(&rc, -1,-1);

		FillRect(hDC, &rc, GetSysColorBrush(m_bActive ? COLOR_ACTIVECAPTION : COLOR_INACTIVECAPTION));
		SetTextColor(hDC, GetSysColor(m_bActive ? COLOR_CAPTIONTEXT : COLOR_INACTIVECAPTIONTEXT));

		CComBSTR bstr;
		GetWindowText(&bstr.m_str);
		InflateRect(&rc, -2, -2);

		if (m_bCloseButtonHot)
		{
			SetTextColor(hDC, 
					AlphaBlendColor(GetSysColor(m_bActive ? COLOR_CAPTIONTEXT : COLOR_INACTIVECAPTIONTEXT),
					GetSysColor(m_bActive ? COLOR_ACTIVECAPTION : COLOR_INACTIVECAPTION), 128));
		}

		SetBkMode(hDC, TRANSPARENT);
		CSelectObject hOldFont(hDC, GetStockObject(DEFAULT_GUI_FONT)); 
		DrawText(hDC, bstr, -1, &rc, DT_SINGLELINE|DT_VCENTER);

		GetCloseButtonRect(&rc);
		CSmartHandle<HFONT> hFont=CreateFontEasy(L"Marlett", 10);
		hOldFont.Select(hDC, hFont);
		DrawText(hDC, L"r", 1, &rc, DT_SINGLELINE|DT_VCENTER|DT_CENTER);

	}
	
	EndPaint(&ps);
	return 0;
}



void CResizeablePanel::GetInternalRect(RECT* prc)
{
	RECT rcBar;
	DWORD dwFlags;
	GetSizeBarRect(&rcBar, &dwFlags);

	RECT rcClient;
	GetClientRect(&rcClient);

	SubtractRect(prc, &rcClient, &rcBar);
}

void CResizeablePanel::GetCloseButtonRect(RECT* prc)
{
	if (!m_bHasTitle)
	{
		SetRectEmpty(prc);
		return;
	}

	RECT rc;
	GetInternalRect(&rc);
	rc.bottom=rc.top+m_iTitleHeight;
	InflateRect(&rc, -3,-3);
	rc.left=rc.right-m_iCloseButtonWidth;
	*prc=rc;
}

void CResizeablePanel::GetSizeBarRect(RECT* prc, DWORD* pdwPlacement)
{
	DWORD dwFlags=(DWORD)::SendMessage(GetParent(), WM_BAR_GETPLACEMENT, 0, (LPARAM)m_hWnd);
	*pdwPlacement=dwFlags;

	GetClientRect(prc);
	
	switch (dwFlags)
	{
	case ASF_LEFT:
		prc->left = prc->right - SIZEBAR_WIDTH-1;
		break;

	case ASF_RIGHT:
		prc->right=prc->left + SIZEBAR_WIDTH;
		break;

	case ASF_TOP:
		prc->top = prc->bottom - SIZEBAR_WIDTH-1;
		break;

	case ASF_BOTTOM:
		prc->bottom = prc->top + SIZEBAR_WIDTH;
		break;
	}
}

// WM_SETCURSOR Handler
LRESULT CResizeablePanel::OnSetCursor(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	DWORD dwFlags;
	RECT rc;
	GetSizeBarRect(&rc, &dwFlags);

	POINT pt;
	GetCursorPos(&pt);
	ScreenToClient(&pt);

	if (PtInRect(&rc, pt))
	{
		SetCursor(LoadCursor(NULL, (dwFlags==ASF_LEFT || dwFlags==ASF_RIGHT) ? IDC_SIZEWE : IDC_SIZENS));
		return TRUE;
	}

	bHandled=FALSE;
	return FALSE;
}

// WM_LBUTTONDOWN Handler
LRESULT CResizeablePanel::OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	POINT pt=GET_PT_LPARAM(lParam);

	// On the close button?
	RECT rc;
	GetCloseButtonRect(&rc);
	if (PtInRect(&rc, pt))
	{
		m_bTrackingCloseButton=true;
		SetCapture();
		return 0;
	}

	// In the size bar?
	GetSizeBarRect(&rc, &m_dwDragPlacement);
	if (!PtInRect(&rc, pt))
	{
		SetFocus();	
		return 0;
	}

	SetCapture();
	GetCursorPos(&m_ptDragStart);

	RECT rcSize;
	GetWindowRect(&rcSize);

	RECT rcSizeParent;
	::GetWindowRect(GetParent(), &rcSizeParent);
	
	if (m_dwDragPlacement==ASF_LEFT || m_dwDragPlacement==ASF_RIGHT)
	{
		m_iSizeStart=Width(rcSize);
		m_iMaxSize=Width(rcSizeParent)/2;
	}
	else
	{
		m_iSizeStart=Height(rcSize);
		m_iMaxSize=Height(rcSizeParent)/2;
	}

	return 0;
}

// WM_MOUSEMOVE Handler
LRESULT CResizeablePanel::OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	m_TrackMouseLeave.OnMouseMove(m_hWnd);

	if (GetCapture()!=m_hWnd || m_bTrackingCloseButton)
	{
		RECT rc;
		GetCloseButtonRect(&rc);
		bool bHot=!!PtInRect(&rc, GET_PT_LPARAM(lParam));
		if (bHot!=m_bCloseButtonHot)
		{
			m_bCloseButtonHot=bHot;
			InvalidateRect(&rc);
		}
		return 0;
	}

	POINT pt;
	GetCursorPos(&pt);
	
	int iNewSize;
	switch (m_dwDragPlacement)
	{
		case ASF_LEFT:
			iNewSize=m_iSizeStart + pt.x - m_ptDragStart.x;
			break;

		case ASF_RIGHT:
			iNewSize=m_iSizeStart + m_ptDragStart.x - pt.x;
			break;

		case ASF_TOP:
			iNewSize=m_iSizeStart + pt.y - m_ptDragStart.y;
			break;

		case ASF_BOTTOM:
			iNewSize=m_iSizeStart + m_ptDragStart.y - pt.y;
			break;
	}

	if (iNewSize>m_iMaxSize)
		iNewSize=m_iMaxSize;
	if (iNewSize<32)
		iNewSize=32;

	SendMessage(GetParent(), WM_BAR_RESIZEBAR, iNewSize, (LPARAM)m_hWnd);
	
	return 0;
}

// WM_LBUTTONUP Handler
LRESULT CResizeablePanel::OnLButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (GetCapture()==m_hWnd)
	{
		if (m_bTrackingCloseButton && m_bCloseButtonHot)
		{
			ShowWindow(SW_HIDE);
			::SendMessage(GetParent(), WM_BAR_RECALCLAYOUT, 0, 0);
		}
		m_bTrackingCloseButton=false;
		ReleaseCapture();
	}
	return 0;
}

// WM_CANCELMODE Handler
LRESULT CResizeablePanel::OnCancelMode(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (GetCapture()==m_hWnd)
		ReleaseCapture();
	return 0;
}

// WM_CAPTURECHANGED Handler
LRESULT CResizeablePanel::OnCaptureChanged(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return 0;
}

// WM_ERASEBKGND Handler
LRESULT CResizeablePanel::OnEraseBkGnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return TRUE;
}

// WM_SIZE Handler
LRESULT CResizeablePanel::OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	HWND hWndChild=GetWindow(GW_CHILD);
	if (hWndChild)
	{
		RECT rc;
		GetInternalRect(&rc);

		InflateRect(&rc, -3, -3);

		if (m_bHasTitle)
		{
			rc.top+=m_iTitleHeight;
		}
		::SetWindowPos(hWndChild, NULL, rc.left, rc.top, Width(rc), Height(rc), SWP_NOZORDER|SWP_NOACTIVATE);
	}
	Invalidate();
	return 0;
}


// WM_SETFOCUS Handler
LRESULT CResizeablePanel::OnSetFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	HWND hWndChild=GetWindow(GW_CHILD);
	if (hWndChild)
		::SetFocus(hWndChild);
	return 0;
}

// WM_MOUSELEAVE Handler
LRESULT CResizeablePanel::OnMouseLeave(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	m_TrackMouseLeave.OnMouseLeave();
	if (m_bCloseButtonHot)
	{
		m_bCloseButtonHot=false;
		RECT rc;
		GetCloseButtonRect(&rc);
		InvalidateRect(&rc);
	}
	return 0;
}



}	// namespace Simple
