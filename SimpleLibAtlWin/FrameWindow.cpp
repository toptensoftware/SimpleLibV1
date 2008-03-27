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
// FrameWindow.cpp - implementation of CBarFrameWindowImpl class

#include "stdafx.h"
#include "SimpleLibAtlWinBuild.h"

#include "FrameWindow.h"

namespace Simple
{

//////////////////////////////////////////////////////////////////////////
// CBarFrameWindowImpl


// Constructor
CBarFrameWindowImpl::CBarFrameWindowImpl(HWND& hWndFrame) 
	: m_hWndFrame(hWndFrame)
{					
	m_bFullScreen=false;
	m_rcMargins=SRect(0,0,0,0);
}

// Destructor
CBarFrameWindowImpl::~CBarFrameWindowImpl()
{				  
}
									   
// Add a bar
void CBarFrameWindowImpl::AddBar(HWND hWndBar, int iSize, DWORD dwPlacement, bool bUpdate, HWND hWndAfter)
{
	// Check not already added
	ASSERT(FindBarEntry(hWndBar)<0);
	
	// Find insert position
	int iPos=FindBarEntry(hWndAfter);
	if (iPos<0)
		iPos=m_Bars.GetSize();

	// Get size from bar
	if (iSize<0)
	{
		RECT rc;
		GetWindowRect(hWndBar, &rc);
		if (dwPlacement & (BARFLAG_TOP|BARFLAG_BOTTOM))
			iSize=Height(rc);
		else if (dwPlacement & (BARFLAG_LEFT|BARFLAG_RIGHT))
			iSize=Width(rc);
	}

	// Insert into collection
	m_Bars.InsertAt(iPos, new CBarEntry(hWndBar, iSize, dwPlacement));
	
	if (m_hWndFrame && bUpdate)
		RecalcLayout();
}

// Remove an existing bar
void CBarFrameWindowImpl::RemoveBar(HWND hWndBar, bool bUpdate)
{
	int iPos=FindBarEntry(hWndBar);
	if (iPos>=0)
		m_Bars.RemoveAt(iPos);

	if (m_hWndFrame && bUpdate)
		RecalcLayout();
}

void CBarFrameWindowImpl::ResizeBar(HWND hWndBar, int iNewSize, bool bUpdate)
{
	int iPos=FindBarEntry(hWndBar);
	if (iPos>=0)
		m_Bars[iPos]->m_iSize=iNewSize;

	if (m_hWndFrame && bUpdate)
		RecalcLayout();
}

void CBarFrameWindowImpl::EnableBarPersistence(HWND hWndBar, const wchar_t* pszName, DWORD dwFlags)
{
	int iPos=FindBarEntry(hWndBar);
	if (iPos<0)
		return;

	m_Bars[iPos]->m_strPersistName=pszName;
	m_Bars[iPos]->m_dwFlags|=dwFlags;
}

void CBarFrameWindowImpl::SaveBarState(const wchar_t* pszSection)
{					  
	if (!pszSection)
		pszSection=m_bFullScreen ? L"FullScreenBarState" : L"BarState";

	for (int i=0; i<m_Bars.GetSize(); i++)
	{
		CBarEntry* pBar=m_Bars[i];

		// Save size
		if (pBar->m_dwFlags & BARFLAG_SAVESIZE)
		{
			SlxSetProfileInt(pszSection, Format(L"%sSize", pBar->m_strPersistName), pBar->m_iSize);
		}

		// Save visibility
		if (pBar->m_dwFlags & BARFLAG_SAVEVISIBLE)
		{
			SlxSetProfileInt(pszSection, Format(L"%sVisible", pBar->m_strPersistName), (GetWindowLong(pBar->m_hWnd, GWL_STYLE) & WS_VISIBLE)!=0);
		}
	}
}

void CBarFrameWindowImpl::LoadBarState(const wchar_t* pszSection, bool bRecalcLayout)
{
	if (!pszSection)
		pszSection=m_bFullScreen ? L"FullScreenBarState" : L"BarState";

	for (int i=0; i<m_Bars.GetSize(); i++)
	{
		CBarEntry* pBar=m_Bars[i];

		// Load size
		if (pBar->m_dwFlags & BARFLAG_SAVESIZE)
		{
			int iSize=SlxGetProfileInt(pszSection, Format(L"%sSize", pBar->m_strPersistName), -10000);
			if (iSize!=-10000)
				pBar->m_iSize=iSize;
		}

		// Load visibility
		if (pBar->m_dwFlags & BARFLAG_SAVEVISIBLE)
		{
			int iVisible=SlxGetProfileInt(pszSection, Format(L"%sVisible", pBar->m_strPersistName), -1);
			if (iVisible>=0)
			{
				ShowWindow(pBar->m_hWnd, iVisible ? SW_SHOW : SW_HIDE);
			}
		}
	}

	if (bRecalcLayout)
		RecalcLayout();
}


void CBarFrameWindowImpl::SavePlacement(const wchar_t* pszSection, const wchar_t* pszPrefix)
{
	Simple::SaveWindowPlacement(m_hWndFrame, pszSection, pszPrefix, m_bFullScreen ? &m_NonFullScreenWindowPlacement : NULL);
}

int CBarFrameWindowImpl::LoadPlacement(const wchar_t* pszSection, const wchar_t* pszPrefix)
{
	return Simple::LoadWindowPlacement(m_hWndFrame, pszSection, pszPrefix);
}


void CBarFrameWindowImpl::RecalcLayout()
{
	if (!m_hWndFrame)
		return;

	// Get available area
	RECT rcAvailable;
	GetClientRect(m_hWndFrame, &rcAvailable);

	rcAvailable.left+=m_rcMargins.left;
	rcAvailable.top+=m_rcMargins.top;
	rcAvailable.bottom-=m_rcMargins.bottom;
	rcAvailable.right-=m_rcMargins.right;

	// Dont resize if zero size (no point)
	if (IsRectEmpty(&rcAvailable))
		return;

	// Position all bars
	HDWP hDWP=BeginDeferWindowPos(5);
	for (int i=0; i<m_Bars.GetSize(); i++)
	{
		// Get the bar
		CBarEntry* pBar=m_Bars[i];

		// Ignore invalid/invisible windows
		if (!IsWindow(pBar->m_hWnd) || (GetWindowLong(pBar->m_hWnd, GWL_STYLE) & WS_VISIBLE)==0)
			continue;

		// Work out position
		RECT rcBar=rcAvailable;
		if (pBar->m_dwFlags & BARFLAG_TOP)
		{
			rcBar.bottom=rcBar.top + pBar->m_iSize;
		}
		else if (pBar->m_dwFlags & BARFLAG_BOTTOM)
		{
			rcBar.top=rcBar.bottom - pBar->m_iSize;
		}
		else if (pBar->m_dwFlags & BARFLAG_LEFT)
		{
			rcBar.right=rcBar.left + pBar->m_iSize;
		}
		else if (pBar->m_dwFlags & BARFLAG_RIGHT)
		{
			rcBar.left=rcBar.right - pBar->m_iSize;
		}
		
		// Move it
		hDWP=DeferWindowPos(hDWP, pBar->m_hWnd, NULL, rcBar.left, rcBar.top, Width(rcBar), Height(rcBar), SWP_NOZORDER|SWP_NOACTIVATE);

		// Update available area
		SubtractRect(&rcAvailable, &rcAvailable, &rcBar);
	}
	EndDeferWindowPos(hDWP);
}

void CBarFrameWindowImpl::SetLayoutMargins(RECT rc)
{
	m_rcMargins=rc;
}


LRESULT CBarFrameWindowImpl::OnMessage(UINT nMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	switch (nMsg)
	{
		case WM_BAR_GETPLACEMENT:
		{
			bHandled=TRUE;
			int iBar=FindBarEntry((HWND)lParam);
			if (iBar<0)
				return 0;
			return m_Bars[iBar]->m_dwFlags & 0x0000FFFF;
		}

		case WM_BAR_SETPLACEMENT:
		{
			bHandled=TRUE;
			int iBar=FindBarEntry((HWND)lParam);
			if (iBar<0)
				return 0;

			if ((m_Bars[iBar]->m_dwFlags & 0x0000FFFF)!=wParam)
			{
				m_Bars[iBar]->m_dwFlags = (m_Bars[iBar]->m_dwFlags & 0xFFFF0000) | (DWORD)wParam;
			}
			return TRUE;
		}

		case WM_BAR_RESIZEBAR:
		{
			bHandled=TRUE;
			ResizeBar((HWND)lParam, int(wParam), true);
			break;
		}

		case WM_BAR_RECALCLAYOUT:
		{
			bHandled=TRUE;
			RecalcLayout();
			break;
		}
	}

	return 0;
}


int CBarFrameWindowImpl::FindBarEntry(HWND hWnd)
{
	for (int i=0; i<m_Bars.GetSize(); i++)
	{
		if (m_Bars[i]->m_hWnd==hWnd)
			return i;
	}

	return -1;
}


bool CBarFrameWindowImpl::GetFullScreen()
{
	return m_bFullScreen;
}

void CBarFrameWindowImpl::SetFullScreen(bool bFullScreen, const wchar_t* pszNormalState, const wchar_t* pszFullScreenState)
{
	if (m_bFullScreen==bFullScreen)
		return;

	if (!pszNormalState)
		pszNormalState=L"BarState";
	if (!pszFullScreenState)
		pszFullScreenState=L"FullScreenBarState";

	m_bFullScreen=bFullScreen;

	// Get current style
	DWORD dwStyle=GetWindowLong(m_hWndFrame, GWL_STYLE);

	if (m_bFullScreen)
	{
		// Save window placement
		m_NonFullScreenWindowPlacement.length=sizeof(m_NonFullScreenWindowPlacement);
		GetWindowPlacement(m_hWndFrame, &m_NonFullScreenWindowPlacement);

		// Save bar state
		SaveBarState(pszNormalState);

		// Save which bars are currently visible
		for (int i=0; i<m_Bars.GetSize(); i++)
		{
			if (m_Bars[i]->IsBar())
			{
				DWORD dwStyle=GetWindowLong(m_Bars[i]->m_hWnd, GWL_STYLE);
				m_Bars[i]->m_bNonFullScreenVisible=(dwStyle & WS_VISIBLE)!=0;
				ShowWindow(m_Bars[i]->m_hWnd, SW_HIDE);
			}
		}

		// Load full screen state
		LoadBarState(pszFullScreenState, false);

		// Modify style
		dwStyle &= ~(WS_DLGFRAME | WS_SYSMENU | WS_MAXIMIZE);
		SetWindowLong(m_hWndFrame, GWL_STYLE, dwStyle);

		// Setup new placement
		WINDOWPLACEMENT wp=m_NonFullScreenWindowPlacement;
		wp.showCmd=SW_SHOWMAXIMIZED;
		SetWindowPlacement(m_hWndFrame, &wp);

	}
	else
	{
		// Save full screen state
		SaveBarState(pszFullScreenState);

		// Restore which bars are currently visible
		for (int i=0; i<m_Bars.GetSize(); i++)
		{
			if (m_Bars[i]->IsBar())
				ShowWindow(m_Bars[i]->m_hWnd, m_Bars[i]->m_bNonFullScreenVisible ? SW_SHOW : SW_HIDE);
		}

		// Restore normal state...
		LoadBarState(pszNormalState);

		// Restore style
		SetWindowLong(m_hWndFrame, GWL_STYLE, dwStyle | WS_DLGFRAME | WS_SYSMENU);
		SetWindowPos(m_hWndFrame, NULL, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE|SWP_NOZORDER|SWP_DRAWFRAME);

		// Restore placement
		SetWindowPlacement(m_hWndFrame, &m_NonFullScreenWindowPlacement);
	}
}


}	// namespace Simple
