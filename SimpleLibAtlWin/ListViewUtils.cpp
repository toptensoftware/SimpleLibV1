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
// ListViewUtils.cpp - implementation of ListViewUtils

#include "stdafx.h"
#include "SimpleLibAtlWinBuild.h"

#include "ListViewUtils.h"

namespace Simple
{

void SIMPLEAPI SelectSingleItem(ATLControls::CListViewCtrl& ListView, int iItem)
{
	int iSel=-1;
	while  ( (iSel=ListView.GetNextItem(iSel, LVNI_SELECTED)) >= 0 )
		{
		if (iSel!=iItem)
			ListView.SetItemState(iSel, 0, LVIS_SELECTED);
		}

	ListView.SetItemState(iItem, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);
}


void SIMPLEAPI SelectAllItems(ATLControls::CListViewCtrl& ListView)
{
	for (int i=ListView.GetItemCount()-1; i>=0; i--)
		{
		ListView.SetItemState(i, LVIS_SELECTED, LVIS_SELECTED);
		}
}

void SIMPLEAPI ToggleCheckState(ATLControls::CListViewCtrl& ListView)
{
	// See if all selected items are checked
	bool bAllSelected=true;
	int iItem=-1;
	while ((iItem=ListView.GetNextItem(iItem, LVNI_SELECTED))>=0)	
		{
		if (ListView.GetCheckState(iItem)!=1)
			{
			bAllSelected=false;
			break;
			}
		}

	// Setup new check state of each item
	iItem=-1;
	while ((iItem=ListView.GetNextItem(iItem, LVNI_SELECTED))>=0)
		{
		ListView.SetCheckState(iItem, bAllSelected ? 0 : 1);
		}
}


void SIMPLEAPI LoadColumnWidths(ATLControls::CListViewCtrl& ListView, const wchar_t* pszSection, const wchar_t* pszPrefix)
{
	OLECHAR szText[MAX_PATH];
	ATLControls::CHeaderCtrl header=ListView.GetHeader();
	for (int i=0; i<header.GetItemCount(); i++)
		{
		HDITEM hdi;
		memset(&hdi, 0, sizeof(hdi));
		hdi.mask=HDI_TEXT;
		hdi.pszText=szText;
		hdi.cchTextMax=_countof(szText);
		header.GetItem(i, &hdi);
		int iWidth=SlxGetProfileInt(pszSection, Format(L"%s%sWidth", pszPrefix, szText), -1);
		if (iWidth>=0)
			{
			hdi.mask=HDI_WIDTH;
			hdi.cxy=iWidth;
			header.SetItem(i, &hdi);
			}
		}
}

void SIMPLEAPI SaveColumnWidths(ATLControls::CListViewCtrl& ListView, const wchar_t* pszSection, const wchar_t* pszPrefix)
{
	OLECHAR szText[MAX_PATH];
	ATLControls::CHeaderCtrl header=ListView.GetHeader();
	for (int i=0; i<header.GetItemCount(); i++)
		{
		HDITEM hdi;
		memset(&hdi, 0, sizeof(hdi));
		hdi.mask=HDI_WIDTH|HDI_TEXT;
		hdi.pszText=szText;
		hdi.cchTextMax=_countof(szText);
		header.GetItem(i, &hdi);
		SlxSetProfileInt(pszSection, Format(L"%s%sWidth", pszPrefix, szText), hdi.cxy);
		}
}


//////////////////////////////////////////////////////////////////////////
// CListViewSorter

// Constructor
CListViewSorter::CListViewSorter()
{
	m_iSortOrder=0;
	m_hImageList=NULL;
}

// Destructor
CListViewSorter::~CListViewSorter()
{
	if (m_hImageList)
		ImageList_Destroy(m_hImageList);
}

// Initialise this sorter...
void CListViewSorter::Init(HWND hWndListView, int iResIdSortIcons)
{
	// Attach list view
	m_ListView.Detach();
	m_ListView.Attach(hWndListView);
	
	// Setup image list
	if (m_hImageList==NULL)
		m_hImageList=ImageList_LoadImage(_AtlBaseModule.GetResourceInstance(), MAKEINTRESOURCE(iResIdSortIcons), 9, 0, RGB(192,192,192), IMAGE_BITMAP, 0);
	ATLControls::CHeaderCtrl header(m_ListView.GetHeader());
	header.SetImageList(m_hImageList);
}

// Call in response to LVN_COLUMNCLICK notification
void CListViewSorter::OnColumnClick(int iColumn)
{	
	LVCOLUMN c;
	memset(&c, 0, sizeof(c));
	c.mask=LVCF_SUBITEM;
	m_ListView.GetColumn(iColumn, &c);

	iColumn=c.iSubItem;

	if (iColumn==abs(m_iSortOrder)-1)
		{
		SetSortOrder(-m_iSortOrder);
		}
	else
		{
		SetSortOrder(iColumn+1);
		}
}


void CListViewSorter::SetSortOrder(int iSortOrder)
{
	if (m_iSortOrder==iSortOrder)
		return;

	m_iSortOrder=iSortOrder;

	ShowSortOrder();
	Resort();
}

int CListViewSorter::GetSortOrder()
{
	return m_iSortOrder;
}

void CListViewSorter::Resort()
{
	m_ListView.SendMessage(LVM_SORTITEMSEX, (WPARAM)this, (LPARAM)CompareProc);
}



void CListViewSorter::AddAltSortOrder(int iColumn)
{
	m_AltSortOrders.Add(iColumn);
}

// Helper to set image on a column
void CListViewSorter::SetColumnImage(int iColumn, int iImage)
{
	LVCOLUMN lvc;
	memset(&lvc, 0, sizeof(lvc));
	lvc.mask=LVCF_IMAGE|LVCF_FMT;
	m_ListView.GetColumn(iColumn, &lvc);

	lvc.iImage=iImage;
	lvc.fmt=(lvc.fmt & ~LVCFMT_IMAGE) | (iImage>0 ? LVCFMT_IMAGE : 0);
	m_ListView.SetColumn(iColumn, &lvc);
}

// Show sort order
void CListViewSorter::ShowSortOrder()
{
	ATLControls::CHeaderCtrl header(m_ListView.GetHeader());

	int iColumns=header.GetItemCount();
	for (int i=0; i<iColumns; i++)
		{
		LVCOLUMN c;
		memset(&c, 0, sizeof(c));
		c.mask=LVCF_SUBITEM;
		m_ListView.GetColumn(i, &c);


		if (c.iSubItem==abs(m_iSortOrder)-1)
			{
			SetColumnImage(i, m_iSortOrder>0 ? 1 : 2);
			}
		else
			{
			SetColumnImage(i, 0);
			}
		}
}

int CListViewSorter::CompareItems(INT_PTR iItem1, INT_PTR iItem2, int iSubItem)
{
USES_CONVERSION;

	// Get item 1's text
	TCHAR szItem1[MAX_PATH];
	LVITEM lvi;
	lvi.mask=LVIF_TEXT;
	lvi.iSubItem=iSubItem;
	lvi.pszText=szItem1;
	lvi.cchTextMax=MAX_PATH;
	m_ListView.SendMessage(LVM_GETITEMTEXT, iItem1, (LPARAM)&lvi);

	// Get item 2's text
	TCHAR szItem2[MAX_PATH];
	lvi.mask=LVIF_TEXT;
	lvi.iSubItem=iSubItem;
	lvi.pszText=szItem2;
	lvi.cchTextMax=MAX_PATH;
	m_ListView.SendMessage(LVM_GETITEMTEXT, iItem2, (LPARAM)&lvi);

	return SmartStringCompare(T2COLE(szItem1), T2COLE(szItem2));
}


int CALLBACK CListViewSorter::CompareProc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
USES_CONVERSION;

	CListViewSorter* pThis=(CListViewSorter*)lParamSort;

	// Do primary sort order comparison
	int iCompare=(int)pThis->CompareItems(lParam1, lParam2, abs(pThis->m_iSortOrder)-1);
	
	// If items are equal, compare by alt columns
	for (int i=0; iCompare==0 && i<pThis->m_AltSortOrders.GetSize(); i++)
		{
		// Get alt sub item
		int iSubItem=pThis->m_AltSortOrders[i];

		// Ignore
		if (iSubItem==abs(pThis->m_iSortOrder)-1)
			continue;

		// Compare sub item
		iCompare=(int)pThis->CompareItems(lParam1, lParam2, iSubItem);
		}

	// Apply direction...
	return pThis->m_iSortOrder<0 ? -iCompare : iCompare;

}




}