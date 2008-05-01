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
// Splitter.cpp - implementation of Splitter

#include "stdafx.h"
#include "SimpleLibWinBuild.h"

#include "Splitter.h"



namespace Simple
{


/////////////////////////////////////////////////////////////////////////////
// Helper to make border of a control transparent for mouse clicks

LRESULT CALLBACK TransEdgeProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	WNDPROC pfnOldProc=(WNDPROC)GetProp(hWnd, L"PreTransProc");
	LRESULT lRetv=CallWindowProc(pfnOldProc, hWnd, msg, wParam, lParam);

	if (msg==WM_NCHITTEST && lRetv==HTBORDER)
		return HTTRANSPARENT;

	return lRetv;
}

void SIMPLEAPI MakeNCTransparent(HWND hWnd)
{
#ifdef _WIN64
	SetProp(hWnd, L"PreTransProc", (HANDLE)GetWindowLongPtr(hWnd, GWLP_WNDPROC));
	SetWindowLongPtr(hWnd, GWLP_WNDPROC, (LONG_PTR)TransEdgeProc);
#else
	SetProp(hWnd, L"PreTransProc", (HANDLE)GetWindowLongPtr(hWnd, GWL_WNDPROC));
	SetWindowLongPtr(hWnd, GWL_WNDPROC, (LONG_PTR)TransEdgeProc);
#endif
}


// Initialisation
void CAutoSplitter::Init(HWND hWndParent) 
{
	CAutoSizeControls::Init(hWndParent, NULL);
}

void CAutoSplitter::Track()
{
	// Work out constraints
	RECT rc;
	GetClientRect(m_hWndParent, &rc);
	InflateRect(&rc, -20, -20);
	SetMouseConstraint(&rc);

	for (int i=0; i<GetSize(); i++)
		{
		RECT rcControl;
		GetWindowRect(GetAt(i)->m_hWndControl, &rcControl);
		
		ScreenToClient(m_hWndParent, (LPPOINT)&rcControl.left);
		ScreenToClient(m_hWndParent, (LPPOINT)&rcControl.right);

		InflateRect(&rcControl, -20, -20);

		DWORD dwFlags=GetAt(i)->m_dwFlags;
		if (dwFlags & ASF_NOHITTEST)
			continue;

		if (dwFlags & ASF_LEFT)
			rc.right=min(rc.right, rcControl.right);
		if (dwFlags & ASF_RIGHT)
			rc.left=max(rc.left, rcControl.left);
		if (dwFlags & ASF_TOP)
			rc.bottom=min(rc.bottom, rcControl.bottom);
		if (dwFlags & ASF_BOTTOM)
			rc.top=max(rc.top, rcControl.top);
		}

	POINT pt;
	GetCursorPos(&pt);
	ScreenToClient(m_hWndParent, &pt);
	CMouseTracker::Track(m_hWndParent, pt);
}

bool CAutoSplitter::HitTest(POINT pt)
{
	ClientToScreen(m_hWndParent, &pt);
	for (int i=0; i<GetSize(); i++)
		{
		RECT rcControl;
		GetWindowRect(GetAt(i)->m_hWndControl, &rcControl);

		DWORD dwFlags=GetAt(i)->m_dwFlags;
		if (dwFlags & ASF_NOHITTEST)
			continue;
		
		if (pt.y>=rcControl.top && pt.y<rcControl.bottom)
			{
			if ((dwFlags & ASF_LEFT) && pt.x>=rcControl.left-5 && pt.x<=rcControl.left+2)
				return true;
			if ((dwFlags & ASF_RIGHT) && pt.x>=rcControl.right-2 && pt.x<=rcControl.right+5)
				return true;
			}

		if (pt.x>=rcControl.left && pt.x<rcControl.right)
			{
			if ((dwFlags & ASF_TOP) && pt.y>=rcControl.top-5 && pt.y<=rcControl.top+2)
				return true;
			if ((dwFlags & ASF_BOTTOM) && pt.y>=rcControl.bottom-2 && pt.y<=rcControl.bottom+5)
				return true;
			}
		}

	return false;
}

void CAutoSplitter::OnMove(POINT ptOld, POINT ptNew)
{
	int dx=ptNew.x - ptOld.x;
	int dy=ptNew.y - ptOld.y;

	MoveControls(dx, dy);
}


void CAutoSplitter::MoveControls(int dx, int dy)
{
	HDWP hDWP=BeginDeferWindowPos(GetSize());
	for (int i=0; i<GetSize(); i++)
		{
		CAutoSizeControl* pControl=GetAt(i);

		RECT rcControl;
		GetWindowRect(pControl->m_hWndControl, &rcControl);

		DWORD dwFlags=pControl->m_dwFlags;

		// Work out new rectangle
		if (dwFlags & ASF_TOP)
			rcControl.top+=dy;
		if (dwFlags & ASF_BOTTOM)
			rcControl.bottom+=dy;
		if (dwFlags & ASF_LEFT)
			rcControl.left+=dx;
		if (dwFlags & ASF_RIGHT)
			rcControl.right+=dx;

		ScreenToClient(m_hWndParent, (LPPOINT)&rcControl.left);
		ScreenToClient(m_hWndParent, (LPPOINT)&rcControl.right);

		hDWP=DeferWindowPos(hDWP, pControl->m_hWndControl, NULL, 
				rcControl.left, rcControl.top,
				rcControl.right-rcControl.left,
				rcControl.bottom-rcControl.top, SWP_NOZORDER|SWP_NOACTIVATE);
		}
	EndDeferWindowPos(hDWP);
}

int CAutoSplitter::GetPosition()
{
	ASSERT(GetSize()>0);

	// Get the first control
	CAutoSizeControl* pControl=GetAt(0);

	// Get its position
	RECT rc;
	GetWindowRect(pControl->m_hWndControl, &rc);
	ScreenToClient(m_hWndParent, (LPPOINT)&rc.left);
	ScreenToClient(m_hWndParent, (LPPOINT)&rc.right);

	if (pControl->m_dwFlags & ASF_LEFT)
		{
		return rc.left;
		}
	if (pControl->m_dwFlags & ASF_RIGHT)
		{
		return rc.right;
		}
	if (pControl->m_dwFlags & ASF_TOP)
		{
		return rc.top;
		}
	if (pControl->m_dwFlags & ASF_BOTTOM)
		{
		return rc.bottom;
		}

	ASSERT(FALSE);
	return -1;
}

void CAutoSplitter::SetPosition(int iPos)
{
	if (iPos<0)
		return;

	ASSERT(GetSize()>0);

	// Get the first control
	CAutoSizeControl* pControl=GetAt(0);

	// Get its current position
	RECT rc;
	GetWindowRect(pControl->m_hWndControl, &rc);
	ScreenToClient(m_hWndParent, (LPPOINT)&rc.left);
	ScreenToClient(m_hWndParent, (LPPOINT)&rc.right);

	int dx=0;
	int dy=0;
	if (pControl->m_dwFlags & ASF_LEFT)
		{
		dx=iPos-rc.left;
		}
	if (pControl->m_dwFlags & ASF_RIGHT)
		{
		dx=iPos-rc.right;
		}
	if (pControl->m_dwFlags & ASF_TOP)
		{
		dy=iPos-rc.top;
		}
	if (pControl->m_dwFlags & ASF_BOTTOM)
		{
		dy=iPos-rc.bottom;
		}

	MoveControls(dx, dy);
}


}	// namespace Simple
