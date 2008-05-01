//////////////////////////////////////////////////////////////////////////
// MenuIcons.cpp - implementation of CMenuIcons class

#include "stdafx.h"
#include "SimpleLibAtlWinBuild.h"

#include "MenuIcons.h"

namespace Simple
{


#define RT_TOOLBAR  MAKEINTRESOURCE(241)


#define ICON_SIZE_X 20
#define ICON_SIZE_Y 20
#define ICON_X		1
#define ICON_Y		1
#define MAIN_X		(ICON_X*2 + ICON_SIZE_X + 1)
#define MIN_HEIGHT	(ICON_Y*2 + ICON_SIZE_Y)
#define TEXT_R_MARGIN	20
#define TEXT_X		(MAIN_X + 8)
#define MENU_BAR_MARGIN 3
#define ICON_CX 16
#define ICON_CY 16

/*
COLORREF rgbColors[] = 
{
0x00c6e7ff,  // 0: COLOR_SCROLLBAR
0x00100010,  // 1: COLOR_BACKGROUND
0x00603367,  // 2: COLOR_ACTIVECAPTION
0x008cceff,  // 3: COLOR_INACTIVECAPTION
0x008cceff,  // 4: COLOR_MENU
0x00b8dbfa,  // 5: COLOR_WINDOW
0x00000000,  // 6: COLOR_WINDOWFRAME
0x00000000,  // 7: COLOR_MENUTEXT
0x00000000,  // 8: COLOR_WINDOWTEXT
0x00c4d5ee,  // 9: COLOR_CAPTIONTEXT
0x008cceff,  // 10: COLOR_ACTIVEBORDER
0x008cceff,  // 11: COLOR_INACTIVEBORDER
0x004f004f,  // 12: COLOR_APPWORKSPACE
0x00695a9a,  // 13: COLOR_HIGHLIGHT
0x00fffffe,  // 14: COLOR_HIGHLIGHTTEXT
0x008cceff,  // 15: COLOR_BTNFACE
0x000996ff,  // 16: COLOR_BTNSHADOW
0x000996ff,  // 17: COLOR_GRAYTEXT
0x00000000,  // 18: COLOR_BTNTEXT
0x00564a7e,  // 19: COLOR_INACTIVECAPTIONTEXT
0x00c6e7ff,  // 20: COLOR_BTNHIGHLIGHT
0x00000000,  // 21: COLOR_3DDKSHADOW
0x008cceff,  // 22: COLOR_3DLIGHT
0x00000000,  // 23: COLOR_INFOTEXT
0x00aeffff,  // 24: COLOR_INFOBK
0x00b5b5b5,  // 25: COLOR_UNUSED           
0x00695a9a,  // 26: COLOR_HOTLIGHT
0x00603367,  // 27: COLOR_GRADIENTACTIVECAPTION
0x00603367,  // 28: COLOR_GRADIENTINACTIVECAPTION
0x00000000,  // 29: COLOR_MENUHILIGHT
0x00000000,  // 30: COLOR_MENUBAR
};

COLORREF GetSysColor2(int iIndex)
{
	if (iIndex<0 || iIndex>=_countof(rgbColors))
		return ::GetSysColor(iIndex);
	else
		return rgbColors[iIndex];
}							

//GetSysColor(COLOR_3DFACE)

#define GetSysColor GetSysColor2
*/

const DWORD CMenuIcons::CItemData::Signature=0x19345292;


#ifndef ODS_NOACCEL
#define ODS_NOACCEL         0x0100
#endif

static void FillSolidRect(HDC hDC, const RECT& rc, COLORREF color)
{
	HBRUSH hbrOld = (HBRUSH) ::SelectObject(hDC, CreateSolidBrush(color));
	::PatBlt(hDC, rc.left, rc.top, rc.right-rc.left, rc.bottom-rc.top, PATCOPY);
	::DeleteObject(::SelectObject(hDC, hbrOld));
}

static bool SIMPLEAPI Draw3DCheckmark(HDC hDC, RECT* prc, COLORREF crBg)
{
#ifndef OBM_CHECK
#define OBM_CHECK 32760 // from winuser.h
#endif

	// Get checkmark bitmap if none, use Windows standard
	CSmartHandle<HBITMAP> hbmCheck= LoadBitmap(NULL, MAKEINTRESOURCE(OBM_CHECK));

	// Get checkmark bitmap size
	BITMAP bm;
	GetObject(hbmCheck, sizeof(bm), &bm);

	// Select check mark
	CSmartHandle<HDC> hdcMem=CreateCompatibleDC(hDC);
	CSelectObject hbmpOld(hdcMem, hbmCheck);

	SetBkMode(hDC, TRANSPARENT);
	COLORREF rgbOld=SetBkColor(hDC, crBg);

	BitBlt(hDC, prc->left + ((prc->right-prc->left)-bm.bmWidth)/2+1,
				prc->top + ((prc->bottom-prc->top)-bm.bmHeight)/2-1,
				bm.bmWidth, bm.bmHeight,
			hdcMem, 0, 0, SRCCOPY);			 

	SetBkColor(hDC, rgbOld);


	return TRUE;
}



void DrawMenuText(HDC hDC, RECT* prc, const wchar_t* pszText, COLORREF color)
{
	// Split main text and accelerator text
	CUniString strLeft;
	CUniString strRight;
	SplitString(pszText, L"\t", strLeft, strRight);

	SetTextColor(hDC, color);
	DrawText(hDC, strLeft, -1, prc, DT_SINGLELINE|DT_LEFT|DT_VCENTER);

	if (!IsEmptyString(strRight))
		DrawText(hDC, strRight, -1, prc, DT_SINGLELINE|DT_RIGHT|DT_VCENTER);
}

//////////////////////////////////////////////////////////////////////////
// CMenuIcons

// Constructor
CMenuIcons::CMenuIcons()
//	: m_Focus(this, 1)
{
	m_bEnableNavKeys=false;
	m_hSelMenu=NULL;
	m_iSelMenu=-1;
	m_hNavHook=NULL;
	m_iItemHeight=1;
	m_bEnable=true;
	m_hWnd=NULL;
	m_pfnOldWndProc=NULL;
}

// Destructor
CMenuIcons::~CMenuIcons()
{
	for (int i=0; i<m_ImageLists.GetSize(); i++)
		ImageList_Destroy(m_ImageLists[i]);
	UnsubclassWindow();
}


// structure of RT_TOOLBAR resource
struct TOOLBARDATA {
	WORD wVersion;		// version # should be 1
	WORD wWidth;		// width of one bitmap
	WORD wHeight;		// height of one bitmap
	WORD wItemCount;	// number of items
	WORD items[1];		// array of command IDs, actual size is wItemCount
};


// Load icons from a toolbar...
BOOL CMenuIcons::LoadToolbar(HINSTANCE hInstance, UINT nIDToolbar, COLORREF rgbMask, UINT nIDToolbar32)
{
	// Load image list
	HIMAGELIST hImageList=ImageList_LoadEx(hInstance, MAKEINTRESOURCE(nIDToolbar), MAKEINTRESOURCE(nIDToolbar32), rgbMask, 16);
	HIMAGELIST hImageListGray=ImageList_LoadGrayScale(hInstance, MAKEINTRESOURCE(nIDToolbar32), 16, false);
	HIMAGELIST hImageListShadow=ImageList_LoadGrayScale(hInstance, MAKEINTRESOURCE(nIDToolbar32), 16, true);

	// load toolbar
	HRSRC hRsrc=FindResource(hInstance, MAKEINTRESOURCE(nIDToolbar), RT_TOOLBAR);
	if (!hRsrc)
		{
		ImageList_Destroy(hImageList);
		if (hImageListGray)
			ImageList_Destroy(hImageListGray);
		if (hImageListShadow)
			ImageList_Destroy(hImageListShadow);
		return FALSE;
		}

	// Keep image list
	m_ImageLists.Add(hImageList);
	if (hImageListGray)
		m_ImageLists.Add(hImageListGray);
	if (hImageListShadow)
		m_ImageLists.Add(hImageListShadow);

	// Get tool bar
	TOOLBARDATA* pToolBar=(TOOLBARDATA*)LockResource(LoadResource(hInstance, hRsrc));
	ASSERT(pToolBar->wVersion==1);
	ASSERT(pToolBar->wWidth==16);
		
	int iIcon=0;
	for (int i = 0; i < pToolBar->wItemCount; i++) 
		{
		if (pToolBar->items[i]>0)
			{
			m_Icons.Add(pToolBar->items[i], new CIconInfo(hImageList, hImageListGray, hImageListShadow, iIcon++));
			}
		}

	return TRUE; // success!
}


void CMenuIcons::ClearBold()
{
	m_BoldSubMenus.RemoveAll();
}

void CMenuIcons::BoldSubMenu(HMENU hSubMenu)
{
	m_BoldSubMenus.Add(hSubMenu, TRUE);
}

void CMenuIcons::EnableNavKeys(bool bEnable)
{
	m_bEnableNavKeys=bEnable;
}

void CMenuIcons::EnableAlphaNav(HMENU hMenu, bool bEnable)
{
	ASSERT(m_bEnableNavKeys);
	if (!bEnable)
		{
		m_AlphaNavMenus.Remove(hMenu);
		}
	else
		{
		m_AlphaNavMenus.Add(hMenu, TRUE);
		}
}

void CMenuIcons::Enable(bool bEnable)
{
	m_bEnable=bEnable;
}

/*
void CMenuIcons::SubclassForMouseWheel(bool bInstall)
{
	if (bInstall)
	{
		ASSERT(m_Focus.m_hWnd==NULL);

		HWND hWndFocus=GetFocus();
		if (hWndFocus)
			m_Focus.SubclassWindow(hWndFocus);
	}
	else
	{
		if (m_Focus.IsWindow())
			m_Focus.UnsubclassWindow();
	}
}

LRESULT CMenuIcons::OnFocusMouseWheel(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	int iDelta=GET_Y_LPARAM(wParam)/120;

	if (iDelta>0)
		PostMessage(WM_KEYDOWN, VK_DOWN);
	else
		PostMessage(WM_KEYDOWN, VK_UP);

	return 0;
}

*/




/////////////////////////////////////////////////////////////////////////////
// Operations

HFONT CMenuIcons::GetMenuFont()
{
	if (!m_hFont) 
		{
		NONCLIENTMETRICS info;
		info.cbSize = sizeof(info);
		SystemParametersInfo(SPI_GETNONCLIENTMETRICS, sizeof(info), &info, 0);
		m_hFont=CreateFontIndirect(&info.lfMenuFont);
		}

	return m_hFont;
}

void CMenuIcons::ConvertMenu(HMENU hMenu, bool bConvert)
{
	USES_CONVERSION;

	UINT nCount = GetMenuItemCount(hMenu);
	if (nCount==0xFFFFFFFF)
	{
		ATLTRACE(_T("Invalid menu handle, ignored\n"));
		return;
	}
	for (UINT i = 0; i < nCount; i++)
	{
		// Get item info
		TCHAR szName[MAX_PATH];
		MENUITEMINFO mii;
		memset(&mii, 0, sizeof(MENUITEMINFO));
		mii.cbSize = 44;
		mii.fMask = MIIM_SUBMENU | MIIM_DATA | MIIM_ID | MIIM_TYPE;
		mii.dwTypeData = szName;
		mii.cch = _countof(szName);
		::GetMenuItemInfo(hMenu, i, TRUE, &mii);

		// Ignore system menu
		if (mii.wID >= 0xF000 && mii.wID<0xFF00)
			return;

		// Get our data...
		CItemData* pItemData=(CItemData*)mii.dwItemData;

		// Ignore unrecognized items data
		if (pItemData && pItemData->m_dwSignature!=CItemData::Signature)
			continue;

		// Modify it
		mii.fMask = 0;

		CComBSTR bstrText;
		if (bConvert)
		{
			// Convert it
			if (!(mii.fType & MFT_OWNERDRAW))
			{
				mii.fType |= MFT_OWNERDRAW;
				mii.fMask |= MIIM_TYPE;
				if (!pItemData)
				{
					pItemData = new CItemData();
					pItemData->m_fType = mii.fType;
					pItemData->m_pIcon = m_Icons.Get(mii.wID, NULL);
					mii.dwItemData = (DWORD_PTR)pItemData;
					mii.fMask |= MIIM_DATA;
					pItemData->m_bBold=!!m_BoldSubMenus.Get(mii.hSubMenu, NULL);
				}
				pItemData->m_bstrText = mii.dwTypeData ? mii.dwTypeData : _T("");
				if (!pItemData->m_bstrText)
					pItemData->m_bstrText=L"";
			}

			// now add the menu to list of "converted" menus
			if (m_ConvertedMenus.Find(hMenu)<0)
				m_ConvertedMenus.Add(hMenu);
		}
		else
		{
			if (mii.fType & MFT_OWNERDRAW)
			{
				mii.fType &= ~MFT_OWNERDRAW;
				mii.fMask |= MIIM_TYPE;
				bstrText = pItemData->m_bstrText;
			}
			else
			{
				bstrText = mii.dwTypeData ? mii.dwTypeData : _T("");		//   use name from MENUITEMINFO
			}

			if (pItemData)
			{
				mii.dwItemData = NULL;
				mii.fMask |= MIIM_DATA;
				delete pItemData;
			}

			if (mii.fMask & MIIM_TYPE)
			{
				// if setting name, copy name from CString to buffer and set cch
				mii.dwTypeData = bstrText;
				mii.cch = bstrText.Length();
			}
		}

		// if after all the above, there is anything to change, change it
		if (mii.fMask)
		{
			SetMenuItemInfo(hMenu, i, TRUE, &mii);
		}
	}
}



/////////////////////////////////////////////////////////////////////////////
// Message handlers


// WM_DRAWITEM Handler
LRESULT CMenuIcons::OnDrawItem(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	// Crack parameters
	DRAWITEMSTRUCT* pdis=(DRAWITEMSTRUCT*)lParam;

	// Quit if not a menu
	if (pdis->CtlType!=ODT_MENU)
		{
		bHandled=FALSE;
		return 0;
		}

	// Get item data and check signature
	CItemData* pItemData=(CItemData*)pdis->itemData;
	if (!pItemData || pItemData->m_dwSignature!=CItemData::Signature)
		{
		bHandled=FALSE;
		return 0;
		}

	// Get draw info
	HDC hDC = pdis->hDC;
	RECT& rcItem = pdis->rcItem;

	// set up the colors we use
	COLORREF colorWindow = GetSysColor(COLOR_WINDOW);
	COLORREF colorMenuBar = GetSysColor(COLOR_3DFACE); // GetMenuBarColor(m_hMenu);
	COLORREF colorMenu = GetMixedColor(colorWindow, colorMenuBar);
	COLORREF colorBitmap = GetMixedColor(colorMenuBar, colorWindow);
	COLORREF colorSel = GetXPHighlightColor(XPSTATE_HOT, colorMenu);
	COLORREF colorBorder = GetSysColor(COLOR_HIGHLIGHT);
	COLORREF colorCheckedBitmap = GetXPHighlightColor(XPSTATE_CHECKED, colorBitmap);

	// define the rectangles we use
	RECT rcLeft = rcItem;	rcLeft.right = rcItem.left + MAIN_X;
	RECT rcRight= rcItem;	rcRight.left = rcItem.left + MAIN_X;
	RECT rcText = rcItem;	rcText.left  = rcItem.left + TEXT_X; rcText.right -= TEXT_R_MARGIN;
	RECT rcIcon = rcItem;	::InflateRect(&rcIcon, -ICON_X, -ICON_Y); rcIcon.right = rcIcon.left + ICON_SIZE_X; 

	if (pItemData->m_fType & MFT_SEPARATOR) 
		{
		// draw separator
		FillSolidRect(hDC, rcLeft, colorBitmap);
		FillSolidRect(hDC, rcRight, colorMenu);
		RECT rcSep = rcRight;
		rcSep.left = rcText.left;
		rcSep.top += (rcSep.bottom-rcSep.top)>>1;						// vertical center
		::DrawEdge(hDC, &rcSep, EDGE_ETCHED, BF_TOP);
		} 
	else 
		{
		bool bDisabled = !!(pdis->itemState & ODS_GRAYED);
		bool bSelected = !!(pdis->itemState & ODS_SELECTED);
		bool bChecked  = !!(pdis->itemState & ODS_CHECKED);
		bool bHasIcon = false;


		if (bSelected)
			{
			colorCheckedBitmap = GetXPHighlightColor(XPSTATE_CHECKED|XPSTATE_HOT, colorBitmap);//MixedColor(colorCheckedBitmap, colorBorder);
			//
			// draw the selection rect
			FillSolidRect(hDC, rcItem, colorSel);

			HPEN hPen=CreatePen(PS_SOLID, 1, colorBorder);
			HPEN hOldPen=(HPEN)SelectObject(hDC, hPen);
			HBRUSH hOldBrush=(HBRUSH)SelectObject(hDC, GetStockObject(NULL_BRUSH));
			Rectangle(hDC, rcItem.left, rcItem.top, rcItem.right, rcItem.bottom);
			SelectObject(hDC, hOldPen);
			SelectObject(hDC, hOldBrush);
			DeleteObject(hPen);
			}
		else
			{
			FillSolidRect(hDC, rcLeft, colorBitmap);
			FillSolidRect(hDC, rcRight, colorMenu);
			}

		if (bChecked)
			{
			//
			// draw XP style check background

			FillSolidRect(hDC, rcIcon, colorCheckedBitmap);

			HPEN hPen=CreatePen(PS_SOLID, 1, colorBorder);
			HPEN hOldPen=(HPEN)SelectObject(hDC, hPen);
			HBRUSH hOldBrush=(HBRUSH)SelectObject(hDC, GetStockObject(NULL_BRUSH));
			Rectangle(hDC, rcIcon.left, rcIcon.top, rcIcon.right, rcIcon.bottom);
			SelectObject(hDC, hOldPen);
			SelectObject(hDC, hOldBrush);
			DeleteObject(hPen);
			}

		

		if (pItemData->m_pIcon) 
			{
			// this item has a button!
			bHasIcon= true;

			POINT pt;
			pt.x = (rcIcon.left + rcIcon.right)/2 - ICON_CX/2;
			pt.y = (rcIcon.top + rcIcon.bottom)/2 - ICON_CY/2+1;

			if (bDisabled)
				{
				if (pItemData->m_pIcon->m_hImageListGray)
					{
					ImageList_Draw(pItemData->m_pIcon->m_hImageListGray, pItemData->m_pIcon->m_iIcon, hDC, pt.x, pt.y, ILD_TRANSPARENT);
					}
				else
					{
					HICON hIcon=ImageList_ExtractIcon(NULL, pItemData->m_pIcon->m_hImageList, pItemData->m_pIcon->m_iIcon);
					ASSERT(hIcon);
					DrawState(hDC, (HBRUSH)NULL, NULL, (LPARAM) hIcon,0, pt.x, pt.y, 0,0, DST_ICON|DSS_DISABLED);
					DestroyIcon(hIcon);
					}
				}
			else
				{		
				if (bSelected && !bChecked)
					{
					if (pItemData->m_pIcon->m_hImageListShadow)
						{
						ImageList_Draw(pItemData->m_pIcon->m_hImageListShadow, pItemData->m_pIcon->m_iIcon, hDC, pt.x+1, pt.y+1, ILD_TRANSPARENT);
						ImageList_Draw(pItemData->m_pIcon->m_hImageList, pItemData->m_pIcon->m_iIcon, hDC, pt.x-1, pt.y-1, ILD_TRANSPARENT);
						}
					else
						{
						HBRUSH hBrush=CreateSolidBrush( GetMixedColor(GetMidColor(GetSysColor(COLOR_BTNSHADOW),GetSysColor(COLOR_3DDKSHADOW)), colorSel));
						HICON hIcon=ImageList_ExtractIcon(NULL, pItemData->m_pIcon->m_hImageList, pItemData->m_pIcon->m_iIcon);
						DrawState(hDC, hBrush, NULL, (LPARAM) hIcon,0, pt.x+1, pt.y+1, 0,0, DST_ICON|DSS_MONO);
						DrawIconEx(hDC, pt.x-1, pt.y-1, hIcon, ICON_CX, ICON_CY, 0, NULL, DI_NORMAL);
						DestroyIcon(hIcon);
						DeleteObject(hBrush);
						}
					}
				else
					{
					ImageList_Draw(pItemData->m_pIcon->m_hImageList, pItemData->m_pIcon->m_iIcon, hDC, pt.x, pt.y, ILD_TRANSPARENT);
					}
				}

			} 
		else 
			{
			// no button: look for custom checked/unchecked bitmaps
			if (bChecked) 
				{
				bHasIcon = true;
				Draw3DCheckmark(hDC, &rcIcon, colorCheckedBitmap);
				}
			}

		//
		// Draw the text. 

		// compute text rectangle and colors
		SetBkMode(hDC, TRANSPARENT);			 // paint transparent text
		COLORREF colorText = GetSysColor(bDisabled ?  COLOR_GRAYTEXT : COLOR_MENUTEXT);

		CSelectObject hOldFont;
		CSmartHandle<HFONT>	hBoldFont;
		if (pItemData->m_bBold)
			{
			LOGFONT lf;
			GetObject(GetCurrentObject(hDC, OBJ_FONT), sizeof(LOGFONT), &lf);
			lf.lfWeight=FW_BOLD;
			hBoldFont=CreateFontIndirect(&lf);
			hOldFont.Select(hDC, hBoldFont);
			}				   

		// Now paint menu item text.	No need to select font,
		// because windows sets it up before sending WM_DRAWITEM
		//
		if (bDisabled && !bSelected) 
			{
			// disabled: draw hilite text shifted southeast 1 pixel for embossed
			// look. Don't do it if item is selected, tho--unless text color same
			// as menu highlight color. Got it?
			//
			OffsetRect(&rcText, 1, 1);
			DrawMenuText(hDC, &rcText, pItemData->m_bstrText, GetSysColor(COLOR_3DHILIGHT));
			OffsetRect(&rcText, -1, -1);
			}


		DrawMenuText(hDC, &rcText, pItemData->m_bstrText, colorText);
	}

	return TRUE;
}

// WM_MEASUREITEM Handler
LRESULT CMenuIcons::OnMeasureItem(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled=FALSE;

	// Crack params
	MEASUREITEMSTRUCT* pmis=(MEASUREITEMSTRUCT*)lParam;

	// Quit if not a menu
	if (pmis->CtlType!=ODT_MENU)
		return 0;

	// Get item data and check signature
	CItemData* pItemData=(CItemData*)pmis->itemData;
	if (!pItemData || pItemData->m_dwSignature!=CItemData::Signature)
		return 0;

	// Separator?
	if (pItemData->m_fType & MFT_SEPARATOR)	
		{
		pmis->itemHeight = GetSystemMetrics(SM_CYMENU) / 2;
		pmis->itemWidth  = 0;
		bHandled=TRUE;
		return TRUE;
		}

	// Calculate size of text
	SIZE sizeText=MeasureDrawText(NULL, GetMenuFont(), pItemData->m_bstrText);

	// Setup height
	pmis->itemHeight=sizeText.cy*3/2+2;
	m_iItemHeight=pmis->itemHeight;

	// Setup width
	pmis->itemWidth = sizeText.cx + MAIN_X + TEXT_R_MARGIN;

	// Add tab spacing
	if (wcschr(pItemData->m_bstrText, L'\t'))
		pmis->itemWidth += 20;

	return TRUE; // handled
}

// WM_INITMENUPOPUP Handler
LRESULT CMenuIcons::OnInitMenuPopup(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	// Do default processing first..
	LRESULT lResult=DefWindowProc(uMsg, wParam, lParam);

	if (m_bEnable)
	{
		m_hSelMenu=(HMENU)wParam;
		m_iSelMenu=-1;

		if (!HIWORD(lParam))
		{
			ConvertMenu((HMENU)wParam, true);
			InstallNavHook();
		}
	}

	return lResult;
}

// Find the index of a menu item with a specific menu ID
int IndexOfMenuItem(HMENU hMenu, UINT nID)
{
	int iCount=GetMenuItemCount(hMenu);
	for (int i=0; i<iCount; i++)
		{
		if (GetMenuItemID(hMenu, i)==nID)
			return i;
		}

	return -1;
}


// WM_MENUSELECT Handler
LRESULT CMenuIcons::OnMenuSelect(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled=FALSE;

	if (lParam==NULL && HIWORD(wParam)==0xFFFF) 
		{
		for (int i=m_ConvertedMenus.GetSize()-1; i>=0; i--)
			{
			ConvertMenu(m_ConvertedMenus[i], false);
			}
		m_ConvertedMenus.RemoveAll();

		RemoveNavHook();
		m_hSelMenu=NULL;
		m_iSelMenu=-1;
		}
	else
		{
		m_hSelMenu=(HMENU)lParam;
		if (HIWORD(wParam) & MF_POPUP)
			{
			m_iSelMenu=LOWORD(wParam);
			}
		else
			{
			m_iSelMenu=IndexOfMenuItem(m_hSelMenu, LOWORD(wParam));
			}
		}



	return 0;
}


int SelectItem(HMENU hMenu, TCHAR ch, int iCurrentSel)
{
	// Get item count
	int iCount=GetMenuItemCount(hMenu);

	// Sanity check
	ASSERT(iCurrentSel>=-1 && iCurrentSel<iCount);

	ch=toupper(ch);

	TCHAR chBest;
	int iBest=-1;

	// Enumerate menu
	int i=iCurrentSel+1;
	while (true)
		{
		// Get menu string
		TCHAR sz[MAX_PATH];
		GetMenuString(hMenu, i, sz, MAX_PATH, MF_BYPOSITION);

		// What character for this item?
		TCHAR chThis=toupper(sz[0]=='&' ? sz[1] : sz[0]);

		// Match?
		if (ch==chThis)
			{
			return i;
			}

		if (chThis > ch)
			{
			if (iBest==-1 || chThis < chBest)
				{
				iBest=i;
				chBest=chThis;
				}
			if (i<iBest && chThis==chBest)
				{
				iBest=i;
				}
			}	

		
		// Back at current selection, no match found
		if (i==iCurrentSel)
			break;

		// Next...
		i++;
		if (i>=iCount)
			i=0;

		}

	return iBest;
}





// WM_MENUCHAR Handler
LRESULT CMenuIcons::OnMenuChar(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (LOWORD(wParam)==' ')
		{
		int iPageSize=GetSystemMetrics(SM_CYSCREEN) / m_iItemHeight-3;
		if (iPageSize<1)
			iPageSize=1;
		ATLTRACE(_T("OnMenuChar from Hook - %i\n"), m_wParamHooked);

		WPARAM wParamHooked=m_wParamHooked;
		m_wParamHooked=0;

		TCHAR ch=(TCHAR)LOWORD(wParamHooked);
		if ((ch>='0' && ch<='9') || 
			(ch>='A' && ch<='Z'))
			{
			int iSel=SelectItem(m_hSelMenu, ch, m_iSelMenu);
			if (iSel>=0)
				return MAKELONG(iSel, MNC_SELECT);
			}

		switch (LOWORD(wParamHooked))
			{
			case VK_HOME:
				m_iSelMenu=0;
				return MAKELONG(0, MNC_SELECT);

			case VK_END:
				return MAKELONG(GetMenuItemCount(m_hSelMenu)-1, MNC_SELECT);

			case VK_PRIOR:
				{
				int iNew=m_iSelMenu-iPageSize;
				if (iNew<0)
					iNew=0;
				return MAKELONG(iNew, MNC_SELECT);
				}
			
			case VK_NEXT:
				{
				int iNew=m_iSelMenu+iPageSize;
				if (iNew>=GetMenuItemCount(m_hSelMenu))
					iNew=GetMenuItemCount(m_hSelMenu)-1;
				return MAKELONG(iNew, MNC_SELECT);
				}
			}

		}

	bHandled=FALSE;

	HMENU hMenu=(HMENU)lParam;

	UINT iCurrentItem = (UINT)-1; // guaranteed higher than any command ID
	CVector<int> vecItemsMatched;		// items that match the character typed

	TCHAR ch=_totupper((TCHAR)wParam);

	UINT nCount = GetMenuItemCount(hMenu);
	UINT i;
	for (i=0; i<nCount; i++) 
		{
		// get menu info
		MENUITEMINFO mii;
		memset(&mii, 0, sizeof(MENUITEMINFO));
		mii.cbSize = 44;
		mii.fMask = MIIM_DATA | MIIM_TYPE | MIIM_STATE;
		::GetMenuItemInfo(hMenu, i, TRUE, &mii);

		CItemData* pItemData=(CItemData*)mii.dwItemData;
		if ((mii.fType & MFT_OWNERDRAW) && 
				pItemData && 
				pItemData->m_dwSignature==CItemData::Signature && 
				!IsEmptyString(pItemData->m_bstrText)) 
			{
			const wchar_t* pszAmper=wcschr(pItemData->m_bstrText, L'&');
			if (pszAmper && towupper(pszAmper[1])==ch)
				vecItemsMatched.Add(i);
			}

		if (mii.fState & MFS_HILITE)
			iCurrentItem = i; // note index of current item
		}

	// arItemsMatched now contains indexes of items that match the char typed.
	//
	//   * if none: beep
	//   * if one:  execute it
	//   * if more than one: hilite next
	//
	UINT nFound = vecItemsMatched.GetSize();
	if (nFound == 0)
		return 0;
	else if (nFound==1)
		return MAKELONG(vecItemsMatched[0], MNC_EXECUTE);

	// more than one found--return 1st one past current selected item;
	UINT iSelect = 0;
	for (i=0; i < nFound; i++) 
		{
		if (vecItemsMatched[i] > (int)iCurrentItem) 
			{
			iSelect = i;
			break;
			}
		}

	return MAKELONG(vecItemsMatched[iSelect], MNC_SELECT);
}

// WM_SETTINGCHANGE Handler
LRESULT CMenuIcons::OnSettingChange(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled=FALSE;
	m_hFont.Release();
	return 0;
}


CMenuIcons*	CMenuIcons::m_pHookedInstance=NULL;

void CMenuIcons::InstallNavHook()
{
	// Quit if nav keys not enabled...
	if (!m_bEnableNavKeys)	
		return;

	// Quit if already installed
	if (m_hNavHook)
		return;

	// Quit if someone else got it...
	ASSERT(m_pHookedInstance==NULL);
	if (m_pHookedInstance)
		return;

	// Set self as the hooked instance
	m_pHookedInstance=this;
	
	// Setup the hook
	m_hNavHook=SetWindowsHookEx(WH_KEYBOARD, NavHookKeyboardProc, _AtlBaseModule.GetModuleInstance(), NULL);
	if (!m_hNavHook)
		{
		m_pHookedInstance=NULL;
		return;
		}
}


void CMenuIcons::RemoveNavHook()
{
	// Quit if not installed
	if (!m_hNavHook)
		return;

	// Remove it
	UnhookWindowsHookEx(m_hNavHook);
	m_hNavHook=NULL;
	ASSERT(m_pHookedInstance==this);
	m_pHookedInstance=NULL;
}


LRESULT CMenuIcons::OnKeyboardHook(int code, WPARAM wParam, LPARAM lParam)
{
	if (code<0)
		return CallNextHookEx(m_hNavHook, code, wParam, lParam);

	if (code==HC_ACTION)
	{
		if ((lParam & 0x80000000)==0)
			{
			bool bSendIt=false;
			if ((LOWORD(wParam)>='A' && LOWORD(wParam)<='Z') || (LOWORD(wParam)>='0' && LOWORD(wParam)<='9'))
				{
				bSendIt=m_AlphaNavMenus.Get(m_hSelMenu, FALSE)!=0;
				}
			else
				{
				switch (LOWORD(wParam))
					{
					case VK_NEXT:
					case VK_PRIOR:
					case VK_HOME:
					case VK_END:
						bSendIt=true;
						break;
					}
				}

			if (bSendIt)
				{
				ATLTRACE(_T("Keyboard hook: %i\n"), lParam);
				m_wParamHooked=wParam;
				m_lParamHooked=lParam;
				PostThreadMessage(GetCurrentThreadId(), WM_CHAR, ' ', lParam);
				return 1;
				}
			}
	}

		
	return 0;
}

LRESULT CALLBACK CMenuIcons::NavHookKeyboardProc(
		int code,       // hook code
		WPARAM wParam,  // virtual-key code
		LPARAM lParam   // keystroke-message information
		)
{
	ASSERT(m_pHookedInstance!=NULL);
	return m_pHookedInstance->OnKeyboardHook(code, wParam, lParam);
}


void CMenuIcons::SubclassWindow(HWND hWnd)
{
	ASSERT(m_hWnd==NULL);

	// Store window handle
	m_hWnd=hWnd;

	// Store old window procedure
	m_pfnOldWndProc=(WNDPROC)GetWindowLongPtr(m_hWnd, GWLP_WNDPROC);

	// Store new window procedure
	SetWindowLongPtr(m_hWnd, GWLP_WNDPROC, (LONG_PTR)WndProcStub);

	// Store this pointer
	SetProp(hWnd, L"__menuicons_this", (HANDLE)this);
}

void CMenuIcons::UnsubclassWindow()
{
	// Restore window proc
	if (m_hWnd)
	{
		SetWindowLongPtr(m_hWnd, GWLP_WNDPROC, (LONG_PTR)m_pfnOldWndProc);
		RemoveProp(m_hWnd, L"__menuicons_this");
		m_hWnd=NULL;
		m_pfnOldWndProc=NULL;
	}
}

LRESULT CMenuIcons::WndProc(UINT msg, WPARAM wParam, LPARAM lParam)
{
	BOOL bHandled=TRUE;
	LRESULT lRetv;

	switch (msg)
	{
		case WM_DRAWITEM:
			lRetv=OnDrawItem(msg, wParam, lParam, bHandled);
			if (bHandled)
				return lRetv;
			break;

		case WM_MEASUREITEM:
			lRetv=OnMeasureItem(msg, wParam, lParam, bHandled);
			if (bHandled)
				return lRetv;
			break;

		case WM_INITMENUPOPUP:
			lRetv=OnInitMenuPopup(msg, wParam, lParam, bHandled);
			if (bHandled)
				return lRetv;
			break;

		case WM_MENUSELECT:
			lRetv=OnMenuSelect(msg, wParam, lParam, bHandled);
			if (bHandled)
				return lRetv;
			break;

		case WM_MENUCHAR:
			lRetv=OnMenuChar(msg, wParam, lParam, bHandled);
			if (bHandled)
				return lRetv;
			break;

		case WM_SETTINGCHANGE:
			lRetv=OnSettingChange(msg, wParam, lParam, bHandled);
			if (bHandled)
				return lRetv;
			break;

		case WM_DESTROY:
			lRetv=DefWindowProc(msg, wParam, lParam);
			UnsubclassWindow();
			return lRetv;
	}


	return DefWindowProc(msg, wParam, lParam);
}

LRESULT CMenuIcons::DefWindowProc(UINT msg, WPARAM wParam, LPARAM lParam)
{
	return CallWindowProc(m_pfnOldWndProc, m_hWnd, msg, wParam, lParam);
}

LRESULT CMenuIcons::WndProcStub(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	CMenuIcons* pThis=(CMenuIcons*)GetProp(hWnd, L"__menuicons_this");
	ASSERT(pThis!=NULL);
	ASSERT(pThis->m_hWnd==hWnd);

	return pThis->WndProc(msg, wParam, lParam);
}

}	// namespace Simple


