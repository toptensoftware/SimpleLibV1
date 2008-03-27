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
// MouseTracker.h - declaration of CMouseTracker class

#ifndef __MOUSETRACKER_H
#define __MOUSETRACKER_H

namespace Simple
{

#define AUTOSCROLL_FREQUENCY		50
#define AUTOSCROLL_TIMER_PERIOD		(AUTOSCROLL_FREQUENCY/4)
#define AUTOSCROLL_TIMER_ID			98765			// Unlikely tracked window will be using this!

#define HOVER_TIMEOUT				700
#define HOVER_TIMER_PERIOD			(HOVER_TIMEOUT/4)
#define HOVER_TIMER_ID				98766			// Unlikely tracked window will be using this!

void NormalizeRect(RECT* prc);

// CMouseTracker Class
class CMouseTracker
{
public:
// Construction
			CMouseTracker();
	virtual ~CMouseTracker();

// Attributes

// Operations
	void SetMouseConstraint(RECT* prc);
	void SetGridSnap(POINT* ppt);
	void SetScrollOffset(POINT* ppt, bool bAutoScroll);
	bool Track(HWND hWnd, POINT& pt);
	POINT StartPoint() { return m_ptStart; }
	POINT GetScrollOffset();

// Implementation
protected:
// Attributes
	HWND		m_hWnd;
	POINT		m_ptStart;
	RECT*		m_prcMouseBounds;
	POINT*		m_pptSnap;
	POINT*		m_pptScrollOffset;
	bool		m_bAutoScroll;
	bool		m_bLockWindowUpdate;
	DWORD		m_dwLastScrollAt;

	bool		m_bHoverDetect;
	DWORD		m_dwHoverStart;

// Operations
	void ConstrainMouse(POINT& pt);
	virtual void SnapMouse(POINT& pt);

	HDC GetDC();
	void ReleaseDC(HDC hDC);

// Overrides
	virtual void OnStarting(POINT ptStart);
	virtual void OnMove(POINT ptOld, POINT ptNew);
	virtual void OnFinished(POINT pt, bool bCancelled);
	virtual void DrawFeedback() {};
	virtual void OnHover(POINT pt) {};
};

}	// namespace Simple

#endif	// __MOUSETRACKER_H

