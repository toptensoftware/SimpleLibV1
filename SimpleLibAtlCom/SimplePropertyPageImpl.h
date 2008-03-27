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

/////////////////////////////////////////////////////////////////////
// SimplePropertyPageImpl - implementation of CSimplePropertyPageImpl

#ifndef _SIMPLEPROPERTYPAGEIMPL_H
#define _SIMPLEPROPERTYPAGEIMPL_H

#ifdef __ATLCTL_H__

namespace Simple
{

// CSimplePropertyPageImpl
template <class T, class Itf>
class CSimplePropertyPageImpl : 
	public IPropertyPageImpl<T>,
	public CDialogImpl<T>
{
	typedef CSimplePropertyPageImpl<T, Itf>	_CSimplePropertyPageImpl;

public:
// Construction
			CSimplePropertyPageImpl() {};
	virtual ~CSimplePropertyPageImpl() {};

// IPropertyPage
	STDMETHODIMP Activate(HWND hWndParent, LPCRECT pRect, BOOL bModal)
	{
		// Prevent page being marked modified during initialisation
		m_bDirty=TRUE;

		// Do default processing
		HRESULT hr=IPropertyPageImpl<T>::Activate(hWndParent, pRect, bModal);
		if (FAILED(hr))
			return hr;

		// Release old object
		m_spObject.Release();
		m_bMultipleSelection=false;
	
		for (UINT i=0; i<m_nObjects; i++)
			{
			CComQIPtr<Itf> spObject(m_ppUnk[i]);
			if (!spObject)
				{
				static_cast<T*>(this)->GetPageObject(m_ppUnk[i], &spObject);
				}
			if (spObject)
				{
				if (m_spObject)
					{
					// Multiple selection
					m_bMultipleSelection=true;
					m_spObject.Release();
					break;
					}
				else
					{
					// Store it
					m_spObject=spObject;
					}
				}
			}

		// Enable/disable all children
		EnableAllChildren(m_spObject!=NULL);

		// Exchange properties
		if (m_spObject)
			static_cast<T*>(this)->ExchangeObjectProperties(m_spObject, false);

		// Page hasn't been modified yet
		m_bDirty=FALSE;

		// Finished
		return S_OK;
	}

	STDMETHODIMP IsPageDirty(void)
	{
		if (!m_hWnd)
			return S_FALSE;

		return IPropertyPageImpl<T>::IsPageDirty();
	}

	STDMETHODIMP Apply()
	{
		if (m_spObject)
			{
			// Exchange properties
			if (m_spObject)
				if (!static_cast<T*>(this)->ExchangeObjectProperties(m_spObject, true))
					return E_FAIL;
			}

		SetDirty(FALSE);
		return S_OK;
	}

	CComPtr<Itf>	m_spObject;
	bool		m_bMultipleSelection;

	void EnableAllChildren(BOOL bEnable)
	{
		// Enable/disable all controls
		HWND hWnd=GetWindow(GW_CHILD);
		while (hWnd)
			{
			::EnableWindow(hWnd, bEnable);
			hWnd=::GetWindow(hWnd, GW_HWNDNEXT);
			}
	}

	// Default handler for all change notifications send via WM_COMMAND
	LRESULT OnChangeCommand(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
	{
		// Mark page as modified
		SetDirty(TRUE);

		// Don't interfere with other handlers...
		bHandled=FALSE;	

		// Finished
		return 0;
	}

	// Default handler for all change notifications send via WM_COMMAND
	LRESULT OnChangeNotify(int idCtrl, LPNMHDR pnmh, BOOL& bHandled)
	{
		// Mark page as modified
		SetDirty(TRUE);

		// Don't interfere with other handlers...
		bHandled=FALSE;	

		// Finished
		return 0;
	}

	//bool ExchangeObjectProperties(Itf* pObject, bool bSave);
	HRESULT GetPageObject(IUnknown* pUnk, Itf** ppObject)
	{
		return E_NOTIMPL;
	}
};

}	// namespace Simple

#endif	__ATLCTL_H__

#endif _SIMPLEPROPERTYPAGEIMPL_H