//////////////////////////////////////////////////////////////////////////
// ToolBarCtrlEx.cpp - implementation of CToolBarCtrlEx class

#include "stdafx.h"
#include "SimpleLibAtlWinBuild.h"
#include "ToolBarCtrlEx.h"

namespace Simple
{

// structure of RT_TOOLBAR resource
#define RT_TOOLBAR  MAKEINTRESOURCE(241)
struct TOOLBARDATA {
	WORD wVersion;		// version # should be 1
	WORD wWidth;		// width of one bitmap
	WORD wHeight;		// height of one bitmap
	WORD wItemCount;	// number of items
	WORD items[1];		// array of command IDs, actual size is wItemCount
};

BOOL LoadToolBar(HWND hWndToolBar, HINSTANCE hInst, LPCTSTR pszResource, COLORREF rgbTransparent, HIMAGELIST* phImageList)
{
	return LoadToolBar(hWndToolBar, hInst, pszResource, NULL, rgbTransparent, phImageList);
}

// Load a toolbar resource...
BOOL LoadToolBar(HWND hWndToolBar, HINSTANCE hInst, LPCTSTR pszResource, LPCTSTR pszResource32, COLORREF rgbTransparent, HIMAGELIST* phImageList)
{
USES_CONVERSION;

	ASSERT(*phImageList==NULL);

	// Load resource
	HRSRC hRsrc;
	TOOLBARDATA* ptbd;
	if ((hRsrc= FindResource(hInst, pszResource, RT_TOOLBAR))== NULL ||
		 (ptbd = (TOOLBARDATA*)LoadResource(hInst, hRsrc))== NULL) 
		{
		return FALSE;
		}
	ASSERT(ptbd->wVersion==1);


	// Load image list
	phImageList[0]=ImageList_LoadEx(hInst, pszResource, pszResource32, rgbTransparent, ptbd->wWidth);

	// Give image list to toolbar
	SendMessage(hWndToolBar, TB_SETIMAGELIST, 0, (LPARAM)phImageList[0]);

	// Add all buttons...
	int iIconIndex=0;
	for (WORD i=0; i<ptbd->wItemCount; i++)
		{
		// Separator?
		if (ptbd->items[i]<=0)
			{
			TBBUTTON button;
			button.iBitmap=0;
			button.idCommand=0;
			button.fsState=TBSTATE_ENABLED;
			button.fsStyle=BTNS_SEP;
			button.dwData=0;
			button.iString=0;
			SendMessage(hWndToolBar, TB_ADDBUTTONS, 1, (LPARAM)&button);
			}
		else
			{
			// Setup strings...
			OLECHAR szString[MAX_PATH*2];
			CUniString strDescription, strToolTip;
			if (LoadString(hInst, ptbd->items[i], szString, MAX_PATH*2))
				{
				SplitString(szString, L"\n", strDescription, strToolTip);
				}

			TBBUTTON button;
			button.iBitmap=iIconIndex++;
			button.idCommand=ptbd->items[i];
			button.fsState=TBSTATE_ENABLED;
			button.fsStyle=BTNS_BUTTON;
			button.dwData=0;
			button.iString=(INT_PTR)static_cast<const wchar_t*>(strToolTip);
			SendMessage(hWndToolBar, TB_ADDBUTTONS, 1, (LPARAM)&button);
			}

		}

	return TRUE;
}

void SetToolBarButtonLabel(HWND hWndToolBar, UINT nID, LPCTSTR pszLabel)
{
	ATLControls::CToolBarCtrl tb(hWndToolBar);

	// Make sure list style toolbar
	if ((tb.GetStyle() & TBSTYLE_LIST)==0)
		tb.ModifyStyle(0, TBSTYLE_LIST);

	// Make sure mixed buttons...
	DWORD dwExtStyle=(DWORD)tb.SendMessage(TB_GETEXTENDEDSTYLE);
	if ((dwExtStyle & TBSTYLE_EX_MIXEDBUTTONS)==0)
		tb.SendMessage(TB_SETEXTENDEDSTYLE, 0, dwExtStyle | TBSTYLE_EX_MIXEDBUTTONS);

	TCHAR szTemp[MAX_PATH];
	
	// Set button text
	TBBUTTONINFO info;
	memset(&info, 0, sizeof(info));
	info.cbSize=sizeof(info);
	info.dwMask=TBIF_STATE|TBIF_STYLE|TBIF_TEXT;
	info.pszText=szTemp;
	info.cchText=MAX_PATH;
	tb.GetButtonInfo(nID, &info);

	if (lstrcmpi(szTemp, pszLabel)==0)
		return;

	// Change text and style
	//info.pszText=(char*)tb.SendMessage(TB_ADDSTRING, 0, (LPARAM)_T("New"));
	info.dwMask=TBIF_STATE|TBIF_STYLE|TBIF_TEXT;
	info.pszText=const_cast<LPTSTR>(pszLabel);
	info.cchText=0;
	info.fsStyle |= (BTNS_SHOWTEXT|BTNS_AUTOSIZE);
	tb.SetButtonInfo(nID, &info);
}


}	// namespace Simple
