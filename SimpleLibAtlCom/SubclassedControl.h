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

//////////////////////////////////////////////////////////////////////////
// SubclassedControl.h - declaration of SubclassedControl

#ifndef __SUBCLASSEDCONTROL_H
#define __SUBCLASSEDCONTROL_H

namespace Simple
{

/*

About CSubclassedControl
------------------------

The CSubclassedControl class, helps create ActiveX controls based on an already
existing window class (eg: button, static, edit etc...).  It works by creating
the window to be subclassed as soon as the the ActiveX part of the control is
instantiated.  The control is created hidden as a child of the desktop window 
and is reparented to the container's window and shown when the ActiveX control
is activated.  Similarly, when the control is de-activated, it's reparented back
to the desktop.  Using this mechanism, the control window (m_hWndControl) is 
always available.

To trap messages sent from the control to the parent (eg: WM_COMMAND/WM_NOTIFY
etc...) CSubclassedControl will use MessageReflection if supported by the 
container.  Otherwise, the CSubclassedControl subclassed the parent window
traps any notification type messages from the control and reflects them back
to the control.  As long as you use OCM_COMMAND, OCM_NOTIFY etc.. to handle
reflected messages, they should work in all containers.



How to use CSubclassedControl
-----------------------------

1. Create ActiveX control using ATL wizards as per normal.

2. Replace inheritance from CComObjectRootEx<ThreadModel> and CComControl<CYourClass>
	with CSubclassedControl<CYourClass, ThreadModel>.

3. Add initialisation to constructor to specify class name of control to be 
	subclassed.

	Eg:
		CYourClass()
			: _CSubclassedControl("edit", ES_MULTILINE)
		{
		}

	Alternatively, override CreateWindowToSubclass to create the window

4. (Optional) Override OnControlCreated to initialise the control

5. Add (or update) the following line to end of message map
	
		CHAIN_MSG_MAP(_CSubclassedControl)

6. Access the subclassed window at any time using the m_hWndControl method. 
	(This means you can't use the window methods in CWindow to talk to the 
	control - sorry :-( ).  Eg: SetWindowText(_T("Hello")) needs to be 
	replaced with ::SetWindowText(m_hWndControl, _T("Hello")).  Eventually
	I'll get around to adding similar methods to CSubclassedControl but 
	haven't got around to it yet.


*/

#ifdef __ATLCTL_H__

// Helper functions (see SubclassedControl.cpp for implementation)
void SIMPLEAPI SubclassedControlSubclassParent(HWND hWndParent, HWND hWndControl);
void SIMPLEAPI SubclassedControlUnsubclassParent(HWND hWndParent, HWND hWndControl);

// Subclassed com control class
template <class T, class ThreadModel>
class CSubclassedControl : 
	public CComObjectRootEx<ThreadModel>,
	public CComControl<T>
{
public:
	typedef CSubclassedControl<T, ThreadModel> _CSubclassedControl;

	// Constructor
	CSubclassedControl(LPCTSTR pszClassName=NULL, DWORD dwStyle=0, DWORD dwExStyle=0)
	{
		m_hWndControl=NULL;
		m_bWindowOnly = TRUE;

		// Store window creation attributes
		m_pszClassName=pszClassName;
		m_dwStyle=(dwStyle|WS_CHILD|WS_CLIPSIBLINGS) & ~WS_VISIBLE; 
		m_dwExStyle=dwExStyle;
	}

	// Destructor
	virtual ~CSubclassedControl()
	{
		// Unsubclass it if still subclassed 
		if (m_hWnd!=NULL)
			UnsubclassWindow();

		// Destroy control window
		if (m_hWndControl!=NULL)
			::DestroyWindow(m_hWndControl);

		// Make sure control window handle set to NULL so ATL doesn't try to
		// destroy the control window also!
		m_hWndCD=NULL;
	}

	// Create the control window
	HRESULT FinalConstruct()
	{
		_ASSERTE(m_hWndControl==NULL);

		// Call default final construct
		HRESULT hr=CComObjectRootEx<ThreadModel>::FinalConstruct();
		if (FAILED(hr)) return hr;

		// Create the control window
		m_hWndControl=CreateWindowToSubclass();

		// Check window created OK
		if (m_hWndControl==NULL)
			{
			ATLTRACE(_T("FinalConstruct: Failed to create window to subclass\n"));
			return E_FAIL;
			}

		// Tell derived class control has been created
		if (!OnControlCreated())
			{
			::DestroyWindow(m_hWndControl);
			m_hWndControl=NULL;
			return E_FAIL;
			}



		// Done
		return S_OK;
	}

	virtual BOOL OnControlCreated()
	{
		return TRUE;
	}

	virtual HWND CreateWindowToSubclass()
	{
		return ::CreateWindowEx(m_dwExStyle, m_pszClassName, NULL, 
						m_dwStyle, 0, 0, 0, 0, ::GetDesktopWindow(), 0, 
						_Module.GetModuleInstance(), NULL);
	}

// Message map
BEGIN_MSG_MAP(_CSubclassedControl)
	MESSAGE_HANDLER(WM_PAINT, OnPaint)
	CHAIN_MSG_MAP(CComControl<T>)
END_MSG_MAP()

	// WM_PAINT handler
	LRESULT OnPaint(UINT nMsg , WPARAM wParam , LPARAM lParam , BOOL& bHandled)
	{
		// Pass WM_PAINT to subclassed control, rather than routing through ATL
		bHandled=TRUE;
		return DefWindowProc(nMsg, wParam, lParam);
	}

	// Override set/kill focus to not set the bHandled flag so original control
	// also sees messages
	LRESULT OnSetFocus(UINT nMsg , WPARAM wParam , LPARAM lParam , BOOL& bHandled)
	{
		bHandled=FALSE;
		return CComControl<T>::OnSetFocus(nMsg, wParam, lParam, bHandled);
	}

	LRESULT OnKillFocus(UINT nMsg , WPARAM wParam , LPARAM lParam , BOOL& bHandled)
	{
		bHandled=FALSE;
		return CComControl<T>::OnKillFocus(nMsg, wParam, lParam, bHandled);
	}

public:
	HWND	m_hWndControl;
	LPCTSTR m_pszClassName;
	DWORD	m_dwStyle;
	DWORD	m_dwExStyle;

	// Create the control window
	virtual HWND CreateControlWindow(HWND hWndParent, RECT& rc)
	{
		// Check have the control already
		_ASSERTE(m_hWndControl!=NULL);
//		_ASSERTE(!::IsWindowVisible(m_hWndControl));

		// Does container support message reflection?
		BOOL bMessageReflect=FALSE;
		if (FAILED(GetAmbientMessageReflect(bMessageReflect)))
			bMessageReflect=FALSE;

		if (!bMessageReflect)
			{
			SubclassedControlSubclassParent(hWndParent, m_hWndControl);
			}

		// Reparent the control
		if (!::SetParent(m_hWndControl, hWndParent))
			return NULL;

		// Reposition it...
		::SetWindowPos(m_hWndControl, NULL, 
				rc.left, rc.top, rc.right-rc.left, rc.bottom-rc.top,
				SWP_NOZORDER|SWP_SHOWWINDOW);

		// Subclass it
		SubclassWindow(m_hWndControl);

		// Done
		return m_hWndControl;
	}

	// Destroy the control window
	virtual void DestroyControlWindow()
	{
		// Sanity checks
		_ASSERTE(m_hWndCD==m_hWnd);
		_ASSERTE(m_hWndCD==m_hWndControl);

		// Hide the window
		::ShowWindow(m_hWndControl, SW_HIDE);

		// Does container support message reflection?
		BOOL bMessageReflect=FALSE;
		if (FAILED(GetAmbientMessageReflect(bMessageReflect)))
			bMessageReflect=FALSE;

		if (!bMessageReflect)
			{
			SubclassedControlUnsubclassParent(::GetParent(m_hWndControl), m_hWndControl);
			}

		// Unsubclass it
		UnsubclassWindow();

		// Reparent it back to desktop
		::SetParent(m_hWndControl, ::GetDesktopWindow());

		// Clear window handles
		m_hWndCD=NULL;

		// Done
	}
	
	// Template overrides
	HRESULT IOleObject_Close(DWORD dwSaveOption)
	{
		// Make sure the control window is destroyed before letting
		// ATL get at it...
		CComPtr<IOleInPlaceObject> pIPO;
		ControlQueryInterface(IID_IOleInPlaceObject, (void**)&pIPO);
		_ASSERTE(pIPO != NULL);
		if (m_hWndCD)
		{
			if (m_spClientSite)
				m_spClientSite->OnShowWindow(FALSE);
		}

		if (m_bInPlaceActive)
		{
			HRESULT hr = pIPO->InPlaceDeactivate();
			if (FAILED(hr))
				return hr;
			_ASSERTE(!m_bInPlaceActive);
		}
		if (m_hWndCD)
		{
			ATLTRACE(_T("Destroying Window\n"));
			DestroyControlWindow();
			_ASSERTE(m_hWndCD==NULL);
		}

		return CComControl<T>::IOleObject_Close(dwSaveOption);
	}

	// IOleInPlaceObject::InPlaceDeactivate
	HRESULT IOleInPlaceObject_InPlaceDeactivate(void)
	{
		CComPtr<IOleInPlaceObject> pIPO;
		ControlQueryInterface(IID_IOleInPlaceObject, (void**)&pIPO);
		_ASSERTE(pIPO != NULL);

		if (!m_bInPlaceActive)
			return S_OK;

		pIPO->UIDeactivate();

		m_bInPlaceActive = FALSE;

		// if we have a window, tell it to go away.
		//
		if (m_hWndCD)
		{
			ATLTRACE(_T("Destroying Window\n"));
			DestroyControlWindow();
			_ASSERTE(m_hWndCD==NULL);
		}

		if (m_spInPlaceSite)
			m_spInPlaceSite->OnInPlaceDeactivate();

		return S_OK;
	}
};

#endif		// __ATLCTL_H__

}	// namespace Simple

#endif	// __SUBCLASSEDCONTROL_H

