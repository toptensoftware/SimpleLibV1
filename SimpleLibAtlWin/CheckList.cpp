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
// CheckList.cpp - implementation of CCheckList class

#include "stdafx.h"
#include "SimpleLibAtlWinBuild.h"

#include "CheckList.h"
#include "SimpleLibRes.h"

//#pragma comment(lib, "comctl32.lib")

namespace Simple
{

class CItemInfo
{
public:
	CItemInfo() { m_bChecked=FALSE; m_lParam=0; m_iImage=0; };
	BOOL		m_bChecked;
	LPARAM		m_lParam;
	int			m_iImage;
	CUniString	m_strText;
};


//////////////////////////////////////////////////////////////////////////
// CCheckList

// Constructor
CCheckList::CCheckList()
{
	// Load check bits...
	m_hCheckBits=LoadBitmap(_AtlBaseModule.GetResourceInstance(), MAKEINTRESOURCE(IDB_SIMPLELIB_CHECKBITS));
	ASSERT(m_hCheckBits!=NULL && "Failed to load IDB_SIMPLELIB_CHECKBITS - make sure SimpleLib.rc included in projects resources");

	// Setup margin width
	BITMAP bm;
	GetObject(m_hCheckBits, sizeof(bm), &bm);
	m_iMarginWidth=(bm.bmWidth/2)+4;

	// Default image width is 0
	m_iImageWidth=0;

	// No image list by default
	m_hImageList=NULL;
}

// Destructor
CCheckList::~CCheckList()
{
	// Delete check bits
	DeleteObject(m_hCheckBits);
}


// Overriden GetItemData...
LPARAM CCheckList::GetItemData(int nIndex) const
{
	// Get item info
	CItemInfo* pItemInfo=GetItemInfo(nIndex);
	if (pItemInfo==NULL)
		return 0;

	// Return item data
	return pItemInfo->m_lParam;
}

// Override GetItemData
int CCheckList::SetItemData(int nIndex, LPARAM dwItemData)
{
	// Get item info
	CItemInfo* pItemInfo=GetItemInfo(nIndex);
	if (pItemInfo==NULL)
		return LB_ERR;

	// Store new value
	pItemInfo->m_lParam=dwItemData;
	return 0;
}

// Get the check state of an item
BOOL CCheckList::GetItemChecked(int nIndex) const
{
	// Get item info
	CItemInfo* pItemInfo=GetItemInfo(nIndex);
	if (pItemInfo==NULL)
		return FALSE;

	// Return item data
	return pItemInfo->m_bChecked;
}

// Set the check state of an item
int CCheckList::SetItemChecked(int nIndex, BOOL bChecked, BOOL bFireNotification)
{
	// Get item info
	CItemInfo* pItemInfo=GetItemInfo(nIndex);
	if (pItemInfo==NULL)
		return LB_ERR;

	// Store new value
	pItemInfo->m_bChecked=bChecked;

	// Invalidate item...
	RECT rc;
	GetItemRect(nIndex, &rc);
	rc.right=rc.left+m_iMarginWidth;
	InvalidateRect(&rc, TRUE);

	// Fire notification
	if (bFireNotification)
		::SendMessage(GetParent(), WM_COMMAND, MAKEWPARAM(GetDlgCtrlID(), CBLN_STATECHANGED), (LPARAM)m_hWnd);

	// Done
	return 0;
}

BOOL CCheckList::SetItemText(int iItem, LPCOLESTR pszText)
{
	// Get item info
	CItemInfo* pItemInfo=GetItemInfo(iItem);
	if (pItemInfo==NULL)
		return FALSE;

	// Store new text
	pItemInfo->m_strText=pszText;
						
	// Invalidate it							
	RECT rc;
	GetItemRect(iItem, &rc);
	InvalidateRect(&rc);

	return TRUE;
}

CUniString CCheckList::GetItemText(int iItem)
{
	// Get item info
	CItemInfo* pItemInfo=GetItemInfo(iItem);
	if (pItemInfo==NULL)
		return FALSE;

	if (!pItemInfo->m_strText)
		{
		CComBSTR bstr;
		GetTextBSTR(iItem, bstr.m_str);
		pItemInfo->m_strText=bstr;
		}

	return pItemInfo->m_strText;
}



CItemInfo* CCheckList::GetItemInfo(int nIndex) const
{
	CItemInfo* pItemInfo=reinterpret_cast<CItemInfo*>(base::GetItemData(nIndex));
	if (reinterpret_cast<DWORD_PTR>(pItemInfo)==(DWORD_PTR)-1)
		pItemInfo=NULL;
	if (pItemInfo)
		return pItemInfo;

	if ((nIndex>=0) && (nIndex<GetCount()))
		{
		// Allocate new item info
		pItemInfo=new CItemInfo;
		const_cast<CCheckList*>(this)->base::SetItemData(nIndex, (LPARAM)pItemInfo);
		}

	// Return item info pointer
	return pItemInfo;
}

// Setup image list
void CCheckList::SetImageList(HIMAGELIST hImageList)
{
	m_hImageList=hImageList;

	if (!m_hImageList)
		{
		m_iImageWidth=0;
		m_cxImage=0;
		m_cyImage=0;
		}
	else
		{
		// Get size of icon
		ImageList_GetIconSize(m_hImageList, &m_cxImage, &m_cyImage);

		// Setup image area width
		m_iImageWidth=m_cxImage+2;
		}

	UpdateItemHeight();
	Invalidate();
}

int CCheckList::SetItemImage(int nIndex, int iImage)
{
	// Get item info
	CItemInfo* pItemInfo=GetItemInfo(nIndex);
	if (pItemInfo==NULL)
		return LB_ERR;

	// Store new value
	pItemInfo->m_iImage=iImage;

	// Invalidate item...
	RECT rc;
	GetItemRect(nIndex, &rc);
	rc.left=rc.left+m_iMarginWidth;
	rc.right=rc.left+m_iImageWidth;
	InvalidateRect(&rc, TRUE);

	return 0;
}


// Get index of every selected item...
void CCheckList::GetSelectedItems(CVector<int>& Items) const
{
	// Empty the list
	Items.RemoveAll();

	// Get selected count
	int iCount=GetSelCount();
	if (iCount<=0)
		return;
	
	// Allocate memory
	int* piItems=(int*)_alloca(iCount * sizeof(int));

	// Get items
	GetSelItems(iCount, piItems);

	// Copy to array
	for (int i=0; i<iCount; i++)
		{
		Items.Add(piItems[i]);
		}

	// Done
}


void CCheckList::UpdateItemHeight()
{
	// Get font height
	HDC hDC;
	hDC=CreateIC(_T("Display"), NULL, NULL, NULL);

	HFONT hOldFont=(HFONT)SelectObject(hDC, GetFont());
	TEXTMETRIC tm;
	GetTextMetrics(hDC, &tm);
	SelectObject(hDC, hOldFont);
	DeleteDC(hDC);

	// Use font height by default
	int iHeight=tm.tmHeight;

	// Get height of check bitmap
	BITMAP bm;
	GetObject(m_hCheckBits, sizeof(bm), &bm);
	if (bm.bmHeight+4 > iHeight)
		iHeight=bm.bmHeight+4;

	if (m_hImageList)
		{
		if (m_cyImage+1 > iHeight)
			iHeight=m_cyImage+1;
		}

	// Update height
	SetItemHeight(0, iHeight);
}

// OCM_DRAWITEM handler
LRESULT CCheckList::OnDrawItem(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
USES_CONVERSION;

	LPDRAWITEMSTRUCT pdis=(LPDRAWITEMSTRUCT)lParam;

	// Quit if empty
	if (pdis->itemID==-1)
		return 0;

	// Get item info
	CItemInfo* pItemInfo=GetItemInfo(pdis->itemID);
	ASSERT(pItemInfo!=NULL);

	// Get item text
	CUniString str=GetItemText(pdis->itemID);

	// Setup DC
	HDC hDC=pdis->hDC;

	// Paint highlighted?
	BOOL bHighlighted=(pdis->itemState & ODS_SELECTED);

	// Work out text rectangle
	RECT rc(pdis->rcItem);
	rc.left+=m_iMarginWidth+m_iImageWidth;
	
	BOOL bEnabled=IsWindowEnabled();

	// Create highlight brush, fill background
	HBRUSH hBrush=GetSysColorBrush(bHighlighted ? (bEnabled ? COLOR_HIGHLIGHT : COLOR_WINDOW) : COLOR_WINDOW);
	FillRect(hDC, &rc, hBrush);

	// Focus
	if (pdis->itemState & ODS_FOCUS)
		DrawFocusRect(hDC, &rc);

	// Setup text color and mode
	COLORREF rgbTextOld=SetTextColor(hDC, GetSysColor(bHighlighted ? 
				(bEnabled ? COLOR_HIGHLIGHTTEXT : COLOR_GRAYTEXT) : 
				(bEnabled ? COLOR_WINDOWTEXT : COLOR_GRAYTEXT)));
	int iOldMode=SetBkMode(hDC, TRANSPARENT);

	rc.left+=2;
	if (GetStyle() & LBS_USETABSTOPS)
		{
		CVector<CUniString> vec;
		SplitString(str, L"\t", vec);

		for (int i=0; i<vec.GetSize(); i++)
			{
			RECT rcText=rc;

			rcText.left=rc.left + (i<m_TabStops.GetSize() ? m_TabStops[i] : 0);
			rcText.right=rc.left + (i+1<m_TabStops.GetSize() ? m_TabStops[i+1] : rc.right);
			DrawText(hDC, OLE2CT(vec[i]), -1, &rcText, DT_LEFT|DT_SINGLELINE|DT_VCENTER);
			}
		}
	else
		{
		DrawText(hDC, str, -1, &rc, DT_LEFT|DT_SINGLELINE|DT_VCENTER);
		}

	SetTextColor(hDC, rgbTextOld);
	SetBkMode(hDC, iOldMode);

	// Paint check box
	rc=pdis->rcItem;
	rc.right=rc.left+m_iMarginWidth;

	// Get bitmap size
	BITMAP bm;
	GetObject(m_hCheckBits, sizeof(bm), &bm);

	// Create memory device context
	BOOL bChecked=pItemInfo->m_bChecked;

	if (bChecked>=0)
		{
		HDC hdcMem=CreateCompatibleDC(hDC);
		HBITMAP hOldBitmap=(HBITMAP)SelectObject(hdcMem, m_hCheckBits);

		// Paint bitmap
		BitBlt(hDC, rc.left+((rc.right-rc.left)-bm.bmWidth/2)/2, rc.top+((rc.bottom-rc.top)-bm.bmHeight)/2,
					bm.bmWidth/2, bm.bmHeight, hdcMem, bChecked ? 0 : bm.bmWidth/2, 0, SRCCOPY);

		// Restore memory device context
		SelectObject(hdcMem, hOldBitmap);
		DeleteDC(hdcMem);
		}

	// Paint image...
	if (m_hImageList)
		{
		// Work out position to paint image
		POINT pt={rc.right + (m_iImageWidth-m_cxImage)/2,
					rc.top + ((rc.bottom-rc.top) - m_cyImage)/2 };

		// Paint it...
		ImageList_Draw(m_hImageList, pItemInfo->m_iImage, hDC, pt.x, pt.y,
				ILD_TRANSPARENT|(bHighlighted ? ILD_SELECTED : 0));
		}

	// Done
	return 0;
}

// OCM_MEASUREITEM handler
LRESULT CCheckList::OnMeasureItem(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return 0;
}

// WM_KEYDOWN handler
LRESULT CCheckList::OnKeyDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (wParam!=VK_SPACE)
		{
		bHandled=FALSE;
		return 0;
		}

	// Get all the selected items
	CVector<int> Items;
	GetSelectedItems(Items);

	// Quit if no items
	if (Items.GetSize()==0)
		return 0;

	// Set the state of all items to the opposite of the first?
	BOOL bNewState=!GetItemChecked(Items[0]);

	for (int i=0; i<Items.GetSize(); i++)
		{
		if (GetItemChecked(Items[i])>=0)
			{
			SetItemChecked(Items[i], bNewState, TRUE);
			}
		}

	return 0;
}

// OCM_DELETEITEM handler
LRESULT CCheckList::OnDeleteItem(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	LPDELETEITEMSTRUCT lpdis=(LPDELETEITEMSTRUCT)lParam;
	CItemInfo* pItemInfo=GetItemInfo(lpdis->itemID);
	if (pItemInfo)
		delete pItemInfo;

//	if ((lpdis->itemData!=NULL) && (lpdis->itemData!=0xFFFFFFFF))
//		delete reinterpret_cast<CItemInfo*>(lpdis->itemData);
	return 0;
}

// WM_LBUTTONDOWN handler
LRESULT CCheckList::OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	POINT point=GET_PT_LPARAM(lParam);

	int iHeight=GetItemHeight(0);
	// In margin (check box area)
	if (point.x<=iHeight)
		{
		// Ignore if past last line...
		if (point.y >= iHeight * GetCount())
			return 0;

		// Work out which row on...
		BOOL bOutside;
		int iItem=ItemFromPoint(point, bOutside);
		if (bOutside || (iItem<0) || (iItem>=GetCount()))
			return 0;

		// Is this item selected
		CVector<int> SelectedItems;
		GetSelectedItems(SelectedItems);

		// Is the clicked item selected
		int i;
		for (i=0; i<SelectedItems.GetSize(); i++)
			{
			// Found?
			if (SelectedItems[i]==(DWORD)iItem)
				break;
			}

		// Work out new state
		BOOL bNewState=!GetItemChecked(iItem);

		if (i==SelectedItems.GetSize())
			{
			// Item wasn't selected, toggle self only
			if (GetItemChecked(iItem)>=0)
				SetItemChecked(iItem, bNewState, TRUE);
			}
		else
			{
			for (int i=0; i<SelectedItems.GetSize(); i++)
				{
				if (GetItemChecked(SelectedItems[i])>=0)
					SetItemChecked(SelectedItems[i], bNewState, TRUE);
				}
			}
		}
	else
		{
		bHandled=FALSE;
		}

	return 0;
}

// WM_LBUTTONUP handler
LRESULT CCheckList::OnLButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled=FALSE;
	return 0;
}

// WM_LBUTTONDBLCLK handler
LRESULT CCheckList::OnLButtonDblClk(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	POINT point=GET_PT_LPARAM(lParam);
	if (point.x<=GetItemHeight(0))
		return OnLButtonDown(uMsg, wParam, lParam, bHandled);

	bHandled=FALSE;
	return 0;
}

// WM_ENABLE handler
LRESULT CCheckList::OnEnable(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	Invalidate();
	bHandled=FALSE;
	return 0;
}

// WM_SETFONT handler
LRESULT CCheckList::OnSetFont(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	// Do default processing...
	LRESULT lRetv=DefWindowProc(uMsg, wParam, lParam);

	UpdateItemHeight();

	bHandled=TRUE;
	return lRetv;
}

void CCheckList::SelectSingleItem(int iItem)
{
	int iCount=GetCount();
	for (int i=0; i<iCount; i++)
		{
		::SendMessage(m_hWnd, LB_SETSEL, i==iItem, i);
		}
}


LRESULT CCheckList::OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	ResetContent();
	bHandled = FALSE;
	return 0;
}

// WM_CHAR Handler
LRESULT CCheckList::OnChar(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (wParam==' ')
		return 0;

	bHandled=FALSE;
	return 0;
}

// LBN_SETTABSTOPS handler
LRESULT CCheckList::OnSetTabStops(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	m_TabStops.RemoveAll();
	for (int i=0; i<int(wParam); i++)
		{
		m_TabStops.Add(((int*)lParam)[i]);
		}
	return TRUE;
}


}	// namespace Simple
