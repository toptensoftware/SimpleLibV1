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
// RegisteredWindow.h - declaration of RegisteredWindow

#ifndef __REGISTEREDWINDOW_H
#define __REGISTEREDWINDOW_H

#ifdef __ATLWIN_H__

#include "WindowImplEx.h"

namespace Simple
{

/*

1.  To create window class, must have includes:

#include <atlbase.h>
extern CComModule _Module;
#include <atlwin.h>

2.  If needed, add

		_Module.Init(NULL, m_hInst);

	and
		
		_Module.Term();

3. Be sure to #include this file

4. Your new window class should look like this:

		// CReshapeControl Class
		class CMyControl : public CRegisteredWindowImpl<CMyControl>
		{
		public:
		// Construction
					CReshapeControl();
			virtual ~CReshapeControl();

		//DECLARE_WND_SUPERCLASS("ReshapeControl", "Edit")
		DECLARE_WND_CLASS("MyControl")

		BEGIN_MSG_MAP(CReshapeControl)
			MESSAGE_HANDLER(WM_LBUTTONDOWN, OnLButtonDown)
		END_MSG_MAP()

		// Attributes

		// Operations
			LRESULT OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
			
		// Implementation
		protected:
		// Attributes

		// Operations

		};

5.  Dont forget to register the class:

		CMyControl::Register();

	and unregister with

		CMyControl::Unregister();

*/



ATOM AtlModuleRegisterGlobalWndClassInfo(_ATL_MODULE* pM, _ATL_WNDCLASSINFOW& p, WNDPROC* pProc);

#define THIS_PTR_PROP	_T("pThis")

template <class T, class TBase = CWindow, class TWinTraits = CControlWinTraits>
class CRegisteredWindowImpl : public CWindowImplEx<T, TBase, TWinTraits>
{
public:
	static LRESULT CALLBACK StartWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		// Get or create pThis ptr...
		T* pThis=(T*)GetProp(hWnd, THIS_PTR_PROP);
		if (!pThis)
			{
			pThis=new T();
			SetProp(hWnd, THIS_PTR_PROP, (HANDLE)pThis);
			pThis->m_hWnd=hWnd;
			}

		LRESULT lRetv=T::WindowProc((HWND)pThis, uMsg, wParam, lParam);

		if (uMsg==WM_NCDESTROY)
			{
			RemoveProp(hWnd, THIS_PTR_PROP);
			pThis->DeleteThis();
			}

		return lRetv;
	}

	static ATOM Register()
	{
		return AtlModuleRegisterGlobalWndClassInfo(_pModule, T::GetWndClassInfo(), NULL);
	}

	static void Unregister()
	{
		UnregisterClass(T::GetWndClassInfo().m_wc.lpszClassName, T::GetWndClassInfo().m_wc.hInstance);
	}
};

template <class T, class TBase = CWindow, class TWinTraits = CControlWinTraits>
class CSuperClassWindowImpl : public CWindowImpl<T, TBase, TWinTraits>
{
public:
	static LRESULT CALLBACK StartWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		// Get or create pThis ptr...
		T* pThis;
		if (uMsg==WM_NCCREATE)
			{
			pThis=new T();
			SetProp(hWnd, THIS_PTR_PROP, (HANDLE)pThis);
			pThis->m_hWnd=hWnd;

			if (T::g_pfnSuperWindowProc)
				pThis->m_pfnSuperWindowProc=T::g_pfnSuperWindowProc;
			}
		else
			{
			pThis=(T*)GetProp(hWnd, THIS_PTR_PROP);
			ATLASSERT(pThis!=NULL);
			}

		LRESULT lRetv=T::WindowProc((HWND)pThis, uMsg, wParam, lParam);

		if (uMsg==WM_NCDESTROY)
			{
			RemoveProp(hWnd, THIS_PTR_PROP);
			delete pThis;
			}

		return lRetv;
	}

	static WNDPROC g_pfnSuperWindowProc;

	static ATOM Register()
	{
		return AtlModuleRegisterGlobalWndClassInfo(_pModule, T::GetWndClassInfo(), &g_pfnSuperWindowProc);
	}

	static void Unregister()
	{
		UnregisterClass(T::GetWndClassInfo().m_wc.lpszClassName, _Module.GetModuleInstance());
	}
};

template <class T, class TBase, class TWinTraits>
WNDPROC CSuperClassWindowImpl<T, TBase, TWinTraits>::g_pfnSuperWindowProc=NULL;

#define START_DECLARE_WND_CLASS(WndClassName) \
static CWndClassInfo& GetWndClassInfo() \
{ \
	static CWndClassInfo wc = \
	{ \
		{ sizeof(WNDCLASSEX), CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS, StartWindowProc, \
		  0, 0, NULL, NULL, NULL, (HBRUSH)(COLOR_WINDOW + 1), NULL, WndClassName, NULL }, \
		NULL, NULL, IDC_ARROW, TRUE, 0, _T("") \
	};

#define END_DECLARE_WND_CLASS() \
	return wc; \
}

}	// namespace Simple

#endif	// __ATLWIN_H__

#endif	// __REGISTEREDWINDOW_H

