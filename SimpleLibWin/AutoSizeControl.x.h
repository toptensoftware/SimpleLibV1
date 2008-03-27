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
// AutoSizeControl.h - declaration of AutoSizeControl

#ifndef __AUTOSIZECONTROL_H
#define __AUTOSIZECONTROL_H

namespace Simple
{

// Which sides of control to autosize
#define ASF_TOP			0x00000001
#define ASF_BOTTOM		0x00000002
#define ASF_LEFT		0x00000004
#define ASF_RIGHT		0x00000008

#define ASF_NOHITTEST	0x80000000	// Dont hit test this control in CAutoSplitter

#define ASF_TOP_RATIO		0x00000010
#define ASF_BOTTOM_RATIO	0x00000020
#define ASF_LEFT_RATIO		0x00000040
#define ASF_RIGHT_RATIO		0x00000080

class CAutoSizeControl
{
public:
	CAutoSizeControl();
	~CAutoSizeControl();

	void Init(HWND hWndControl, DWORD dwFlags, RECT* prcParent=NULL);
	void InitEdges(RECT* prcParent=NULL);
	HDWP Resize(RECT* prcOriginal, RECT* prcParent, HDWP hDWP=NULL);

	HWND	m_hWndControl;
	RECT	m_rcEdges;
	RECT	m_rcOriginal;
	RECT	m_rcRatioGaps;
	DWORD	m_dwFlags;
};

class CAutoSizeControls : public CVector<CAutoSizeControl*, SOwnedPtr>
{
public:
	CAutoSizeControls();
	~CAutoSizeControls();

	void Init(HWND hWndParent, RECT* prcParent=NULL);
	void SetMinimumSize(SIZE size);
	void SetMinimumSize();
	void AddControl(int iID, DWORD dwFlags);
	void AddControl(HWND hWnd, DWORD dwFlags);
	void Resize();
	bool IsInitialized() { return m_hWndParent!=NULL; };
	void OnControlsMoved(RECT* prcParent=NULL);
	void SetRatioGaps(UINT nID, int iLeft, int iTop, int iRight, int iBottom);
	void MaintainHorizontalGap(UINT idLeft, UINT idRight);
	SIZE GetNaturalSize();

	CAutoSizeControl* FindControl(UINT nID);

	RECT	m_rcInitial;
	HWND	m_hWndParent;
	SIZE	m_sizeMinimum;
};



}	// namespace Simple

#endif	// __AUTOSIZECONTROL_H

