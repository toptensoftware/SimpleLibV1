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
// MdiWindow.h - declaration of MdiWindow

#ifndef __MDIWINDOW_H
#define __MDIWINDOW_H

#include "WindowImplEx.h"

#ifdef __ATLWIN_H__

namespace Simple
{

#define IDW_MDICLIENT	0xD000
#define ID_MDI_WINDOW0	0xD001

class CMDIClientWindow : public CWindow
{
public:
// Construction
			CMDIClientWindow();
	virtual ~CMDIClientWindow();

// Creation
	bool Create(HWND hWndParent, HMENU hWindowMenu=NULL, UINT idFirstChild=ID_MDI_WINDOW0, UINT idClient=IDW_MDICLIENT);
	bool TranslateAccelerator(MSG* pMsg);

	static HMENU FindWindowMenu(HMENU hMainMenu);

	HMENU MdiSetMenu(HMENU hMenuFrame, HMENU hMenuWindow)
	{
		return (HMENU)SendMessage(WM_MDISETMENU, (WPARAM)hMenuFrame, (LPARAM)hMenuWindow);
	}

	void MdiTile(bool bVertical)
	{
		SendMessage(WM_MDITILE, bVertical ? MDITILE_VERTICAL : MDITILE_HORIZONTAL, 0);
	}
	void MdiCascade()
	{
		SendMessage(WM_MDICASCADE);
	}
	HWND MdiGetActive(BOOL* pbMaximized=NULL)
	{
		return (HWND)SendMessage(WM_MDIGETACTIVE, 0, (LPARAM)pbMaximized);
	}	
	void MdiClose();
	void MdiCloseAll();
};



template <class T>
class CMDIChildWindow : public CWindowImplEx<T>
{
protected:
	typedef CMDIChildWindow<T> _CMDIChildWindow;
	typedef CWindowImpl<T>	_base;

public:
	CMDIChildWindow()
	{
		m_bMaximized=false;
		m_bActive=false;
	}

BEGIN_MSG_MAP(CMDIChildWindow)
	MESSAGE_HANDLER(WM_MDIACTIVATE, OnMdiActivate)
	MESSAGE_HANDLER(WM_SIZE, OnSize)
END_MSG_MAP()

	LRESULT OnMdiActivate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		HWND hWndDeactivate=(HWND)wParam;
		HWND hWndActivate=(HWND)lParam;

		m_bActive=hWndActivate==m_hWnd;

		if (hWndActivate==m_hWnd || (hWndDeactivate==m_hWnd && hWndActivate==NULL))
		{
			::SendMessage(GetParentFrame(), WM_MDIACTIVATE, wParam, lParam);
		}

		if (m_bActive)
		{
			// Send to top level parent frame...
			::SendMessage(GetParentFrame(), WM_MDISETMENU, (WPARAM)m_hMenu.m_hObject, (LPARAM)CMDIClientWindow::FindWindowMenu(m_hMenu));
		}


		bHandled=FALSE;
		return 0;
	}

	LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		if (!m_bInCreateDestroy)
		{
			bool bMaximized=((GetStyle() & WS_MAXIMIZE)!=0);
			m_bMaximizeNew=bMaximized;
			if (bMaximized != m_bMaximized)
			{
				m_bMaximized=bMaximized;
				::PostMessage(GetParentFrame(), WM_MDIMAXIMIZE, NULL, (LPARAM)m_hWnd);
			}
		}
		bHandled=FALSE;
		return 0;
	}

	virtual LRESULT OnDefWindowProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		return DefMDIChildProc(m_hWnd, uMsg, wParam, lParam);
	}

	HWND Create(HWND hWndMdiClient, LPCTSTR pszWindowName, LPARAM lParam=0)
    {
        // We need the class name to fill in the struct,
        // so register it if it hasn't already been registered.
		ATL::CWndClassInfo& wndclass=T::GetWndClassInfo();
        ATOM atom = wndclass.Register(&m_pfnSuperWindowProc);

        // Create a thunk
        _Module.AddCreateWndData(&m_thunk.cd, this);

        MDICREATESTRUCT mdicreate = { 0 };
        mdicreate.szClass = (LPCTSTR)MAKELONG(atom, 0);
        mdicreate.szTitle = pszWindowName;
        mdicreate.hOwner  = _Module.GetResourceInstance();
        mdicreate.x       = CW_USEDEFAULT;
        mdicreate.y       = CW_USEDEFAULT;
        mdicreate.cx      = CW_USEDEFAULT;
        mdicreate.cy      = CW_USEDEFAULT;
        mdicreate.lParam  = lParam;
		mdicreate.style   = WS_CLIPCHILDREN | (m_bMaximizeNew ? WS_MAXIMIZE : 0);

		m_bInCreateDestroy=true;
        HWND hWnd=(HWND)::SendMessage(hWndMdiClient, WM_MDICREATE, 0, (LPARAM)&mdicreate);
		m_bInCreateDestroy=false;
		return hWnd;
    }

	BOOL DestroyWindow()
	{
		m_bInCreateDestroy=true;
		::SendMessage(GetParent(), WM_MDIDESTROY, (WPARAM)m_hWnd, 0);
		m_bInCreateDestroy=false;
		return TRUE;
	}

	BOOL MDIActivate()
	{
		::SendMessage(GetParent(), WM_MDIACTIVATE, (WPARAM)m_hWnd, 0);
		return TRUE;
	}


	HWND GetParentFrame()
	{
		HWND hWnd=m_hWnd;
		while (::GetWindowLong(hWnd, GWL_STYLE) & WS_CHILD)
		{
			hWnd=::GetParent(hWnd);
		}
		return hWnd;
	}

	BOOL LoadMenu(UINT nResID)
	{
		m_hMenu=::LoadMenu(_Module.GetResourceInstance(), MAKEINTRESOURCE(nResID));
		if (m_bActive)
		{
			// Send to top level parent frame...
			::SendMessage(GetParentFrame(), WM_MDISETMENU, (WPARAM)m_hMenu.m_hObject, (LPARAM)CMDIClientWindow::FindWindowMenu(m_hMenu));
		}
		return TRUE;
	}

	bool m_bMaximized;
	bool m_bActive;
	static bool m_bMaximizeNew;
	static bool m_bInCreateDestroy;
	CSmartHandle<HMENU>	m_hMenu;
};

template <class T> bool CMDIChildWindow<T>::m_bMaximizeNew=true;
template <class T> bool CMDIChildWindow<T>::m_bInCreateDestroy=false;

}	// namespace Simple

#endif	// __ATLWIN_H__

#endif	// __MDIWINDOW_H

