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
// ListViewUtils.h - declaration of ListViewUtils

#ifndef __LISTVIEWUTILS_H
#define __LISTVIEWUTILS_H

#ifdef __ATLWIN_H__
#ifdef __ATLCONTROLS_H__

namespace Simple
{

#include "SimpleLibRes.h"

void SelectSingleItem(ATLControls::CListViewCtrl& ListView, int iItem);
void SelectAllItems(ATLControls::CListViewCtrl& ListView);
void ToggleCheckState(ATLControls::CListViewCtrl& ListView);
void LoadColumnWidths(ATLControls::CListViewCtrl& ListView, const wchar_t* pszSection, const wchar_t* pszPrefix);
void SaveColumnWidths(ATLControls::CListViewCtrl& ListView, const wchar_t* pszSection, const wchar_t* pszPrefix);

// CListViewSorter Class
class CListViewSorter
{
public:
// Construction
			CListViewSorter();
	virtual ~CListViewSorter();

// Attributes
	void Init(HWND hWndListView, int iResIdSortIcons=IDB_SIMPLELIB_SORTICONS);

// Operations
	void OnColumnClick(int iColumn);
	void SetSortOrder(int iSubItem);
	void Resort();
	int GetSortOrder();
	void AddAltSortOrder(int iSubItem);

// Implementation
protected:
// Attributes
	HIMAGELIST						m_hImageList;
	ATLControls::CListViewCtrl		m_ListView;
	int								m_iSortOrder;
	CVector<int>					m_AltSortOrders;

// Operations
	void SetColumnImage(int iColumn, int iImage);
	void ShowSortOrder();

	virtual int CompareItems(INT_PTR iItem1, INT_PTR iItem2, int iSubItem);

	static int CALLBACK CompareProc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);
};





}	// namespace Simple

#endif	//	__ATLCONTROLS_H__
#endif	//	__ATLWIN_H__

#endif	// __LISTVIEWUTILS_H

