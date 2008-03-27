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
	m_iReferencePoint=0;
}

// Destructor
CAutoSizeControl::~CAutoSizeControl()
{
}

void CAutoSizeControl::Init(HWND hWndControl, DWORD dwFlags, POINT ptRef)
{
	// Store params
	m_hWndControl=hWndControl;
	m_dwFlags=dwFlags;

	// Must be a child window
	ASSERT(GetWindowLong(hWndControl, GWL_STYLE) & WS_CHILD);

	InitEdges(ptRef);
}

void CAutoSizeControl::InitEdges(POINT ptRef)
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

	m_rcEdges.left=ptRef.x-rcControl.left;
	m_rcEdges.right=ptRef.x-rcControl.right;
	m_rcEdges.top=ptRef.y-rcControl.top;
	m_rcEdges.bottom=ptRef.y-rcControl.bottom;
}

/*
void CAutoSizeControl::Prepare()
{
	if (!m_hWndControl)
		return;
	ASSERT(IsWindow(m_hWndControl));

	// Get parent control
	HWND hWndParent=GetParent(m_hWndControl);

	// Get window rectangle in client coords
	GetWindowRect(m_hWndControl, &m_rcWorking);
	ScreenToClient(hWndParent, (LPPOINT)&m_rcWorking.left);
	ScreenToClient(hWndParent, (LPPOINT)&m_rcWorking.right);
}

HDWP CAutoSizeControl::Apply(HDWP hDWP)
{
	if (!m_hWndControl)
		return hDWP;
	ASSERT(IsWindow(m_hWndControl));

	if (hDWP)
	{
		return DeferWindowPos(hDWP, m_hWndControl, NULL, 
					m_rcWorking.left, m_rcWorking.top,
					m_rcWorking.right-m_rcWorking.left,
					m_rcWorking.bottom-m_rcWorking.top, SWP_NOZORDER|SWP_NOACTIVATE);
	}
	else
	{
		SetWindowPos(m_hWndControl, NULL, 
					m_rcWorking.left, m_rcWorking.top,
					m_rcWorking.right-m_rcWorking.left,
					m_rcWorking.bottom-m_rcWorking.top, SWP_NOZORDER|SWP_NOACTIVATE);
		return NULL;
	}

}
*/


void CAutoSizeControl::Resize(RECT& rcControl, POINT ptOriginal, POINT ptNew)
{
	if (!m_hWndControl)
		return;

	ASSERT(IsWindow(m_hWndControl));

	// Work out new rectangle
	if (m_dwFlags & ASF_TOP)
		rcControl.top=ptNew.y-m_rcEdges.top;
	if (m_dwFlags & ASF_BOTTOM)
		rcControl.bottom=ptNew.y-m_rcEdges.bottom;
	if (m_dwFlags & ASF_LEFT)
		rcControl.left=ptNew.x-m_rcEdges.left;
	if (m_dwFlags & ASF_RIGHT)
		rcControl.right=ptNew.x-m_rcEdges.right;

	/*
	#define ADJUST	4

	if (m_dwFlags & ASF_TOP_RATIO)
		rcControl.top=ptNew.y * (m_rcOriginal.top-m_rcRatioGaps.left) / ptOriginal.y + m_rcRatioGaps.left;
	if (m_dwFlags & ASF_BOTTOM_RATIO)
		rcControl.bottom=ptNew.y * (m_rcOriginal.bottom+m_rcRatioGaps.bottom) / ptOriginal.y -m_rcRatioGaps.bottom;
	if (m_dwFlags & ASF_LEFT_RATIO)
		rcControl.left=ptNew.x * (m_rcOriginal.left-m_rcRatioGaps.left) / ptOriginal.x + m_rcRatioGaps.left;
	if (m_dwFlags & ASF_RIGHT_RATIO)
		rcControl.right=ptNew.x * (m_rcOriginal.right+m_rcRatioGaps.right) / ptOriginal.x - m_rcRatioGaps.right;
		*/
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

	RemoveAll();
	m_ReferencePoints.RemoveAll();
		
	// Save initial rectangle
	if (prcParent)
	{
		m_sizeOriginal=Size(*prcParent);
	}
	else
	{
		RECT rc;
		GetClientRect(hWndParent, &rc);
		m_sizeOriginal=Size(rc);
	}

	AddReferencePoint(0, 1.0f, 1.0f);
}

void CAutoSizeControls::SetMinimumSize(SIZE size)
{
	m_sizeMinimum=size;
}

// Set minimum size to original size
void CAutoSizeControls::SetMinimumSize()
{
	m_sizeMinimum=m_sizeOriginal;
}


void CAutoSizeControls::AddReferencePoint(int iID, float fltXRatio, float fltYRatio)
{
	// Check not already defined
	ASSERT(!m_ReferencePoints.HasKey(iID));

	// Create the reference point
	CAutoSizeReferencePoint* pPoint=new CAutoSizeReferencePoint(fltXRatio, fltYRatio);

	// Set its initial position
	pPoint->Resize(m_sizeOriginal);
	pPoint->m_ptOriginal=pPoint->m_ptCurrent;

	// Add to collection
	m_ReferencePoints.Add(iID, pPoint);
}

void CAutoSizeControls::AddControl(HWND hWnd, DWORD dwFlags, int iReferencePoint)
{
	ASSERT(IsWindow(m_hWndParent));

	CAutoSizeReferencePoint* pPoint=m_ReferencePoints.Get(iReferencePoint, NULL);
	ASSERT(pPoint!=NULL);

	// Create a new auto size control
	CAutoSizeControl* pControl=new CAutoSizeControl();
	pControl->Init(hWnd, dwFlags, pPoint->m_ptCurrent);
	pControl->m_iReferencePoint=iReferencePoint;

	// Add it
	Add(pControl);
}

void CAutoSizeControls::AddControl(int iID, DWORD dwFlags, int iReferencePoint)
{
	ASSERT(IsWindow(m_hWndParent));

	// Get the control
	HWND hWndControl=::GetDlgItem(m_hWndParent, iID);
	ASSERT(hWndControl!=NULL);

	AddControl(hWndControl, dwFlags, iReferencePoint);
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

	// Resize all reference points
	for (int i=0; i<m_ReferencePoints.GetSize(); i++)
	{
		m_ReferencePoints[i].Value->Resize(Size(rc));
	}

	class CResizeInfo
	{
	public:
		HWND	m_hWndControl;
		RECT	m_rcControl;
	};

	CMap<HWND, CResizeInfo*, SValue, SOwnedPtr>	ResizeInfoMap;

	// Prepare for resize
	for (int i=0; i<GetSize(); i++)
	{
		if (!GetAt(i)->m_hWndControl)
			continue;

		if (!ResizeInfoMap.HasKey(GetAt(i)->m_hWndControl))
		{
			// Create resize info
			CResizeInfo* pInfo=new CResizeInfo();
			pInfo->m_hWndControl=GetAt(i)->m_hWndControl;

			// Get window rectangle in client coords
			GetWindowRect(pInfo->m_hWndControl, &pInfo->m_rcControl);
			ScreenToClient(m_hWndParent, (LPPOINT)&pInfo->m_rcControl.left);
			ScreenToClient(m_hWndParent, (LPPOINT)&pInfo->m_rcControl.right);

			// Add to map
			ResizeInfoMap.Add(pInfo->m_hWndControl, pInfo);
		}
	}

	// Do the resize
	for (int i=0; i<GetSize(); i++)	
	{
		if (!GetAt(i)->m_hWndControl)
			continue;

		// Get reference point
		CAutoSizeReferencePoint* pPoint=m_ReferencePoints.Get(GetAt(i)->m_iReferencePoint, NULL);
		ASSERT(pPoint!=NULL);

		// Get info
		CResizeInfo* pInfo=ResizeInfoMap.Get(GetAt(i)->m_hWndControl, NULL);
		ASSERT(pInfo!=NULL);

		// Resize the control
		GetAt(i)->Resize(pInfo->m_rcControl, pPoint->m_ptOriginal, pPoint->m_ptCurrent);
	}

	// Apply
	HDWP hDWP=BeginDeferWindowPos(ResizeInfoMap.GetSize());
	for (int i=0; i<ResizeInfoMap.GetSize(); i++)
	{
		CResizeInfo* pInfo=ResizeInfoMap[i].Value;

		hDWP=DeferWindowPos(hDWP, pInfo->m_hWndControl, NULL, 
			pInfo->m_rcControl.left, pInfo->m_rcControl.top,
			pInfo->m_rcControl.right-pInfo->m_rcControl.left,
			pInfo->m_rcControl.bottom-pInfo->m_rcControl.top, SWP_NOZORDER|SWP_NOACTIVATE);
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
	// Update reference points
	for (int i=0; i<m_ReferencePoints.GetSize(); i++)
	{
		m_ReferencePoints[i].Value->Resize(Size(*prcParent));
	}

	for (int i=0; i<GetSize(); i++)
	{
		CAutoSizeReferencePoint* pPoint=m_ReferencePoints.Get(GetAt(i)->m_iReferencePoint, NULL);
		GetAt(i)->InitEdges(pPoint->m_ptOriginal);
	}
}

/*
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
*/

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
