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
// WindowImpleEx.h - declaration of CWindowImpleEx class

#ifndef __WINDOWIMPLEEX_H
#define __WINDOWIMPLEEX_H

#ifdef __ATLWIN_H__


// Enhanced version of CWindowImpl that supports:
//		virtual OnDefWindowProc - overrideable def window proc routing
//		DeleteThis() function to delete the class once all messages on call stack finished.

namespace Simple
{

template <class T, class TBase = CWindow, class TWinTraits = CControlWinTraits>
class CWindowImplEx : public CWindowImpl<T, TBase, CControlWinTraits>
{
protected:
	typedef CWindowImpl<T, TBase, CControlWinTraits> _base;
	typedef CWindowImplEx<T, TBase, CControlWinTraits> _this;
public:
	CWindowImplEx()
	{
		m_bSelfDeleted=false;
	}
	virtual ~CWindowImplEx()
	{
	}


	virtual LRESULT OnDefWindowProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		return _base::DefWindowProc(uMsg, wParam, lParam);
	}




	virtual WNDPROC GetWindowProc() 
	{ 
		return WindowProcEx; 
	}

	static LRESULT CALLBACK WindowProcEx(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		// Get this pointer
		_this* pThis = (_this*)hWnd;

		// set a ptr to this message and save the old value
#ifdef VC6
		MSG msg = { pThis->m_hWnd, uMsg, wParam, lParam, 0, { 0, 0 } };
		const MSG* pOldMsg = pThis->m_pCurrentMsg;
#else
		_ATL_MSG msg(pThis->m_hWnd, uMsg, wParam, lParam);
		const _ATL_MSG* pOldMsg = pThis->m_pCurrentMsg;
#endif
		pThis->m_pCurrentMsg = &msg;

		// pass to the message map to process
		LRESULT lRes;
		BOOL bRet = pThis->ProcessWindowMessage(pThis->m_hWnd, uMsg, wParam, lParam, lRes, 0);

		// If window has been destroyed and this is the last message,
		// then delete ourselves.
		if (pThis->m_bSelfDeleted && pOldMsg == NULL)
		{
			delete pThis;
			return FALSE;
		}

		// restore saved value for the current message
		ATLASSERT(pThis->m_pCurrentMsg == &msg);
		pThis->m_pCurrentMsg = pOldMsg;

		// do the default processing if message was not handled
		if(!bRet)
		{
			if(uMsg != WM_NCDESTROY)
				lRes = pThis->DefWindowProc(uMsg, wParam, lParam);
			else
			{
				// unsubclass, if needed
				LONG_PTR pfnWndProc = ::GetWindowLongPtr(pThis->m_hWnd, GWLP_WNDPROC);
				lRes = pThis->DefWindowProc(uMsg, wParam, lParam);
				if(pThis->m_pfnSuperWindowProc != ::DefWindowProc && ::GetWindowLongPtr(pThis->m_hWnd, GWLP_WNDPROC) == pfnWndProc)
					::SetWindowLongPtr(pThis->m_hWnd, GWLP_WNDPROC, (LONG_PTR)pThis->m_pfnSuperWindowProc);
				// clear out window handle
				HWND hWnd = pThis->m_hWnd;
				pThis->m_hWnd = NULL;
				// clean up after window is destroyed
				pThis->OnFinalMessage(hWnd);
			}
		}
		return lRes;
	}

	bool m_bSelfDeleted;
	void DeleteThis()
	{
		// If no outstanding messages to process in call stack,
		// m_pCurrentMsg will be NULL so we can delete ourselves.
		if (m_pCurrentMsg == NULL)
			delete this;
		else
			m_bSelfDeleted=true;
	}

	// Overriden to call DefWindowProc which uses DefFrameProc
	LRESULT DefWindowProc()
	{
		const MSG* pMsg = m_pCurrentMsg;
		LRESULT lRes = 0;
		if (pMsg != NULL)
			lRes = DefWindowProc(pMsg->message, pMsg->wParam, pMsg->lParam);
		return lRes;
	}

	LRESULT DefWindowProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		return OnDefWindowProc(uMsg, wParam, lParam);
	}


};


}	// namespace Simple

#endif	// __ATLWIN_H__

#endif	// __WINDOWIMPLEEX_H

