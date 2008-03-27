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
// SimpleDataObject.cpp - implementation of CSimpleDataObjectImpl class

#include "stdafx.h"
#include "SimpleLibAtlComBuild.h"

#include "SimpleDataObject.h"
#include "SimpleDragDrop.h"

namespace Simple
{

void MakeFormatEtc(UINT cf, FORMATETC* pfetc)
{
	pfetc->cfFormat=cf;
	pfetc->dwAspect=DVASPECT_CONTENT;
	pfetc->lindex=-1;
	pfetc->ptd=NULL;
	pfetc->tymed=TYMED_HGLOBAL;
}


//////////////////////////////////////////////////////////////////////
// CSimpleEnumFormats

// Constructor
CSimpleEnumFormats::CSimpleEnumFormats(ULONG nStartIndex)
{
	m_dwRef=0;
	m_nPos=nStartIndex;
}

// Destructor
CSimpleEnumFormats::~CSimpleEnumFormats()
{
}


// Add a new FORMAT
void CSimpleEnumFormats::Add(FORMATETC* pfetc)
{
	FORMATETC* pNew=new FORMATETC;
	*pNew=*pfetc;
	m_Formats.Add(pNew);
}

//////////////////////////////////////////////////////////////////////
// IUnknown

STDMETHODIMP_(ULONG) CSimpleEnumFormats::AddRef()
{
	return ++m_dwRef;
}

STDMETHODIMP_(ULONG) CSimpleEnumFormats::Release()
{
	m_dwRef--;
	if (m_dwRef==0)
		{
		delete this;
		return 0;
		}

	return m_dwRef;
}

STDMETHODIMP CSimpleEnumFormats::QueryInterface(REFIID iid, void ** ppvObject)
{
	// Check params
	if (!ppvObject) 
		return E_POINTER;

	// IUnknown or IDispatch?
	if ((iid==IID_IUnknown) || (iid==IID_IEnumFORMATETC))
		{
		AddRef();
		*ppvObject=static_cast<IEnumFORMATETC*>(this);
		return S_OK;
		}

	// Unsupported interface
	return E_NOINTERFACE;
}


// Get more supported formats...
STDMETHODIMP CSimpleEnumFormats::Next(ULONG celt, FORMATETC* rgelt, ULONG* pceltFetched)
{
	// Quit if nothing to do
	if (celt==0) return S_OK;
	if (rgelt==NULL) return E_POINTER;


	// Quit if requested more than 1 element and no return count buffer
	if ((celt>1) && (pceltFetched==NULL))
		return E_POINTER;

	// Work out how many can return
	BOOL bEnd=FALSE;
	if (m_nPos+celt > (UINT)m_Formats.GetSize())
		{
		celt=m_Formats.GetSize()-m_nPos;
		bEnd=TRUE;
		}

	// Setup return count
	if (pceltFetched) *pceltFetched=celt;

	// Return em...
	for (UINT i=0; i<celt; i++)
	{
		rgelt[i]=*m_Formats[m_nPos+i];
	}

	// Update position
	m_nPos+=celt;
	ASSERT(m_nPos<=(UINT)m_Formats.GetSize());

	// Done
	return bEnd ? S_FALSE : S_OK;
}

// Skip forware
STDMETHODIMP CSimpleEnumFormats::Skip(ULONG celt)
{
	// Update position
	m_nPos+=celt;

	// Check not past end...
	if (m_nPos>=(UINT)m_Formats.GetSize())
		{
		m_nPos=m_Formats.GetSize();
		return S_FALSE;
		}
		
	// Done
	return S_OK;
}

// Reset position
STDMETHODIMP CSimpleEnumFormats::Reset()
{
	m_nPos=0;
	return S_OK;
}

// Clone enumerator
STDMETHODIMP CSimpleEnumFormats::Clone(IEnumFORMATETC** ppenum)
{
	// Check params
	if (!ppenum) return E_POINTER;

	// Create a clone
	CSimpleEnumFormats* pClone=new CSimpleEnumFormats(m_nPos);

	for (int i=0; i<m_Formats.GetSize(); i++)
	{
		pClone->Add(m_Formats[i]);
	}

	// Addref and return if
	pClone->AddRef();
	*ppenum=static_cast<IEnumFORMATETC*>(pClone);

	return S_OK;
}



//////////////////////////////////////////////////////////////////////////
// CSimpleDataObjectImpl

// Constructor
CSimpleDataObjectImpl::CSimpleDataObjectImpl()
{
}

// Destructor
CSimpleDataObjectImpl::~CSimpleDataObjectImpl()
{
}


HRESULT CSimpleDataObjectImpl::AddData(UINT cfFormat, TYMED tymed)
{
	// Remove existing entry
	CFormat* pFormat=FindFormat(cfFormat, tymed);
	if (pFormat)
	{
		m_Formats.Remove(pFormat);
	}
	
	// Create a new format entry
	pFormat=new CFormat(cfFormat, tymed);

	// Add it
	m_Formats.Add(pFormat);

	return S_OK;
}

HRESULT CSimpleDataObjectImpl::AddData(UINT cfFormat, IStream* pStream)
{
	// Remove existing entry
	CFormat* pFormat=FindFormat(cfFormat, TYMED_ISTREAM);
	if (pFormat)
	{
		m_Formats.Remove(pFormat);
	}
	
	// Create a new format entry
	pFormat=new CFormat(cfFormat, pStream);

	// Add it
	m_Formats.Add(pFormat);

	return S_OK;
}


HRESULT CSimpleDataObjectImpl::AddData(UINT cfFormat, HGLOBAL hData)
{
	// Remove existing entry
	CFormat* pFormat=FindFormat(cfFormat, TYMED_HGLOBAL);
	if (pFormat)
	{
		m_Formats.Remove(pFormat);
	}
	
	// Create a new format entry
	pFormat=new CFormat(cfFormat, hData);

	// Add it
	m_Formats.Add(pFormat);

	// Add emulated modes...
	if (cfFormat==CF_TEXT && !FindFormat(CF_UNICODETEXT, TYMED_HGLOBAL) && hData)
	{
		AddData(CF_UNICODETEXT, TYMED_HGLOBAL);
	}
	if (cfFormat==CF_UNICODETEXT && !FindFormat(CF_TEXT, TYMED_HGLOBAL) && hData)
	{
		AddData(CF_TEXT, TYMED_HGLOBAL);
	}

	return S_OK;
}


HRESULT CSimpleDataObjectImpl::AddData(UINT cfFormat, const void* pData, UINT cbData)
{
	// Allocate global memory
	HGLOBAL hData=GlobalAlloc(GHND, cbData ? cbData : 4);
	if (!hData)
		return E_OUTOFMEMORY;

	// Copy data
	if (cbData)
	{
		memcpy(GlobalLock(hData), pData, cbData);
		GlobalUnlock(hData);
	}

	// Add it
	return AddData(cfFormat, hData);
}

HRESULT CSimpleDataObjectImpl::AddText(LPCOLESTR psz, UINT cfFormat)
{
	return AddData(CF_UNICODETEXT, psz, (lstrlenW(psz)+1)*sizeof(OLECHAR));
}


HRESULT CSimpleDataObjectImpl::SetClipboard()
{
	RETURNIFFAILED(OleSetClipboard(this));
	RETURNIFFAILED(OleFlushClipboard());
	return S_OK;
}

HRESULT CSimpleDataObjectImpl::DoDragDrop(DWORD dwOKEffects, DWORD* pdwEffect)
{
	CSimpleDropSource DropSource;
	return ::DoDragDrop(this, &DropSource, dwOKEffects, pdwEffect);
}


CSimpleDataObjectImpl::CFormat* CSimpleDataObjectImpl::FindFormat(UINT cfFormat, TYMED tymed)
{
	for (int i=0; i<m_Formats.GetSize(); i++)
	{
		if (m_Formats[i]->m_cfFormat==cfFormat && (m_Formats[i]->m_tymed & tymed)!=0)
			return m_Formats[i];
	}

	return NULL;
}

HRESULT CSimpleDataObjectImpl::OnGetData(UINT cfFormat, LONG lIndex, HGLOBAL* phData)
{
	if (lIndex!=-1)
		return DV_E_LINDEX;

	if (cfFormat==CF_UNICODETEXT)
	{
		// Find existing entry
		CFormat* pFormat=FindFormat(CF_TEXT, TYMED_HGLOBAL);
		if (!pFormat)
			return DATA_E_FORMATETC;

		ASSERT(pFormat->m_hData);

		// Get ANSI text
		LPCSTR psz=(LPCSTR)GlobalLock(pFormat->m_hData);
		int iLenA=lstrlenA(psz)+1;

		// Work out unicode length
		int iLenW=MultiByteToWideChar(CP_ACP, 0, psz, iLenA, NULL, 0);

		// Allocate memory
		*phData=GlobalAlloc(GHND, iLenW * sizeof(OLECHAR));

		// Convert it
		MultiByteToWideChar(CP_ACP, 0, psz, iLenA, (LPOLESTR)GlobalLock(phData[0]), iLenW);

		// Unlock and finished
		GlobalUnlock(phData[0]);
		return S_OK;
	}

	if (cfFormat==CF_TEXT)
	{
		// Find existing entry
		CFormat* pFormat=FindFormat(CF_UNICODETEXT, TYMED_HGLOBAL);
		if (!pFormat)
			return DATA_E_FORMATETC;

		ASSERT(pFormat->m_hData);

		// Get ANSI text
		LPCOLESTR psz=(LPCOLESTR)GlobalLock(pFormat->m_hData);
		int iLenW=lstrlenW(psz)+1;

		// Work out unicode length
		int iLenA=WideCharToMultiByte(CP_ACP, 0, psz, iLenW, NULL, 0, NULL, NULL);

		// Allocate memory
		*phData=GlobalAlloc(GHND, iLenA * sizeof(OLECHAR));

		// Convert it
		WideCharToMultiByte(CP_ACP, 0, psz, iLenW, (LPSTR)GlobalLock(phData[0]), iLenA, NULL, NULL);

		// Unlock and finished
		GlobalUnlock(phData[0]);
		return S_OK;
	}


	return DATA_E_FORMATETC;
}


/////////////////////////////////////////////////////////////////////////////
// IDataObject

// Implementation of GetData
STDMETHODIMP CSimpleDataObjectImpl::GetData(FORMATETC* pformatetc, STGMEDIUM* pmedium)
{
	// Check format etc
	if (pformatetc->dwAspect!=DVASPECT_CONTENT)	return DV_E_DVASPECT;

#ifdef _DEBUG
	TCHAR szFormat[MAX_PATH];
	if (!GetClipboardFormatName(pformatetc->cfFormat, szFormat, MAX_PATH))
	{
		wsprintf(szFormat, _T("%i"), pformatetc->cfFormat);
	}
	ATLTRACE(_T("CSimpleDataObjectImpl::GetData - %s %i\n"), szFormat, pformatetc->tymed);
#endif

	// Get the format...
	CFormat* pFormat=FindFormat(pformatetc->cfFormat, (TYMED)pformatetc->tymed);
	if (!pFormat)
		return DATA_E_FORMATETC;

	// Init STGMEDIUM
	memset(pmedium, 0, sizeof(*pmedium));

	if (pformatetc->tymed & TYMED_HGLOBAL)
	{
		// Delayed data?
		if (!pFormat->m_hData)
		{
			if (SUCCEEDED(OnGetData(pformatetc->cfFormat, pformatetc->lindex, &pmedium->hGlobal)))
			{
				pmedium->tymed=TYMED_HGLOBAL;
				return S_OK;
			}
		}
		else if (pformatetc->lindex==-1)
		{
			// Allocate memory
			DWORD_PTR dwSize=GlobalSize(pFormat->m_hData);
			pmedium->hGlobal=GlobalAlloc(GHND, dwSize);
			if (!pmedium->hGlobal)
				return E_OUTOFMEMORY;

			// Copy it
			memcpy(GlobalLock(pmedium->hGlobal), GlobalLock(pFormat->m_hData), dwSize);

			// Unlock
			GlobalUnlock(pmedium->hGlobal);
			GlobalUnlock(pFormat->m_hData);

			pmedium->tymed=TYMED_HGLOBAL;
			return S_OK;
		}
	}

	if (pformatetc->tymed & TYMED_ISTREAM)
	{
		// Delayed data?
		if (!pFormat->m_spStream)
		{
			if (SUCCEEDED(OnGetData(pformatetc->cfFormat, pformatetc->lindex, &pmedium->pstm)))
			{
				pmedium->tymed=TYMED_ISTREAM;
				return S_OK;
			}
		}
		else if (pformatetc->lindex==-1)
		{
			pFormat->m_spStream.CopyTo(&pmedium->pstm);
			pmedium->tymed=TYMED_ISTREAM;
			return S_OK;
		}
	}

	// Unknown format
	return DATA_E_FORMATETC;
}


// Implementation of GetDataHere
STDMETHODIMP CSimpleDataObjectImpl::GetDataHere(FORMATETC* pformatetc, STGMEDIUM* pmedium)
{
	return E_NOTIMPL;
}

// Implementation of QueryGetData
STDMETHODIMP CSimpleDataObjectImpl::QueryGetData(FORMATETC* pformatetc)
{
	// Check format etc
	if (pformatetc->dwAspect!=DVASPECT_CONTENT)	return DV_E_DVASPECT;

	// Supported format?
	if (FindFormat(pformatetc->cfFormat, (TYMED)pformatetc->tymed))
		return S_OK;

	// Unsupported
	return DV_E_FORMATETC;
}

// Implementation of GetCanonicalFormatEtc
STDMETHODIMP CSimpleDataObjectImpl::GetCanonicalFormatEtc(FORMATETC* pformatectIn, FORMATETC* pformatetcOut)
{
	return E_NOTIMPL;
}

// Implementation of SetData
STDMETHODIMP CSimpleDataObjectImpl::SetData(FORMATETC* pformatetc, STGMEDIUM* pmedium, BOOL fRelease)
{
	return E_NOTIMPL;
}

// Implementation of EnumFormatEtc
STDMETHODIMP CSimpleDataObjectImpl::EnumFormatEtc(DWORD dwDirection, IEnumFORMATETC** ppenumFormatEtc)
{
	// Check direction
	if (dwDirection!=DATADIR_GET) return E_NOTIMPL;
	if (!ppenumFormatEtc) return E_POINTER;

	// Create a clone
	CSimpleEnumFormats* pClone=new CSimpleEnumFormats();

	FORMATETC fetc;
	fetc.dwAspect=DVASPECT_CONTENT;
	fetc.lindex=-1;
	fetc.ptd=NULL;
	for (int i=0; i<m_Formats.GetSize(); i++)
	{
		fetc.tymed=m_Formats[i]->m_tymed;
		fetc.cfFormat=m_Formats[i]->m_cfFormat;
		pClone->Add(&fetc);
	}

	// Addref and return if
	pClone->AddRef();
	*ppenumFormatEtc=static_cast<IEnumFORMATETC*>(pClone);

	return S_OK;
}

// Implementation of DAdvise
STDMETHODIMP CSimpleDataObjectImpl::DAdvise(FORMATETC* pformatetc, DWORD advf, IAdviseSink* pAdvSink, DWORD* pdwConnection)
{
	return E_NOTIMPL;
}

// Implementation of DUnadvise
STDMETHODIMP CSimpleDataObjectImpl::DUnadvise(DWORD dwConnection)
{
	return E_NOTIMPL;
}

// Implementation of EnumDAdvise
STDMETHODIMP CSimpleDataObjectImpl::EnumDAdvise(IEnumSTATDATA** ppenumAdvise)
{
	return E_NOTIMPL;
}


}	// namespace Simple
