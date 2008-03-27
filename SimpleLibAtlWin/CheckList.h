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
// FrameWindow.h - declaration of CBarFrameWindow class

#ifndef __CHECKLIST_H
#define __CHECKLIST_H


#ifdef __ATLWIN_H__
#ifdef __ATLCONTROLS_H__

namespace Simple
{

class CItemInfo;

#define CBLN_STATECHANGED	100

// CCheckList Class
class CCheckList : public CWindowImpl<CCheckList, ATLControls::CListBox>
{
	typedef CWindowImpl<CCheckList, ATLControls::CListBox> base;

public:
// Construction
			CCheckList();
	virtual ~CCheckList();

// Operations
	LPARAM GetItemData(int nIndex) const;
	int SetItemData(int nIndex, LPARAM dwItemData);
	BOOL GetItemChecked(int nIndex) const;
	int SetItemChecked(int nIndex, BOOL bChecked, BOOL bFireNotification=FALSE);
	void SetImageList(HIMAGELIST hImageList);
	int SetItemImage(int nIndex, int iImage);
	void SelectSingleItem(int iItem);
	BOOL SetItemText(int iItem, LPCOLESTR pszText);
	CUniString GetItemText(int iItem);

protected:
// Implementation
BEGIN_MSG_MAP(CCheckList)
	MESSAGE_HANDLER(OCM_DRAWITEM, OnDrawItem)
	MESSAGE_HANDLER(OCM_MEASUREITEM, OnDrawItem)
	MESSAGE_HANDLER(OCM_DELETEITEM, OnDeleteItem)
	MESSAGE_HANDLER(WM_KEYDOWN, OnKeyDown)
	MESSAGE_HANDLER(WM_LBUTTONDOWN, OnLButtonDown)
	MESSAGE_HANDLER(WM_LBUTTONUP, OnLButtonUp)
	MESSAGE_HANDLER(WM_LBUTTONDBLCLK, OnLButtonDblClk)
	MESSAGE_HANDLER(WM_ENABLE, OnEnable)
	MESSAGE_HANDLER(WM_SETFONT, OnSetFont)
	MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
	DEFAULT_REFLECTION_HANDLER()
	MESSAGE_HANDLER(WM_CHAR, OnChar)
	MESSAGE_HANDLER(LB_SETTABSTOPS, OnSetTabStops)
END_MSG_MAP()
	LRESULT OnChar(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	// Handler Insert Pos(CCheckList)


	LRESULT OnDrawItem(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnMeasureItem(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnKeyDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnDeleteItem(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnLButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnLButtonDblClk(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnEnable(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnSetFont(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnSetTabStops(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

// Attributes
	HBITMAP		m_hCheckBits;
	int			m_iMarginWidth;
	int			m_iImageWidth;
	int			m_cxImage;
	int			m_cyImage;
	HIMAGELIST	m_hImageList;
	CVector<int>	m_TabStops;

// Operations
	CItemInfo* GetItemInfo(int nIndex) const;
	void GetSelectedItems(CVector<int>& Items) const;
	void UpdateItemHeight();

};

}	// namepace Simple

#endif	//	__ATLCONTROLS_H__
#endif	//	__ATLWIN_H__

#endif	// __CHECKLIST_H

