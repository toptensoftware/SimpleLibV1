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
// FontHolder.h - declaration of FontHolder

#ifndef __FONTHOLDER_H
#define __FONTHOLDER_H

namespace Simple
{

/*

About CComFontHolder
-----------------

CComFontHolderBase/CComFontHolder is a simple class to wrap up the code required
to implement a standard font property on an ActiveX control.  It supports:

	- connecting to the font's IPropertyNotifySink to receive changes to the font
	- provides methods to which the properties implementation can simply delegate
			(get, put and putref).
	- support for more than 1 font property on a control (use a different DISPID 
			in CComFontHolder constructor).
	- simple method for retrieving (and optionally addreffing) a font handle.


How to use CComFontHolder
----------------------

1. Add property to IDL as follows:

		[propget, id(DISPID_FONT)] HRESULT Font([out, retval] IFontDisp** pVal);
		[propput, id(DISPID_FONT)] HRESULT Font([in] IFontDisp* pVal);
		[propputref, id(DISPID_FONT)] HRESULT Font([in] IFontDisp* pVal);



2. Add to class declaration: (.h file)

	// Attributes
		CComFontHolder<CYourClass>	m_TheFont;



3. Add to class declaration: (.h file)

		STDMETHOD(get_Font)(IFontDisp** pVal);
		STDMETHOD(put_Font)(IFontDisp* pVal);
		STDMETHOD(putref_Font)(IFontDisp* pVal);



4. Add to class implementation: (.cpp file)

		// Implementation of get_Font
		STDMETHODIMP CYourClass::get_Font(IFontDisp** pVal)
		{
			return m_TheFont.get_IFontDisp(pVal);
		}

		// Implementation of put_Font
		STDMETHODIMP CYourClass::put_Font(IFontDisp* pVal)
		{
			return m_TheFont.put(pVal);
		}

		// Implementation of putref_Font
		STDMETHODIMP CYourClass::putref_Font(IFontDisp* pVal)
		{
			return m_TheFont.putref(pVal);
		}



5. Add this to property map to persist font settings

	PROP_ENTRY("Font", DISPID_FONT, CLSID_StockFontPage)



6. To class constructor add:

		CYourClass() : m_TheFont(this, DISPID_FONT)



7. Add a method to class
		
		void OnFontChanged(DISPID dispid)
		{
			// Shouldn't get this for different DISPID's
			if (dispid!=DISPID_FONT)
				return;

			// Set modified flag
			SetDirty(TRUE);

			// Handle change of font...
		}



8. Depending on your control's implementation, you may need to call 
	OnFontChange(DISPID_FONT) after the control is created to get 
	the correct font displayed when the control is first created.


To handle font change
---------------------

To get a handle to a font for use in rendering, use the GetFontHandle method of 
CComFontHolderBase.  This method returns a font handle (for the screen only) and optionally
keeps it locked (ie: permanent flag).  The permanent flags causes the font handle
to be addreffed with IFont::AddRefHfont.  The addreffed handle is automatically
released by CComFontHolderBase as appropriate.  See IFont::AddRefHFont for more about this.

Don't ever delete the font returned by GetFontHandle(TRUE) or GetFontHandle(FALSE).  Just
use it and be happy... :-)

If rendering yourself, leave the permanent flag off.  Example:

	OnFontChanged:
		
		void OnFontChanged(DISPID dispid)
		{
			::InvalidateRect(m_hWndControl, NULL, TRUE);
		}

	In paint handler:

		HDC hDC=::BeginPaint(hWnd, &ps)
		HFONT hOldFont=(HFONT)::SelectObject(hdc, m_TheFont.GetFontHandle(FALSE));
		// Paint something
		::DrawText(hDC, ...)
		::SelectObject(hDC, hOldFont);
		::EndPaint(hWnd, &ps);


If using a subclassed control, use the permanent flag.  Example:
	
	void CYourControl::OnFontChanged(DISPID dispid)
	{
		if (dispid!=DISPID_FONT)
			return;

		// Set modified flag
		SetDirty(TRUE);

		// Update font handle...
		SendMessage(m_hWndControl, WM_SETFONT, 
						(WPARAM)m_TheFont.GetFontHandle(TRUE), TRUE);
	}

*/

		

// Base class for CComFontHolder - provides most of the implementation without
// duplicating it in every template form of CComFontHolder.
class CComFontHolderBase : public IPropertyNotifySink
{
public:
// Construction
			CComFontHolderBase();
			~CComFontHolderBase();

// Operations
	void ReleaseFont();
	void CreateDefaultFont();
	HFONT GetFontHandle(BOOL bPermanent=TRUE);
	void ReleaseFontHandle();

	operator IFont*() { return m_spFont; };
	bool operator!() const
	{
		return (m_spFont == NULL);
	}

// Delegate property implementations to here...
	HRESULT put(IUnknown* newVal);
	HRESULT putref(IUnknown* newVal);
	HRESULT get(IFont** ppFont);
	HRESULT get(IFontDisp** ppFontDisp);
	HRESULT Clone(IFontDisp** ppFontDisp);
	HRESULT Create(LPCOLESTR pszFaceName, __int64 ptSize=100000, bool bBold=false, bool bItalic=false);


protected:
	// Attributes
	CComPtr<IFont>	m_spFont;
	DWORD	m_dwAdvise;
	HFONT	m_hCachedHFont;

	// IUnknown
	STDMETHOD(QueryInterface)(REFIID riid, void** ppvObject);
	STDMETHOD_(ULONG, AddRef)();
	STDMETHOD_(ULONG, Release)();

	// IPropertyNotifySink
	STDMETHOD(OnChanged)(DISPID dispid);
	STDMETHOD(OnRequestEdit)(DISPID dispid);
};


// CComFontHolder template.  Provides the link between CComFontHolderBase and 
//				CYourClass::OnFontChanged(DISPID).
template <class T>
class CComFontHolder : public CComFontHolderBase
{
public:
	// Constructor
	CComFontHolder(T* pNotify, DISPID dispid)
	{
		_ASSERTE(pNotify!=NULL);
		m_pNotify=pNotify;
		m_dispid=dispid;
	}

protected:
	STDMETHOD(OnChanged)(DISPID dispid)
	{
		// NB:
		//		dispid param is dispid of of font property that changed (eg: bold, pt size etc...)
		//		m_dispid is dispid of font object that changed (eg: DISPID_FONT)
		m_pNotify->OnFontChanged(m_dispid);

		return S_OK;
	}

	// Attributes
	DISPID	m_dispid;
	T*		m_pNotify;
};



}	// namespace Simple

#endif	// __FONTHOLDER_H

