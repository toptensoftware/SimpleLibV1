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
// AutoSizeControl.cpp - implementation of AutoSizeControl

#include "stdafx.h"
#include "SimpleLibWinBuild.h"

#include "AutoSizeControl.h"
#include "GeoOps.h"

namespace Simple
{

// Constructor
CAutoSizeControl::CAutoSizeControl()
{
	m_hWndControl=NULL;
	SetRect(&m_rcRatioGaps, 5, 5, 5, 5);
}

// Destructor
CAutoSizeControl::~CAutoSizeControl()
{
}

void CAutoSizeControl::Init(HWND hWndControl, DWORD dwFlags, RECT* prcParent)
{
	// Store params
	m_hWndControl=hWndControl;
	m_dwFlags=dwFlags;

	// Must be a child window
	ASSERT(GetWindowLong(hWndControl, GWL_STYLE) & WS_CHILD);
		
	InitEdges(prcParent);
}

void CAutoSizeControl::InitEdges(RECT* prcParent)
{
	// Get parent control
	HWND hWndParent=GetParent(m_hWndControl);

	// Get window rectangle in client coords
	RECT rcControl;
	GetWindowRect(m_hWndControl, &rcControl);
	ScreenToClient(hWndParent, (LPPOINT)&rcControl.left);
	ScreenToClient(hWndParent, (LPPOINT)&rcControl.right);

	// Store original location
	m_rcOriginal=rcControl;

	// Get parent coords
	RECT rcParent;
	if (prcParent)
		{
		rcParent=*prcParent;
		}
	else
		{
		GetClientRect(hWndParent, &rcParent);
		}

	m_rcEdges.left=rcParent.right-rcControl.left;
	m_rcEdges.right=rcParent.right-rcControl.right;
	m_rcEdges.top=rcParent.bottom-rcControl.top;
	m_rcEdges.bottom=rcParent.bottom-rcControl.bottom;
}

HDWP CAutoSizeControl::Resize(RECT* prcOriginal, RECT* prcParent, HDWP hDWP)
{
	if (!m_hWndControl)
		return hDWP;

	ASSERT(IsWindow(m_hWndControl));

	// Get parent control
	HWND hWndParent=GetParent(m_hWndControl);

	// Get window rectangle in client coords
	RECT rcControl;
	GetWindowRect(m_hWndControl, &rcControl);
	ScreenToClient(hWndParent, (LPPOINT)&rcControl.left);
	ScreenToClient(hWndParent, (LPPOINT)&rcControl.right);

	// Work out new rectangle
	if (m_dwFlags & ASF_TOP)
		rcControl.top=prcParent->bottom-m_rcEdges.top;
	if (m_dwFlags & ASF_BOTTOM)
		rcControl.bottom=prcParent->bottom-m_rcEdges.bottom;
	if (m_dwFlags & ASF_LEFT)
		rcControl.left=prcParent->right-m_rcEdges.left;
	if (m_dwFlags & ASF_RIGHT)
		rcControl.right=prcParent->right-m_rcEdges.right;

	#define ADJUST	4

	if (m_dwFlags & ASF_TOP_RATIO)
		rcControl.top=Height(*prcParent) * (m_rcOriginal.top-m_rcRatioGaps.left) / Height(*prcOriginal) + m_rcRatioGaps.left;
	if (m_dwFlags & ASF_BOTTOM_RATIO)
		rcControl.bottom=Height(*prcParent) * (m_rcOriginal.bottom+m_rcRatioGaps.bottom) / Height(*prcOriginal) -m_rcRatioGaps.bottom;
	if (m_dwFlags & ASF_LEFT_RATIO)
		rcControl.left=Width(*prcParent) * (m_rcOriginal.left-m_rcRatioGaps.left) / Width(*prcOriginal) + m_rcRatioGaps.left;
	if (m_dwFlags & ASF_RIGHT_RATIO)
		rcControl.right=Width(*prcParent) * (m_rcOriginal.right+m_rcRatioGaps.right) / Width(*prcOriginal) - m_rcRatioGaps.right;

	if (hDWP)
		{
		return DeferWindowPos(hDWP, m_hWndControl, NULL, 
					rcControl.left, rcControl.top,
					rcControl.right-rcControl.left,
					rcControl.bottom-rcControl.top, SWP_NOZORDER|SWP_NOACTIVATE);
		}
	else
		{
		SetWindowPos(m_hWndControl, NULL, 
					rcControl.left, rcControl.top,
					rcControl.right-rcControl.left,
					rcControl.bottom-rcControl.top, SWP_NOZORDER|SWP_NOACTIVATE);
		return NULL;
		}

}




/////////////////////////////////////////////////////////////////////////////
// CAutoSizeControls

// Constructor
CAutoSizeControls::CAutoSizeControls()
{
	m_hWndParent=NULL;
	m_sizeMinimum=SSize(0,0);
}

// Destructor
CAutoSizeControls::~CAutoSizeControls()
{
}


void CAutoSizeControls::Init(HWND hWndParent, RECT* prcParent)
{
	// Save parent window handle
	m_hWndParent=hWndParent;

	// Save initial rectangle
	if (prcParent)
		{
		m_rcInitial=*prcParent;
		}
	else
		{
		GetClientRect(hWndParent, &m_rcInitial);
		}
}

void CAutoSizeControls::SetMinimumSize(SIZE size)
{
	m_sizeMinimum=size;
}

// Set minimum size to original size
void CAutoSizeControls::SetMinimumSize()
{
	m_sizeMinimum=Size(m_rcInitial);
}


void CAutoSizeControls::AddControl(HWND hWnd, DWORD dwFlags)
{
	ASSERT(IsWindow(m_hWndParent));

	RECT rc;
	GetClientRect(m_hWndParent, &rc);

	// Create a new auto size control
	CAutoSizeControl* pControl=new CAutoSizeControl();
	pControl->Init(hWnd, dwFlags, &rc);

	// Add it
	Add(pControl);
}

void CAutoSizeControls::AddControl(int iID, DWORD dwFlags)
{
	ASSERT(IsWindow(m_hWndParent));

	// Get the control
	HWND hWndControl=::GetDlgItem(m_hWndParent, iID);
	ASSERT(hWndControl!=NULL);

	AddControl(hWndControl, dwFlags);
}

void CAutoSizeControls::Resize()
{
	RECT rc;
	GetClientRect(m_hWndParent, &rc);
	if (m_sizeMinimum.cx>0 && Width(rc)<m_sizeMinimum.cx)
	{
		rc.right=rc.left + m_sizeMinimum.cx;
	}
	if (m_sizeMinimum.cy>0 && Height(rc)<m_sizeMinimum.cy)
	{
		rc.bottom=rc.top + m_sizeMinimum.cy;
	}


	HDWP hDWP=BeginDeferWindowPos(GetSize());
	for (int i=0; i<GetSize(); i++)	
		{
		hDWP=GetAt(i)->Resize(&m_rcInitial, &rc, hDWP);
		}
	EndDeferWindowPos(hDWP);
}


// Get the size of this window, adjusted for minimum size constraint
SIZE CAutoSizeControls::GetNaturalSize()
{
	RECT rc;
	GetClientRect(m_hWndParent, &rc);
	SIZE size=Size(rc);

	if (m_sizeMinimum.cx>0 && size.cx<m_sizeMinimum.cx)
	{
		size.cx=m_sizeMinimum.cx;
	}
	if (m_sizeMinimum.cy>0 && size.cy<m_sizeMinimum.cy)
	{
		size.cy=m_sizeMinimum.cy;
	}

	return size;
}

void CAutoSizeControls::OnControlsMoved(RECT* prcParent)
{
	for (int i=0; i<GetSize(); i++)
		{
		GetAt(i)->InitEdges(prcParent);
		}
}

void CAutoSizeControls::SetRatioGaps(UINT nID, int iLeft, int iTop, int iRight, int iBottom)
{
	CAutoSizeControl* pControl=FindControl(nID);
	SetRect(&pControl->m_rcRatioGaps, iLeft, iTop, iRight, iBottom);
}

void CAutoSizeControls::MaintainHorizontalGap(UINT idLeft, UINT idRight)
{
	CAutoSizeControl* pLeft=FindControl(idLeft);
	CAutoSizeControl* pRight=FindControl(idRight);

	int iGap=pRight->m_rcOriginal.left - pLeft->m_rcOriginal.right;

	int iLeftGap=iGap/2;
	int iRightGap=iGap-iLeftGap;

	pLeft->m_rcRatioGaps.right=iLeftGap;
	pRight->m_rcRatioGaps.left=iRightGap;
}

CAutoSizeControl* CAutoSizeControls::FindControl(UINT nID)
{
	for (int i=0; i<GetSize(); i++)
		{
		if (GetDlgCtrlID(GetAt(i)->m_hWndControl)==(int)nID)
			{
			return GetAt(i);
			}
		}

	ASSERT(FALSE);
	return NULL;
}



}	// namespace Simple
