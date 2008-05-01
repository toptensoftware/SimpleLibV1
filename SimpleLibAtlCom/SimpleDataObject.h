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
// SimpleDataObject.h - declaration of CSimpleDataObjectImpl class

#ifndef __SIMPLEDATAOBJECT_H
#define __SIMPLEDATAOBJECT_H

namespace Simple
{

void SIMPLEAPI MakeFormatEtc(UINT cf, FORMATETC* pfetc);

class CStgMedium : public STGMEDIUM
{
public:
	CStgMedium()
	{
		memset(static_cast<STGMEDIUM*>(this), 0, sizeof(STGMEDIUM));
	}

	~CStgMedium()
	{
		ReleaseStgMedium(this);
	}
};

class CSimpleEnumFormats : public IEnumFORMATETC
{
public:
// Construction
			CSimpleEnumFormats(ULONG nStartIndex=0);
	virtual ~CSimpleEnumFormats();

	void Add(FORMATETC* pfetc);

protected:
// Implementation

// Attributes	   
	DWORD		m_dwRef;
	ULONG		m_nPos;
	CVector<FORMATETC*, SOwnedPtr>	m_Formats;

public:
// IUnknown
	STDMETHOD_(ULONG, AddRef)();
	STDMETHOD_(ULONG, Release)();
	STDMETHOD(QueryInterface)(REFIID iid, void ** ppvObject);

// IEnumFORMATETC
    STDMETHOD(Next)(ULONG celt, FORMATETC* rgelt, ULONG* pceltFetched);
	STDMETHOD(Skip)(ULONG celt);
	STDMETHOD(Reset)();
	STDMETHOD(Clone)(IEnumFORMATETC** ppenum);
};


// CSimpleDataObjectImpl Class
class CSimpleDataObjectImpl : 
	public IDataObject
{
public:
// Construction
			CSimpleDataObjectImpl();
	virtual ~CSimpleDataObjectImpl();

// Operations

	HRESULT AddData(UINT cfFormat, TYMED tymed);
	HRESULT AddData(UINT cfFormat, IStream* pStream);
	HRESULT AddData(UINT cfFormat, HGLOBAL hData);
	HRESULT AddData(UINT cfFormat, const void* pData, UINT cbData);
	HRESULT AddText(LPCOLESTR psz, UINT cfFormat=CF_UNICODETEXT);
	HRESULT SetClipboard();
	HRESULT DoDragDrop(DWORD dwOKEffects, DWORD* pdwEffect);

// Overrides
	virtual HRESULT OnGetData(UINT cfFormat, LONG lIndex, HGLOBAL* phData);
	virtual HRESULT OnGetData(UINT cfFormat, LONG lIndex, IStream** ppStream) { return DATA_E_FORMATETC; };

// Implementation
protected:
// Attributes
	class CFormat
	{
	public:
		CFormat(UINT cfFormat, TYMED tymed) :
			m_cfFormat(cfFormat),
			m_hData(NULL),
			m_tymed(tymed)
		{
		}							 

		CFormat(UINT cfFormat, IStream* pStream) :
			m_cfFormat(cfFormat),
			m_hData(NULL),
			m_spStream(pStream),
			m_tymed(TYMED_ISTREAM)
		{
		}							 

		CFormat(UINT cfFormat, HGLOBAL hData) :
			m_cfFormat(cfFormat),
			m_hData(hData),
			m_tymed(TYMED_HGLOBAL)
		{
		}

		~CFormat()
		{
			if (m_hData)
				GlobalFree(m_hData);
		}

		UINT				m_cfFormat;
		HGLOBAL				m_hData;
		TYMED				m_tymed;
		CComPtr<IStream>	m_spStream;
	};

	CVector<CFormat*, SOwnedPtr>	m_Formats;

// Operations
	CFormat* FindFormat(UINT cfFormat, TYMED tymed);

// IDataObject
	STDMETHODIMP GetData(FORMATETC* pformatetcIn, STGMEDIUM* pmedium);
	STDMETHODIMP GetDataHere(FORMATETC* pformatetc, STGMEDIUM* pmedium);
	STDMETHODIMP QueryGetData(FORMATETC* pformatetc);
	STDMETHODIMP GetCanonicalFormatEtc(FORMATETC* pformatectIn, FORMATETC* pformatetcOut);
	STDMETHODIMP SetData(FORMATETC* pformatetc, STGMEDIUM* pmedium, BOOL fRelease);
	STDMETHODIMP EnumFormatEtc(DWORD dwDirection, IEnumFORMATETC** ppenumFormatEtc);
	STDMETHODIMP DAdvise(FORMATETC* pformatetc, DWORD advf, IAdviseSink* pAdvSink, DWORD* pdwConnection);
	STDMETHODIMP DUnadvise(DWORD dwConnection);
	STDMETHODIMP EnumDAdvise(IEnumSTATDATA** ppenumAdvise);
};


class CSimpleDataObject : 	public CSimpleDataObjectImpl
{
public:
// Construction
	CSimpleDataObject()
	{
	}
	virtual ~CSimpleDataObject()
	{
	}

// IUnknown
	STDMETHODIMP_(ULONG) AddRef()
	{
		return 2;
	}

	STDMETHODIMP_(ULONG) Release()
	{
		return 1;
	}

	STDMETHODIMP QueryInterface(REFIID riid, void** ppvObject)
	{
		if (riid==IID_IUnknown || riid==IID_IDataObject)
			{
			*ppvObject=static_cast<IDataObject*>(this);
			AddRef();
			return S_OK;
			}

		return E_NOINTERFACE;
	}

};


}	// namespace Simple

#endif	// __SIMPLEDATAOBJECT_H

