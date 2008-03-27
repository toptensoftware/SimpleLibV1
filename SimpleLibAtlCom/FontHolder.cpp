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
// FontHolder.cpp - implementation of FontHolder

#include "stdafx.h"
#include "SimpleLibAtlComBuild.h"

#include "FontHolder.h"

namespace Simple
{


// Constructor
CComFontHolderBase::CComFontHolderBase()
{
	m_dwAdvise=0;
	m_hCachedHFont=NULL;
	CreateDefaultFont();
};

// Destructor
CComFontHolderBase::~CComFontHolderBase()
{
	// Clean up...
	ReleaseFont();
}


// Release font and resources
void CComFontHolderBase::ReleaseFont()
{
	if (!m_spFont)
		return;
		
	// Release HFONT
	if (m_hCachedHFont)
		m_spFont->ReleaseHfont(m_hCachedHFont);

	// Unadvise
	if (m_dwAdvise!=0)
		{
		AtlUnadvise(m_spFont, IID_IPropertyNotifySink, m_dwAdvise);
		m_dwAdvise=0;
		}

	// Clean up...
	m_spFont.Release();
	m_hCachedHFont=NULL;
}


// Create a default font
void CComFontHolderBase::CreateDefaultFont()
{
	// Create a default font object
	CComPtr<IFont> spFont;
	if (SUCCEEDED(spFont.CoCreateInstance(CLSID_StdFont)))
		{
		// Set point size to 8.25
		CY cySize;
		cySize.int64=82500;
		spFont->put_Size(cySize);

		// Set the font
		putref(spFont);
		}
}

// Put a copy of an IFont
HRESULT CComFontHolderBase::put(IUnknown* newVal)
{
	CComQIPtr<IFont> spFont(newVal);

	if (spFont)
		{
		// Clone passed in font...
		CComPtr<IFont> spClone;
		HRESULT hr=spFont->Clone(&spClone);
		if (FAILED(hr)) return hr;

		// Put reference to clone
		return putref(spClone);
		}
	else
		return putref(NULL);
}

// Set the IFont
HRESULT CComFontHolderBase::putref(IUnknown* newVal)
{
	CComQIPtr<IFont> spFont(newVal);

	// Release old font object
	ReleaseFont();

	// Store new font object
	m_spFont=spFont;

	// Setup IPropertyNotifySink
	AtlAdvise(m_spFont, this, IID_IPropertyNotifySink, &m_dwAdvise);

	// Font changed!
	OnChanged(DISPID_VALUE);

	return S_OK;
}



// Get the IFont
HRESULT CComFontHolderBase::get(IFont** ppFont)
{
	return m_spFont.CopyTo(ppFont);
}


// Get the IFont as an IFontDisp
HRESULT CComFontHolderBase::get(IFontDisp** ppFontDisp)
{	
	// Check params
	if (!ppFontDisp) return E_POINTER;

	// NULL font?
	if (!m_spFont)
		{
		*ppFontDisp=NULL;
		return S_OK;
		}

	// Query for IFontDisp
	return m_spFont.QueryInterface(ppFontDisp);
}


HRESULT CComFontHolderBase::Clone(IFontDisp** ppFontDisp)
{
	CComPtr<IFont> spFont;
	RETURNIFFAILED(m_spFont->Clone(&spFont));
	return spFont.QueryInterface(ppFontDisp);
}

HRESULT CComFontHolderBase::Create(LPCOLESTR pszFaceName, __int64 ptSize, bool bBold, bool bItalic)
{
	// Create a new font
	FONTDESC fd;
	fd.cbSizeofstruct=sizeof(fd);
	fd.lpstrName=const_cast<LPOLESTR>(pszFaceName);
	fd.cySize.int64=ptSize;
	fd.sWeight=bBold ? FW_BOLD : FW_NORMAL;
	fd.sCharset=DEFAULT_CHARSET;
	fd.fItalic=bItalic ? 1 : 0;
	fd.fUnderline=0;
	fd.fStrikethrough=0;

	// Create new font
	CComPtr<IFont> spFont;
	OleCreateFontIndirect(&fd, IID_IFontDisp, (void**)&spFont);
	return putref(spFont);
}

void CComFontHolderBase::ReleaseFontHandle()
{
	if (m_hCachedHFont)
		{
		// Release old locked hFont?
		if (m_hCachedHFont)	m_spFont->ReleaseHfont(m_hCachedHFont);
		m_hCachedHFont=NULL;
		}
}

// Get HFont
HFONT CComFontHolderBase::GetFontHandle(BOOL bPermanent)
{
	HFONT hFont=NULL;
	if (m_spFont)
		{
		ReleaseFontHandle();

		// Get the font
		if (SUCCEEDED(m_spFont->get_hFont(&hFont)))
			{
			_ASSERTE(hFont!=NULL);

			if (bPermanent)
				{
				// AddRef and store new font
				m_spFont->AddRefHfont(hFont);
				m_hCachedHFont=hFont;
				}
			}
		}

	// Return font if we got one...
	if (hFont)
		return hFont;

	// Otherwise, fall back to system font if something went wrong...
	return (HFONT)GetStockObject(SYSTEM_FONT);
}

//////////////////////////////////////////////////////////////////////
// Property notify sink

HRESULT CComFontHolderBase::QueryInterface(REFIID riid, void** ppvObject)
{
	// Check params
	if (!ppvObject)
		return E_INVALIDARG;

	// Known interface?
	if ((riid==IID_IPropertyNotifySink || riid==IID_IUnknown))
		{
		*ppvObject=static_cast<IPropertyNotifySink*>(this);
		AddRef();
		return S_OK;
		}

	// Sorry...
	return E_NOINTERFACE;
}

ULONG CComFontHolderBase::AddRef()
{
	// Extremely complicated reference counting algorithm...
	return 2;
}

ULONG CComFontHolderBase::Release()
{
	// Extremely complicated reference counting algorithm...
	return 1;
}

HRESULT CComFontHolderBase::OnChanged(DISPID dispid)
{
	return S_OK;
}

HRESULT CComFontHolderBase::OnRequestEdit(DISPID dispid)
{
	return S_OK;
}



}	// namespace Simple
