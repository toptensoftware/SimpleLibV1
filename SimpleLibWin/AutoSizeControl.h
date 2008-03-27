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

/*

// DEPRECATED - Use Reference Points instead

#define ASF_TOP_RATIO		0x00000010
#define ASF_BOTTOM_RATIO	0x00000020
#define ASF_LEFT_RATIO		0x00000040
#define ASF_RIGHT_RATIO		0x00000080
*/

class CAutoSizeControl
{
public:
	CAutoSizeControl();
	~CAutoSizeControl();

	void Init(HWND hWndControl, DWORD dwFlags, POINT ptRef);
	void InitEdges(POINT ptRef);
	void Resize(RECT& rcControl, POINT ptOriginal, POINT ptNew);

	HWND	m_hWndControl;
	RECT	m_rcEdges;
	RECT	m_rcOriginal;
	DWORD	m_dwFlags;
	int		m_iReferencePoint;
};

class CAutoSizeReferencePoint
{
public:
	CAutoSizeReferencePoint(float fltXRatio, float fltYRatio) : 
		m_fltXRatio(fltXRatio),
		m_fltYRatio(fltYRatio)
	{
	}

	
	void Resize(SIZE sizeNew)
	{
		m_ptCurrent.x=int(float(sizeNew.cx) * m_fltXRatio);
		m_ptCurrent.y=int(float(sizeNew.cy) * m_fltYRatio);
	}

	float	m_fltXRatio;
	float	m_fltYRatio;
	POINT	m_ptCurrent;
	POINT	m_ptOriginal;
};

class CAutoSizeControls : public CVector<CAutoSizeControl*, SOwnedPtr>
{
public:
	CAutoSizeControls();
	~CAutoSizeControls();

	void Init(HWND hWndParent, RECT* prcParent=NULL);
	void SetMinimumSize(SIZE size);
	void SetMinimumSize();
	void AddReferencePoint(int iID, float fltXRatio, float fltYRatio);
	void AddControl(int iID, DWORD dwFlags, int iReferencePoint=0);
	void AddControl(HWND hWnd, DWORD dwFlags, int iReferencePoint=0);
	void Resize();
	bool IsInitialized() { return m_hWndParent!=NULL; };
	void OnControlsMoved(RECT* prcParent=NULL);
	SIZE GetNaturalSize();

	CAutoSizeControl* FindControl(UINT nID);

	SIZE	m_sizeOriginal;
	HWND	m_hWndParent;
	SIZE	m_sizeMinimum;
	CMap<int, CAutoSizeReferencePoint*, SValue, SOwnedPtr>	m_ReferencePoints;
};



}	// namespace Simple

#endif	// __AUTOSIZECONTROL_H

