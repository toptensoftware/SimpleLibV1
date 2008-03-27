//////////////////////////////////////////////////////////////////////
//
// SimpleLib ATL COM Version 1.0
//
// Copyright (C) 1998-2007 Topten Software.  All Rights Reserved
// http://www.toptensoftware.com
//
// This code has been released for use "as is".  Any redistribution or 
// modification however is strictly prohibited.   See the readme.txt file 
// for complete terms and conditions.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SimpleLibAtlComBuild.h"

#include "SubclassedControl.h"


namespace Simple
{

// Control parent info...
#pragma warning(disable : 4200)		// Shutup about non-standard extension
#pragma pack(1)
struct CONTROLPARENTINFO
{
	WNDPROC		m_pfnOldProc;
	int			m_iNumControls;
	HWND		m_hwndControls[];
};
#pragma pack()
#pragma warning(default : 4200)



// Build property name used to attach CONTROLPARENTINFO to a window
static void BuildPropertyName(LPTSTR pszPropName)
{
	wsprintf(pszPropName, _T("ControlParentInfo:%.8x:%.8x"), 
					_AtlBaseModule.GetModuleInstance(), GetCurrentThreadId());
}



// Get parent info...
static CONTROLPARENTINFO* GetParentInfo(HWND hWndParent)
{
	// Get property name
	TCHAR szPropName[64];
	BuildPropertyName(szPropName);

	// Get control parent info
	return (CONTROLPARENTINFO*)::GetProp(hWndParent, szPropName);
}



// Set parent info...
static void SetParentInfo(HWND hWndParent, CONTROLPARENTINFO* pInfo)
{
	// Get property name
	TCHAR szPropName[64];
	BuildPropertyName(szPropName);

	// Get control parent info
	::SetProp(hWndParent, szPropName, pInfo);
}



// Parent window subclass window procedure
LRESULT CALLBACK ParentSubClassProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	// Get control info structure
	CONTROLPARENTINFO* pInfo=GetParentInfo(hWnd);
	_ASSERTE(pInfo!=NULL);

	HWND hWndChild = NULL;

	switch (msg)
		{
		case WM_NCDESTROY:	
			{
			// Store window address
			WNDPROC pfnOldProc=pInfo->m_pfnOldProc;

			// Release info
			HeapFree(GetProcessHeap(), 0, pInfo);

			// Detach info...
			SetParentInfo(hWnd, NULL);

			// Call default
			return ::CallWindowProc(pfnOldProc, hWnd, msg, wParam, lParam);
			}

		case WM_COMMAND:
			if(lParam != NULL)	// not from a menu
				hWndChild = (HWND)lParam;
			break;

		case WM_NOTIFY:
			hWndChild = ((LPNMHDR)lParam)->hwndFrom;
			break;

		case WM_PARENTNOTIFY:
			switch(LOWORD(wParam))
			{
			case WM_CREATE:
			case WM_DESTROY:
				hWndChild = (HWND)lParam;
				break;
			default:
				hWndChild = ::GetDlgItem(hWnd, HIWORD(wParam));
				break;
			}
			break;

		case WM_DRAWITEM:
			if(wParam)	// not from a menu
				hWndChild = ((LPDRAWITEMSTRUCT)lParam)->hwndItem;
			break;
		case WM_MEASUREITEM:
			if(wParam)	// not from a menu
				hWndChild = ::GetDlgItem(hWnd, ((LPMEASUREITEMSTRUCT)lParam)->CtlID);
			break;
		case WM_COMPAREITEM:
			if(wParam)	// not from a menu
				hWndChild = ::GetDlgItem(hWnd, ((LPCOMPAREITEMSTRUCT)lParam)->CtlID);
			break;
		case WM_DELETEITEM:
			if(wParam)	// not from a menu
				hWndChild = ::GetDlgItem(hWnd, ((LPDELETEITEMSTRUCT)lParam)->CtlID);
			break;
		case WM_VKEYTOITEM:
		case WM_CHARTOITEM:
		case WM_HSCROLL:
		case WM_VSCROLL:
			hWndChild = (HWND)lParam;
			break;
		case WM_CTLCOLORBTN:
		case WM_CTLCOLORDLG:
		case WM_CTLCOLOREDIT:
		case WM_CTLCOLORLISTBOX:
		case WM_CTLCOLORMSGBOX:
		case WM_CTLCOLORSCROLLBAR:
		case WM_CTLCOLORSTATIC:
			hWndChild = (HWND)lParam;
			break;
		}


	if (hWndChild!=NULL)
		{
		// Check valid window handle
		ATLASSERT(::IsWindow(hWndChild));

		// Check this control in our list of child controls
		BOOL bFound=FALSE;
		for (int i=0; i<pInfo->m_iNumControls; i++)
			{
			if (pInfo->m_hwndControls[i]==hWndChild)
				{
				bFound=TRUE;
				break;
				}
			}

		// Send message to control
		if (bFound)
			return ::SendMessage(hWndChild, OCM__BASE + msg, wParam, lParam);
		}

	// Call default
	return ::CallWindowProc(pInfo->m_pfnOldProc, hWnd, msg, wParam, lParam);
}



// Subclass parent window of control to reflect messages (easier than
// creating a parking window!)
void SubclassedControlSubclassParent(HWND hWndParent, HWND hWndControl)
{
	// Get control parent info
	CONTROLPARENTINFO* pInfo=GetParentInfo(hWndParent);
	if (!pInfo)
		{
		// No info, create it and subclass parent...

		// Allocate new info structure
		int iSize=sizeof(CONTROLPARENTINFO)+sizeof(HWND);
		pInfo=(CONTROLPARENTINFO*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, iSize);
		memset(pInfo, 0, iSize);

		// Initialise vector size to 1...
		pInfo->m_iNumControls=1;

		// Sub class the window
		pInfo->m_pfnOldProc=(WNDPROC)GetWindowLongPtr(hWndParent, GWLP_WNDPROC);
		SetWindowLongPtr(hWndParent, GWLP_WNDPROC, (LRESULT)ParentSubClassProc);

		// Attach info...
		SetParentInfo(hWndParent, pInfo);
		}

	_ASSERTE(pInfo!=NULL);

	// Look for an available slot
	for (int i=0; i<pInfo->m_iNumControls; i++)
		{
		if (pInfo->m_hwndControls[i]==NULL)
			{
			// Found empty slot, claim it and finished...
			pInfo->m_hwndControls[i]=hWndControl;
			return;
			}
		}

	// Make room for more window handles
	int iNewControlCount=pInfo->m_iNumControls+1;
	int iNewSize=sizeof(CONTROLPARENTINFO)+(sizeof(HWND)*iNewControlCount);
	pInfo=(CONTROLPARENTINFO*)HeapReAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, pInfo, iNewSize);
	_ASSERTE(pInfo!=NULL);		// Lazy, lazy, lazy...

	// Re-attach info...
	SetParentInfo(hWndParent, pInfo);

	// Use newly allocated slot...
	pInfo->m_hwndControls[pInfo->m_iNumControls]=hWndControl;

	// Update control count
	pInfo->m_iNumControls=iNewControlCount;
}



// Unsubclass parent window
void SubclassedControlUnsubclassParent(HWND hWndParent, HWND hWndControl)
{
	// Get control parent info
	CONTROLPARENTINFO* pInfo=GetParentInfo(hWndParent);
	_ASSERTE(pInfo!=NULL);

	// Find entry...
	for (int i=0; i<pInfo->m_iNumControls; i++)
		{
		if (pInfo->m_hwndControls[i]==hWndControl)
			{
			// Free up this slot...
			pInfo->m_hwndControls[i]=NULL;
			return;
			}
		}

	// Oops...
	_ASSERTE(FALSE);
}


}	// Simple