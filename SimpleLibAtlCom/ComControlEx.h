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
// ComControlEx.h - declaration of CommandLineParser

#ifndef __COMCONTROLEX_H
#define __COMCONTROLEX_H

#ifdef __ATLCTL_H__

namespace Simple
{

inline HWND SIMPLEAPI _AtlGetTopLevelWindow(HWND hWnd)
{
	HWND hWndTop;

	do
	{
		hWndTop = hWnd;
		hWnd = ::GetParent(hWnd);
	}
	while (hWnd != NULL);

	return hWndTop;
}


///////////////////////////////////////////////////////////////////////////////////
// this class causes the owning container of a control go inactive whilst a dialog box is open
//
// USAGE:
//		CYourDialog dlg;
//		DOMODAL(this);
//		if (dlg.DoModal()==IDOK) ...
//
struct ModalDialogController
{
	CComPtr<IOleInPlaceFrame> m_spInPlaceFrame;

	ModalDialogController(IOleInPlaceSite* pInPlaceSite) 
	{
		CComPtr<IOleInPlaceUIWindow> spInPlaceUIWindow;
		OLEINPLACEFRAMEINFO frameInfo;
		frameInfo.cb = sizeof(OLEINPLACEFRAMEINFO);
		RECT rcPos, rcClip;

		HWND hwndParent; 
		// This call to GetWindow is a fix for Delphi
		if (pInPlaceSite->GetWindow(&hwndParent) == S_OK)
		{
			pInPlaceSite->GetWindowContext(&m_spInPlaceFrame, &spInPlaceUIWindow, &rcPos, &rcClip, &frameInfo);
		}

		PreModalDialog();
	}

	~ModalDialogController()
	{
		PostModalDialog();
	}

	void PreModalDialog()
	{
		if (m_spInPlaceFrame!= NULL)
		{
			m_spInPlaceFrame->EnableModeless(FALSE);
		}
		else
		{
			HWND hWndTop = _AtlGetTopLevelWindow(NULL);
			if (hWndTop != NULL)
				::EnableWindow(hWndTop, FALSE);
		}
	}
	void PostModalDialog()
	{
		if (m_spInPlaceFrame!= NULL)
		{
			m_spInPlaceFrame->EnableModeless(TRUE);
		}
		else
		{
			HWND hWndTop = _AtlGetTopLevelWindow(NULL);
			if (hWndTop != NULL)
				::EnableWindow(hWndTop, TRUE);
		}
	}
};

#define DOMODAL(ctl) ModalDialogController xcontroller(ctl->m_spInPlaceSite);


// CComControlEx control class
template <
			class T, 
			class ThreadModel, 
			class WinBase=CWindowImpl<T> 
			>
class CComControlEx : 
	public CComObjectRootEx<ThreadModel>,
	public CComControlBase,
	public WinBase,
	public IViewObjectExImpl<T>,
	public IDataObjectImpl<T>
{
public:
	typedef CComControlEx<T, ThreadModel, WinBase> _CComControlEx;

	// Constructor
	CComControlEx() : 
		CComControlBase(m_hWndActive)
	{
		m_hWnd=NULL;
		m_hWndActive=NULL;
		m_bWindowOnly = TRUE;
	}

	// Create the control window
	HRESULT FinalConstruct()
	{
		_ASSERTE(m_hWnd==NULL);

		// Call default final construct
		HRESULT hr=CComObjectRootEx<ThreadModel>::FinalConstruct();
		if (FAILED(hr)) return hr;

		// Create the control window
		RECT rc;
		SetRectEmpty(&rc);

		T* pT = static_cast<T*>(this);
		pT->Create(GetDesktopWindow(), rc);
		pT->ShowWindow(SW_HIDE);

		// Check window created OK
		if (m_hWnd==NULL)
			{
			ATLTRACE(_T("FinalConstruct: Failed to create window to subclass\n"));
			return E_FAIL;
			}

		// Done
		return S_OK;
	}

	void FinalRelease()
	{
		if (m_hWnd)
			{
			DestroyWindow();
			m_hWnd=NULL;
			}
	}

// Interface map
BEGIN_COM_MAP(_CComControlEx)
	COM_INTERFACE_ENTRY(IViewObjectEx)
	COM_INTERFACE_ENTRY(IViewObject2)
	COM_INTERFACE_ENTRY(IViewObject)
	COM_INTERFACE_ENTRY(IDataObject)
END_COM_MAP()

// Message map
BEGIN_MSG_MAP(_CComControlEx)
	MESSAGE_HANDLER(WM_PAINT, CComControlBase::OnPaint)
	MESSAGE_HANDLER(WM_SETFOCUS, CComControlBase::OnSetFocus)
	MESSAGE_HANDLER(WM_KILLFOCUS, CComControlBase::OnKillFocus)
	MESSAGE_HANDLER(WM_MOUSEACTIVATE, CComControlBase::OnMouseActivate)
END_MSG_MAP()

	HWND	m_hWndActive;

public:
	// Create the control window
	virtual HWND CreateControlWindow(HWND hWndParent, RECT& rc)
	{
		// Reparent the control
		if (!::SetParent(m_hWnd, hWndParent))
			return NULL;

		// Reposition it...
		::SetWindowPos(m_hWnd, NULL, 
				rc.left, rc.top, rc.right-rc.left, rc.bottom-rc.top,
				SWP_NOZORDER|SWP_SHOWWINDOW);

		// Store active window handle
		m_hWndActive=m_hWnd;

		// Done
		return m_hWnd;
	}

	virtual HRESULT ControlQueryInterface(const IID& iid, void** ppv)
	{
		T* pT = static_cast<T*>(this);
		return pT->_InternalQueryInterface(iid, ppv);
	}

	// Destroy the control window
	virtual void DestroyControlWindow()
	{
		// Sanity checks
		_ASSERTE(m_hWndCD==m_hWnd);

		// Clear active window handle
		m_hWndActive=NULL;

		// Hide the window
		::ShowWindow(m_hWnd, SW_HIDE);

		// Reparent it back to desktop
		::SetParent(m_hWnd, ::GetDesktopWindow());

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

		return CComControlBase::IOleObject_Close(dwSaveOption);
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


// IViewObject
	STDMETHODIMP Draw(DWORD dwDrawAspect, LONG lindex, void* pvAspect, DVTARGETDEVICE* ptd, HDC hdcTargetDev, HDC hdcDraw, LPCRECTL lprcBounds, LPCRECTL lprcWBounds, BOOL (__stdcall *pfnContinue)(DWORD dwContinue), DWORD dwContinue)
	{
		int iTech=GetDeviceCaps(hdcDraw, TECHNOLOGY);
		if (iTech!=DT_RASDISPLAY && iTech!=DT_METAFILE)
			{
			// Create an enhanced metafile
			CComQIPtr<IDataObject> spDataObject(this);
			FORMATETC fetc;
			fetc.cfFormat=CF_ENHMETAFILE;
			fetc.lindex=lindex;
			fetc.ptd=ptd;
			fetc.tymed=TYMED_ENHMF;
			fetc.dwAspect=dwDrawAspect;
			STGMEDIUM stgm;
			RETURNIFFAILED(spDataObject->GetData(&fetc, &stgm));

			// Play it
			RECT rc={lprcBounds->left, lprcBounds->top, lprcBounds->right, lprcBounds->bottom};
			PlayEnhMetaFile(hdcDraw, stgm.hEnhMetaFile, &rc);

			ReleaseStgMedium(&stgm);
			return S_OK;
			}
		else
			{
			return IViewObjectExImpl<T>::Draw(dwDrawAspect, lindex, pvAspect, ptd, hdcTargetDev, hdcDraw, lprcBounds, lprcWBounds, pfnContinue, dwContinue);
			}
	};


// IDataObject
	STDMETHODIMP GetData(FORMATETC* pformatetcIn, STGMEDIUM* pmedium)
	{
		// Enhanced metafile?
		if (pformatetcIn->cfFormat==CF_ENHMETAFILE)
			{
			T* pT = static_cast<T*>(this);

			// Check format and stgmedium
			if (pformatetcIn->dwAspect!=DVASPECT_CONTENT || pformatetcIn->lindex!=-1)
				return DATA_E_FORMATETC;

			// Create target device context
			HDC hdcTarget=AtlCreateTargetDC(NULL, pformatetcIn->ptd);

			// Create meta file
			RECT rcBounds;
			rcBounds.left=0;
			rcBounds.top=0;
			rcBounds.right=pT->m_sizeExtent.cx;//*94/100;
			rcBounds.bottom=pT->m_sizeExtent.cy;//*94/100;
			HDC hdcMetaFile=CreateEnhMetaFile(hdcTarget, NULL, NULL, NULL);

			SelectObject(hdcMetaFile, GetStockObject(SYSTEM_FONT));

			SetMapMode(hdcMetaFile, MM_TEXT);

			SIZEL sizePixels;
			AtlHiMetricToPixel(&pT->m_sizeExtent, &sizePixels);

			RECTL rclBounds = { 0, 0, sizePixels.cx, sizePixels.cy };

			ATL_DRAWINFO di;
			memset(&di, 0, sizeof(di));
			di.cbSize=sizeof(ATL_DRAWINFO);
			di.dwDrawAspect=pformatetcIn->dwAspect;
			di.lindex=pformatetcIn->lindex;
			di.ptd=pformatetcIn->ptd;
			di.hicTargetDev=hdcTarget;
			di.hdcDraw=hdcMetaFile;
			di.prcBounds=&rclBounds;
			di.prcWBounds=NULL;
			di.bOptimize=FALSE;
			di.bZoomed=FALSE;
			di.bRectInHimetric=FALSE;

			pT->OnDrawAdvanced(di);

			// Close meta file
			memset(pmedium, 0, sizeof(STGMEDIUM));
			pmedium->hEnhMetaFile=CloseEnhMetaFile(hdcMetaFile);
			pmedium->tymed=TYMED_ENHMF;

			// Close target device context
			DeleteDC(hdcTarget);

			// Done
			return S_OK;
			}


		if(pformatetcIn->cfFormat & CF_BITMAP)
			{

			T* pT = static_cast<T*>(this);

			HDC hdc = pT->GetDC();
			
			RECT rcBounds;
			rcBounds.left=0;
			rcBounds.top=0;
			rcBounds.right=m_sizeExtent.cx;//*94/100;
			rcBounds.bottom= m_sizeExtent.cy;//*94/100;
			
			SIZEL sizePixels;
			AtlHiMetricToPixel(&m_sizeExtent, &sizePixels);

			HDC	hdcTarget = CreateCompatibleDC(hdc);
			HBITMAP hBitmap = CreateCompatibleBitmap(hdc, sizePixels.cx, sizePixels.cy);
			SIZE sz;
			SetBitmapDimensionEx(hBitmap, sizePixels.cx, sizePixels.cy, &sz);
			GetBitmapDimensionEx(hBitmap, &sz);

			HBITMAP hOld = (HBITMAP)SelectObject(hdcTarget, hBitmap);
			
			
			
			RECTL rclBounds = { 0, 0, sizePixels.cx, sizePixels.cy };

			ATL_DRAWINFO di;
			memset(&di, 0, sizeof(di));
			di.cbSize=sizeof(ATL_DRAWINFO);
			di.dwDrawAspect=pformatetcIn->dwAspect;
			di.lindex=pformatetcIn->lindex;
			di.ptd=pformatetcIn->ptd;
			di.hdcDraw=hdcTarget;
			di.prcBounds=&rclBounds;
			di.prcWBounds=NULL;
			di.bOptimize=FALSE;
			di.bZoomed=FALSE;
			di.bRectInHimetric=FALSE;

			

			pT->OnDrawAdvanced(di);
			memset(pmedium, 0, sizeof(pmedium));
			pmedium->tymed = TYMED_GDI;
			pmedium->pUnkForRelease = NULL;
  			pmedium->hBitmap = hBitmap;

			SelectObject(hdcTarget, hOld);
			DeleteDC(hdcTarget);

			return S_OK;
			}
	
		// Do default
		return IDataObjectImpl<T>::GetData(pformatetcIn, pmedium);
	};

	STDMETHODIMP QueryGetData(FORMATETC* pformatetc)
	{
		// Enhanced metafile?
		if ( (pformatetc->cfFormat==CF_ENHMETAFILE || pformatetc->cfFormat==CF_BITMAP) && 
			pformatetc->dwAspect==DVASPECT_CONTENT &&
			pformatetc->lindex==-1)
			{
			return S_OK;
			}

		return IDataObjectImpl<T>::QueryGetData(pformatetc);
	};


	HRESULT FireOnRequestEdit(DISPID dispID)
	{
		T* pT = static_cast<T*>(this);
		return T::__ATL_PROP_NOTIFY_EVENT_CLASS::FireOnRequestEdit(pT->GetUnknown(), dispID);
	}
	HRESULT FireOnChanged(DISPID dispID)
	{
		T* pT = static_cast<T*>(this);
		return T::__ATL_PROP_NOTIFY_EVENT_CLASS::FireOnChanged(pT->GetUnknown(), dispID);
	}

};


// CComFullControlEx control class
template <
			class T, 
			class ThreadModel, 
			const CLSID* pclsid, 
			const IID* psrcid, 
			const GUID* plibid, 
			WORD wMajor, 
			WORD wMinor, 
			class WinBase=CWindowImpl<T> 
			>
class CComFullControlEx : 
	public CComControlEx<T, ThreadModel, WinBase>,
	public CComCoClass<T, pclsid>,
	public IOleControlImpl<T>,
	public IOleObjectImpl<T>,
	public IOleInPlaceActiveObjectImpl<T>,
	public IOleInPlaceObjectWindowlessImpl<T>,
	public IQuickActivateImpl<T>,
	public IConnectionPointContainerImpl<T>,
#ifndef NO_COMCONTROL_PERSIST_SUPPORT
#ifdef COMARCHIVE_SUPPORT
	public IPersistStreamInitComArchiveImpl<T>,
	public CComArchivable,
#else
	public IPersistStreamInitImpl<T>,
#endif
	public IPersistStorageImpl<T>,
#endif // NO_COMCONTROL_PERSIST_SUPPORT
	public ISpecifyPropertyPagesImpl<T>,
	public IProvideClassInfo2Impl<pclsid, psrcid, plibid, wMajor, wMinor>,
	public IPropertyNotifySinkCP<T>
{
public:
	typedef CComFullControlEx<T, ThreadModel, pclsid, psrcid, plibid, wMajor, wMinor, WinBase> _CComFullControlEx;

	// Constructor
	CComFullControlEx()
	{
		m_dwControlInfoFlags = 0;
	}

// Interface map
BEGIN_COM_MAP(_CComFullControlEx)
	COM_INTERFACE_ENTRY(IOleInPlaceObject)
	COM_INTERFACE_ENTRY2(IOleWindow, IOleInPlaceObject)
	COM_INTERFACE_ENTRY(IOleInPlaceActiveObject)
	COM_INTERFACE_ENTRY(IOleControl)
	COM_INTERFACE_ENTRY(IOleObject)
	COM_INTERFACE_ENTRY(IQuickActivate)
	COM_INTERFACE_ENTRY(IConnectionPointContainer)
#ifndef NO_COMCONTROL_PERSIST_SUPPORT
	COM_INTERFACE_ENTRY(IPersistStreamInit)
	COM_INTERFACE_ENTRY2(IPersist, IPersistStreamInit)
	COM_INTERFACE_ENTRY(IPersistStorage)
#endif // #ifndef NO_COMCONTROL_PERSIST_SUPPORT
	COM_INTERFACE_ENTRY(ISpecifyPropertyPages)
	COM_INTERFACE_ENTRY(IProvideClassInfo)
	COM_INTERFACE_ENTRY(IProvideClassInfo2)
	COM_INTERFACE_ENTRY_CHAIN(_CComControlEx)
END_COM_MAP()

// Message map
/*
BEGIN_MSG_MAP(_CComFullControlEx)
	CHAIN_MSG_MAP(_CComControlEx)
END_MSG_MAP()
*/

	DWORD	m_dwControlInfoFlags;

public:

// IOleControl
	STDMETHODIMP GetControlInfo(LPCONTROLINFO pCI)
	{
		pCI->hAccel=NULL;
		pCI->cAccel=0;
		pCI->dwFlags=m_dwControlInfoFlags;
		return S_OK;
	}

	void OnChanged(bool bSetDirty=true, DISPID dispid=DISPID_UNKNOWN, bool bInvalidate=true)
	{
		// Fire property change
		if (dispid!=DISPID_UNKNOWN)
			FireOnChanged(dispid);		

		// Set dirty flag?
		if (bSetDirty)
			SetDirty(TRUE);

		// Send other change notifications
		FireViewChange();		// NB: This will do an invalidate on the whole control!
		SendOnDataChange();
	}

	void OnFontChanged(DISPID dispid)
	{
		OnChanged(true, dispid);
	}
};



template <
			class T, 
			class ThreadModel, 
			const CLSID* pclsid, 
			const IID* psrcid, 
			const GUID* plibid, 
			WORD wMajor, 
			WORD wMinor
			>
class CComFullCompositeControlEx : public CComFullControlEx< T, ThreadModel, pclsid, psrcid, plibid, wMajor, wMinor, CAxDialogImpl< T > >
{
protected:
	typedef CComFullCompositeControlEx< T, ThreadModel, pclsid, psrcid, plibid, wMajor, wMinor> _CComFullCompositeControlEx;

public:
	CComFullCompositeControlEx()
	{
		m_hbrBackground = NULL;
	}
	~CComFullCompositeControlEx()
	{
		DeleteObject(m_hbrBackground);
	}
	HRESULT AdviseSinkMap(bool bAdvise)
	{
		if(!bAdvise && m_hWnd == NULL)
		{
			// window is gone, controls are already unadvised
			ATLTRACE2(atlTraceControls, 1, _T("CComFullCompositeControlEx::AdviseSinkMap called after the window was destroyed\n"));
			return S_OK;
		}
		T* pT = static_cast<T*>(this);
		return AtlAdviseSinkMap(pT, bAdvise);
	}
	HBRUSH m_hbrBackground;
	HRESULT SetBackgroundColorFromAmbient()
	{
		if (m_hbrBackground != NULL)
		{
			DeleteObject(m_hbrBackground);
			m_hbrBackground = NULL;
		}
		OLE_COLOR clr;
		HRESULT hr = GetAmbientBackColor(clr);
		if (SUCCEEDED(hr))
		{
			COLORREF rgb;
			::OleTranslateColor(clr, NULL, &rgb);
			m_hbrBackground = ::CreateSolidBrush(rgb);
			EnumChildWindows(m_hWnd, (WNDENUMPROC)BackgroundColorEnumProc, (LPARAM) clr);
		}
		return hr;
	}
	static BOOL CALLBACK BackgroundColorEnumProc(HWND hwnd, LPARAM l)
	{
		CAxWindow wnd(hwnd);
		CComPtr<IAxWinAmbientDispatch> spDispatch;
		wnd.QueryHost(&spDispatch);
		if (spDispatch != NULL)
			spDispatch->put_BackColor((OLE_COLOR)l);
		return TRUE;
	}
	LRESULT OnDialogColor(UINT, WPARAM w, LPARAM, BOOL&)
	{
		HDC dc = (HDC) w;
		LOGBRUSH lb;
		::GetObject(m_hbrBackground, sizeof(lb), (void*)&lb);
		::SetBkColor(dc, lb.lbColor);
		return (LRESULT)m_hbrBackground;
	}
	HWND Create(HWND hWndParent, RECT& /*rcPos*/, LPARAM dwInitParam = NULL)
	{
		_CComFullControlEx::Create(hWndParent, dwInitParam);
		SetBackgroundColorFromAmbient();
		if (m_hWnd != NULL)
			ShowWindow(SW_SHOWNOACTIVATE);
		return m_hWnd;
	}
	BOOL CalcExtent(SIZE& size)
	{
		HINSTANCE hInstance = _Module.GetResourceInstance();
		LPCTSTR lpTemplateName = MAKEINTRESOURCE(T::IDD);
		HRSRC hDlgTempl = FindResource(hInstance, lpTemplateName, RT_DIALOG);
		if (hDlgTempl == NULL)
			return FALSE;
		HGLOBAL hResource = LoadResource(hInstance, hDlgTempl);
		DLGTEMPLATE* pDlgTempl = (DLGTEMPLATE*)LockResource(hResource);
		if (pDlgTempl == NULL)
			return FALSE;
		AtlGetDialogSize(pDlgTempl, &size);
		AtlPixelToHiMetric(&size, &size);
		return TRUE;
	}
//Implementation
	BOOL PreTranslateAccelerator(LPMSG pMsg, HRESULT& hRet)
	{
		hRet = S_OK;
		if ((pMsg->message < WM_KEYFIRST || pMsg->message > WM_KEYLAST) &&
		   (pMsg->message < WM_MOUSEFIRST || pMsg->message > WM_MOUSELAST))
			return FALSE;
		// find a direct child of the dialog from the window that has focus
		HWND hWndCtl = ::GetFocus();
		if (IsChild(hWndCtl) && ::GetParent(hWndCtl) != m_hWnd)
		{
			do
			{
				hWndCtl = ::GetParent(hWndCtl);
			}
			while (::GetParent(hWndCtl) != m_hWnd);
		}
		// give controls a chance to translate this message
		if (::SendMessage(hWndCtl, WM_FORWARDMSG, 0, (LPARAM)pMsg) == 1)
			return TRUE;

		// special handling for keyboard messages
		DWORD dwDlgCode = ::SendMessage(pMsg->hwnd, WM_GETDLGCODE, 0, 0L);
		switch(pMsg->message)
		{
		case WM_CHAR:
			if(dwDlgCode == 0)	// no dlgcode, possibly an ActiveX control
				return FALSE;	// let the container process this
			break;
		case WM_KEYDOWN:
			switch(LOWORD(pMsg->wParam))
			{
			case VK_TAB:
				// prevent tab from looping inside of our dialog
				if((dwDlgCode & DLGC_WANTTAB) == 0)
				{
					HWND hWndFirstOrLast = ::GetWindow(m_hWnd, GW_CHILD);
					if (::GetKeyState(VK_SHIFT) >= 0)  // not pressed
						hWndFirstOrLast = GetNextDlgTabItem(hWndFirstOrLast, TRUE);
					if (hWndFirstOrLast == hWndCtl)
						return FALSE;
				}
				break;
			case VK_LEFT:
			case VK_UP:
			case VK_RIGHT:
			case VK_DOWN:
				// prevent arrows from looping inside of our dialog
				if((dwDlgCode & DLGC_WANTARROWS) == 0)
				{
					HWND hWndFirstOrLast = ::GetWindow(m_hWnd, GW_CHILD);
					if (pMsg->wParam == VK_RIGHT || pMsg->wParam == VK_DOWN)	// going forward
						hWndFirstOrLast = GetNextDlgTabItem(hWndFirstOrLast, TRUE);
					if (hWndFirstOrLast == hWndCtl)
						return FALSE;
				}
				break;
			case VK_EXECUTE:
			case VK_RETURN:
			case VK_ESCAPE:
			case VK_CANCEL:
				// we don't want to handle these, let the container do it
				return FALSE;
			}
			break;
		}

		return IsDialogMessage(pMsg);
	}
	HRESULT IOleInPlaceObject_InPlaceDeactivate(void)
	{
		AdviseSinkMap(false); //unadvise
		return _CComFullControlEx::IOleInPlaceObject_InPlaceDeactivate();
	}
	virtual HWND CreateControlWindow(HWND hWndParent, RECT& rcPos)
	{
		HWND hWnd=_CComFullControlEx::CreateControlWindow(hWndParent, rcPos);
		if (hWnd != NULL)
			AdviseSinkMap(true);
		return hWnd;
	}
	virtual HRESULT OnDraw(ATL_DRAWINFO& di)
	{
		if(!m_bInPlaceActive)
		{
			HPEN hPen = (HPEN)::GetStockObject(BLACK_PEN);
			HBRUSH hBrush = (HBRUSH)::GetStockObject(GRAY_BRUSH);
			::SelectObject(di.hdcDraw, hPen);
			::SelectObject(di.hdcDraw, hBrush);
			::Rectangle(di.hdcDraw, di.prcBounds->left, di.prcBounds->top, di.prcBounds->right, di.prcBounds->bottom);
			::SetTextColor(di.hdcDraw, ::GetSysColor(COLOR_WINDOWTEXT));
			::SetBkMode(di.hdcDraw, TRANSPARENT);
			::DrawText(di.hdcDraw, _T("ATL Composite Control Ex"), -1, (LPRECT)di.prcBounds, DT_CENTER | DT_SINGLELINE | DT_VCENTER);
		}
		return S_OK;
	}
	BEGIN_MSG_MAP(CComCompositeControl< T >)
		MESSAGE_HANDLER(WM_CTLCOLORDLG, OnDialogColor)
		MESSAGE_HANDLER(WM_CTLCOLORSTATIC, OnDialogColor)
		MESSAGE_HANDLER(WM_SETFOCUS, CComControlBase::OnSetFocus)
		MESSAGE_HANDLER(WM_KILLFOCUS, CComControlBase::OnKillFocus)
		MESSAGE_HANDLER(WM_MOUSEACTIVATE, CComControlBase::OnMouseActivate)
	END_MSG_MAP()

	BEGIN_SINK_MAP(T)
	END_SINK_MAP()
};




}	// namespace Simple

#endif		// __ATLCTL_H__


#endif		// __COMCONTROLEX_H

