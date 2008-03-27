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
// MouseTracker.cpp - implementation of CMouseTracker class

#include "stdafx.h"
#include "SimpleLibWinBuild.h"

#include "MouseTracker.h"
#include "GdiMisc.h"

namespace Simple
{

void NormalizeRect(RECT* prc)
{
	if (prc->left>prc->right)
		{
		int i=prc->left;
		prc->left=prc->right;
		prc->right=i;
		}

	if (prc->top>prc->bottom)
		{
		int i=prc->top;
		prc->top=prc->bottom;
		prc->bottom=i;
		}
}

//////////////////////////////////////////////////////////////////////////
// CMouseTracker

// Constructor
CMouseTracker::CMouseTracker()
{
	m_prcMouseBounds=NULL;
	m_pptSnap=NULL;
	m_pptScrollOffset=NULL;
	m_bAutoScroll=false;
	m_bHoverDetect=false;
	m_dwHoverStart=GetTickCount();
	m_bLockWindowUpdate=true;
}

// Destructor
CMouseTracker::~CMouseTracker()
{
}

void CMouseTracker::SetMouseConstraint(RECT* prc)
{
	m_prcMouseBounds=prc;
}

void CMouseTracker::SetGridSnap(POINT* ppt)
{
	m_pptSnap=ppt;
}

void CMouseTracker::SetScrollOffset(POINT* ppt, bool bAutoScroll)
{
	m_pptScrollOffset=ppt;
	m_bAutoScroll=bAutoScroll;
}

POINT CMouseTracker::GetScrollOffset()
{
	POINT pt={0,0};
	return m_pptScrollOffset ? *m_pptScrollOffset : pt;
}


void CMouseTracker::ConstrainMouse(POINT& pt)
{
	// Constrain bounds
	if (m_prcMouseBounds)
		{
		if (pt.x<m_prcMouseBounds->left)
			pt.x=m_prcMouseBounds->left;
		if (pt.x>m_prcMouseBounds->right)
			pt.x=m_prcMouseBounds->right;
		if (pt.y<m_prcMouseBounds->top)
			pt.y=m_prcMouseBounds->top;
		if (pt.y>m_prcMouseBounds->bottom)
			pt.y=m_prcMouseBounds->bottom;
		}
}

int SnapInt(int i, int iSnap)
{
	if (i==0)
		return 0;

	if (iSnap==0)
		return i;

	if (i>0)
		return ((i + iSnap/2) / iSnap) * iSnap;
	else
		return -((-i + iSnap/2) / iSnap) * iSnap;
}

void CMouseTracker::SnapMouse(POINT& pt)
{
	if (m_pptSnap)
		{
		pt.x= ( (pt.x + m_pptSnap->x/2) / m_pptSnap->x ) * m_pptSnap->x;
		pt.y= ( (pt.y + m_pptSnap->y/2) / m_pptSnap->y ) * m_pptSnap->y;
		}
}


HDC CMouseTracker::GetDC()
{
	HDC hDC=GetDCEx(m_hWnd, NULL, DCX_PARENTCLIP|DCX_LOCKWINDOWUPDATE);
	POINT pt=GetScrollOffset();
	SetWindowOrgEx(hDC, pt.x, pt.y, NULL);
	return hDC;
}

void CMouseTracker::ReleaseDC(HDC hDC)
{
	::ReleaseDC(m_hWnd, hDC);
}


#define WM_SYSKEYFIRST  WM_SYSKEYDOWN
#define WM_SYSKEYLAST   WM_SYSDEADCHAR
#define WM_NCMOUSEFIRST WM_NCMOUSEMOVE
#define WM_NCMOUSELAST  WM_NCMBUTTONDBLCLK

static bool IsInputMessage(UINT msg)
{
	if ((msg>=WM_MOUSEFIRST) && (msg<=WM_MOUSELAST)) return TRUE;
	if ((msg>=WM_NCMOUSEFIRST) && (msg<=WM_NCMOUSELAST)) return TRUE;
	if ((msg>=WM_KEYFIRST) && (msg<=WM_KEYLAST)) return TRUE;
	if ((msg>=WM_SYSKEYFIRST) && (msg<=WM_SYSKEYLAST)) return TRUE;

	return FALSE;
}


// Track mouse
bool CMouseTracker::Track(HWND hWnd, POINT& pt)
{
	// Capture mouse
	SetCapture(hWnd);

	// Constrain to bounds
	SnapMouse(pt);
	ConstrainMouse(pt);

	// Store attributes
	m_hWnd=hWnd;
	m_ptStart=pt;

	if (m_bLockWindowUpdate)
		{
		// Update the window
		RedrawWindow(hWnd, NULL, NULL, RDW_UPDATENOW|RDW_ALLCHILDREN);
		LockWindowUpdate(m_hWnd);
		}

	// Call start handler
	OnStarting(m_ptStart);

	if (m_bAutoScroll)
		{
		m_dwLastScrollAt=GetTickCount();
		SetTimer(m_hWnd, AUTOSCROLL_TIMER_ID, AUTOSCROLL_TIMER_PERIOD, NULL);
		}

	if (m_bHoverDetect)
		{
		m_dwHoverStart=GetTickCount();
		SetTimer(m_hWnd, HOVER_TIMER_ID, HOVER_TIMER_PERIOD, NULL);
		}


	// Spin message loop
	bool bCancelled=true;
	bool bMoved=false;
	while (true)
		{
		// Get a message
		MSG msg;
		if (!GetMessage(&msg, NULL, 0, 0))
			break;

		// Dispatch all non-input messages
		if (!IsInputMessage(msg.message) && 
				!(msg.message==WM_TIMER && (msg.wParam==AUTOSCROLL_TIMER_ID || msg.wParam==HOVER_TIMER_ID)))
			{
			DispatchMessage(&msg);
			continue;
			}

		// Quit if lost capture
		if (GetCapture()!=m_hWnd)
			break;

		// Handle message
		switch (msg.message)
			{
			case WM_TIMER:
				if (m_bHoverDetect && m_dwHoverStart && msg.wParam==HOVER_TIMER_ID)
					{
					if (GetTickCount() > m_dwHoverStart+HOVER_TIMEOUT)
						{
						POINT pt;
						GetCursorPos(&pt);
						ScreenToClient(m_hWnd, &pt);
						m_dwHoverStart=0;
						OnHover(pt);
						}

					break;
					}


				if (msg.wParam!=AUTOSCROLL_TIMER_ID)
					break;
				{
				POINT pt;
				GetCursorPos(&pt);
				ScreenToClient(m_hWnd, &pt);
				msg.lParam=MAKELPARAM(pt.x, pt.y);
				// Fall throught to fake a WM_MOUSEMOVE
				}

			case WM_MOUSEMOVE:
				{
				// Get new point
				POINT ptNew;
				ptNew.x=(int)(short)LOWORD(msg.lParam);
				ptNew.y=(int)(short)HIWORD(msg.lParam);

				if (ptNew.x!=m_ptStart.x || ptNew.y!=m_ptStart.y)
					bMoved=true;

				// Get client rectangle
				if (m_bAutoScroll)
					{
					// Outside client area?
					RECT rcClient;
					GetClientRect(m_hWnd, &rcClient);
					if (!PtInRect(&rcClient, ptNew) && GetTickCount()>m_dwLastScrollAt + AUTOSCROLL_FREQUENCY)
						{
						// Kill old feedback 
						DrawFeedback();

						// Release DC
						if (m_bLockWindowUpdate)	
							{
							LockWindowUpdate(NULL);
							}

						// Do scrolling
						if (ptNew.x<rcClient.left)
							SendMessage(m_hWnd, WM_HSCROLL, SB_LINEUP, 0);
						if (ptNew.x>rcClient.right)
							SendMessage(m_hWnd, WM_HSCROLL, SB_LINEDOWN, 0);
						if (ptNew.y<rcClient.top)
							SendMessage(m_hWnd, WM_VSCROLL, SB_LINEUP, 0);
						if (ptNew.y>rcClient.bottom)
							SendMessage(m_hWnd, WM_VSCROLL, SB_LINEDOWN, 0);
						UpdateWindow(m_hWnd);

						// Get a newly clipped DC
						if (m_bLockWindowUpdate)
							{
							RedrawWindow(hWnd, NULL, NULL, RDW_UPDATENOW|RDW_ALLCHILDREN);
							LockWindowUpdate(m_hWnd);
							}

						// Redraw feedback
						DrawFeedback();

						m_dwLastScrollAt=GetTickCount();
						}
					}

				// Adjust for scrolling
				POINT ptScrollOffset=GetScrollOffset();
				ptNew.x+=ptScrollOffset.x;
				ptNew.y+=ptScrollOffset.y;

				// Constrain to bounds
				SnapMouse(ptNew);
				ConstrainMouse(ptNew);

				// Only handle if still moved after constraining
				if (ptNew.x!=pt.x || ptNew.y!=pt.y || msg.message==WM_TIMER)
					{
					if (msg.message!=WM_TIMER)	
						{
						m_dwHoverStart=GetTickCount();
						}
			
					// Call handler
					OnMove(pt, ptNew);

					// Store new point
					pt=ptNew;
					}
				}
				break;

			case WM_LBUTTONUP:
			case WM_RBUTTONUP:
				bCancelled=false;
				ReleaseCapture();
				break;
			
			case WM_RBUTTONDOWN:
				ReleaseCapture();
				break;

			case WM_KEYDOWN:
				if (msg.wParam==VK_ESCAPE)
					{
					ReleaseCapture();
					break;
					}
				// Fall through

			case WM_KEYUP:
			case WM_SYSKEYDOWN:
			case WM_SYSKEYUP:
				// Call handler
				OnMove(pt, pt);
				break;
			}

		// Quit if lost capture
		if (GetCapture()!=m_hWnd)
			break;
		}

	if (m_bAutoScroll)
		{
		KillTimer(m_hWnd, AUTOSCROLL_TIMER_ID);
		}

	if (m_bHoverDetect)
		{
		KillTimer(m_hWnd, HOVER_TIMER_ID);
		}

	// Finished
	OnFinished(pt, bCancelled);

	// Clean up
	if (m_bLockWindowUpdate)
		{
		LockWindowUpdate(NULL);
		}

	return bMoved && !bCancelled;


}

void CMouseTracker::OnStarting(POINT ptStart)
{

}

// Move handler
void CMouseTracker::OnMove(POINT ptOld, POINT ptNew)
{
}

void CMouseTracker::OnFinished(POINT pt, bool bCancelled)
{
}




}	// namespace Simple
