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
// Splitter.h - declaration of Splitter

#ifndef __SPLITTER_H
#define __SPLITTER_H

#include "MouseTracker.h"
#include "AutoSizeControl.h"

namespace Simple
{

// Helper to make border of a control transparent for mouse clicks
void SIMPLEAPI MakeNCTransparent(HWND hWnd);

// Splitter class
class CAutoSplitter : 
	public CAutoSizeControls,
	public CMouseTracker
{
public:
// Construction
	CAutoSplitter() 
	{
		m_bLockWindowUpdate=false;
	};

// Initialisation
	void Init(HWND hWndParent);
	void Track();
	bool HitTest(POINT pt);
	virtual void OnMove(POINT ptOld, POINT ptNew);

	void MoveControls(int dx, int dy);

	int GetPosition();
	void SetPosition(int iPos);
};



}	// namespace Simple

#endif	// __SPLITTER_H

