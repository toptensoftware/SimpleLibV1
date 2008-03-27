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
// XMLArchive.h - declaration of XMLArchive

#ifndef __XMLARCHIVE_H
#define __XMLARCHIVE_H

#include "ComPtrVector.h"
#include <msxml.h>

namespace Simple
{

class CXMLArchive;

#define LCID_XML	0x0409

#define XMLARCHIVE_E_MISSINGATTRIBUTE		MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF, 5000)
#define XMLARCHIVE_E_MISSINGELEMENT			MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF, 5001)
#define XMLARCHIVE_E_PARSEERROR				MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF, 5002)


// Persist XML Flags
#define PXF_ALL						0x0000FFFF
#define PXF_PROPERTIES				0x00000001
#define PXF_CONTENT					0x00000002
#define PXF_RESETCONTENT			0x00000004		// When loading, reset content first
#define PXF_CLEARDIRTY				0x00000008
#define PXF_INITNEW					0x80000000		// Call InitNew when loading objects
#define PXF_DONTSAVEDEFAULTS		0x40000000		// Don't save default values

/////////////////////////////////////////////////////////////////////////////
// IPersistXML and IPersistXMLImpl

// IPersistXML interface declaration
MIDL_INTERFACE("E46A1B67-6741-11D4-8FCB-00A0C9E4F3AA")
IPersistXML : public IUnknown
{
public:
	virtual HRESULT STDMETHODCALLTYPE Save(IXMLDOMElement* pElement, IUnknown* pContext, DWORD dwFlags) = 0;
	virtual HRESULT STDMETHODCALLTYPE Load(IXMLDOMElement* pElement, IUnknown* pContext, DWORD dwFlags) = 0;
};


// Implementation of IPersistXML using CXMLArchive
class IPersistXMLImpl : public IPersistXML
{
protected:
	virtual HRESULT Serialize(CXMLArchive& ar)=0;

public:
// IPersistXML archive
	STDMETHODIMP Save(IXMLDOMElement* pElement, IUnknown* pContext, DWORD dwFlags);
	STDMETHODIMP Load(IXMLDOMElement* pElement, IUnknown* pContext, DWORD dwFlags);


// IStream support
	HRESULT Load(IStream* pStream, LPCOLESTR pszDocumentElementName, DWORD dwFlags);
	HRESULT Save(IStream* pStream, LPCOLESTR pszDocumentElementName, DWORD dwFlags);
};

// IPersistStream(Init) implementation using IPersistXML
/* 
eg:
	public IPersistXMLImpl,
	public IPersistStreamUsingXML<CYourClass>

	COM_INTERFACE_ENTRY(IPersist)
	COM_INTERFACE_ENTRY2(IPersistStream, IPersistStreamInit)
	COM_INTERFACE_ENTRY(IPersistStreamInit)
	COM_INTERFACE_ENTRY(IPersistXML)
*/

template <class T>
class IPersistStreamUsingXML : public IPersistStreamInit
{
public:
	IPersistStreamUsingXML(LPCOLESTR pszElementName) :
		m_bstrElementName(pszElementName)
	{
	}

	STDMETHODIMP GetClassID(CLSID* pClassID)
	{
		*pClassID = T::GetObjectCLSID();
		return S_OK;
	};
	STDMETHODIMP IsDirty(void )
	{
		T* pT = static_cast<T*>(this);
		return (pT->m_bRequiresSave) ? S_OK : S_FALSE;
	};
	STDMETHODIMP Load(LPSTREAM pStm)
	{
		IPersistXMLImpl* p=static_cast<T*>(this);
		return p->Load(pStm, m_bstrElementName, PXF_ALL);
	};
	STDMETHODIMP Save(LPSTREAM pStm, BOOL fClearDirty)
	{
		IPersistXMLImpl* p=static_cast<T*>(this);
		T* pT = static_cast<T*>(this);
		RETURNIFFAILED(p->Save(pStm, m_bstrElementName, PXF_ALL));
		if (fClearDirty)
			pT->m_bRequiresSave=false;
		return S_OK;
	};
	STDMETHODIMP GetSizeMax(ULARGE_INTEGER* pCbSize)
	{
		return E_NOTIMPL;
	};
	STDMETHODIMP InitNew(void )
	{
		return S_OK;
	};

	CComBSTR	m_bstrElementName;
};



/////////////////////////////////////////////////////////////////////////////
// CXMLArchive

typedef CComPtrVector<IXMLDOMElement*>	CXMLDOMElements;

class CXMLTarget
{
public:
	virtual BOOL IsStoring()=0;
	virtual BOOL IsLoading()=0;
	virtual HRESULT ExchangeAttribute(LPCOLESTR pszName, CComBSTR& bstrValue, LPCOLESTR pszDefault=NULL)=0;
	virtual HRESULT SetResult(HRESULT hr)=0;
	virtual void ClearError()=0;
	virtual bool SaveDefaults()=0;
};

class CXMLArchive;

class CXMLElementTarget : public CXMLTarget
{
public:
	CXMLElementTarget(CXMLArchive& ar);
	virtual BOOL IsStoring();
	virtual BOOL IsLoading();
	virtual HRESULT ExchangeAttribute(LPCOLESTR pszName, CComBSTR& bstrValue, LPCOLESTR pszDefault=NULL);
	virtual HRESULT SetResult(HRESULT hr);
	virtual void ClearError();
	virtual bool SaveDefaults();
	CXMLArchive& m_ar;
};

class CXMLArchive : public CXMLTarget
{
public:
// Construction
			CXMLArchive();
	virtual ~CXMLArchive();

// CXMLTarget overrides
	virtual BOOL IsStoring() { return m_bStoring; };
	virtual BOOL IsLoading() { return !m_bStoring; };
	// Attribute support (DONT USE THIS! - use XML_Item helpers below instead of this function)
	virtual HRESULT ExchangeAttribute(LPCOLESTR pszName, CComBSTR& bstrValue, LPCOLESTR pszDefault=NULL);
	virtual HRESULT SetResult(HRESULT hr);
	virtual void ClearError();
	virtual bool SaveDefaults() { return (m_dwFlags & PXF_DONTSAVEDEFAULTS)==0; };

	void DontSaveDefaults() { m_dwFlags |= PXF_DONTSAVEDEFAULTS; };

	CXMLTarget& Elements() { return m_Elements; };
	CXMLTarget& Attributes() { return *this; };

// Load initialisation
	HRESULT Load(IXMLDOMElement* pNode, DWORD dwFlags);
	HRESULT LoadXML(BSTR bstrXML, LPCOLESTR pszElementName, DWORD dwFlags);
	HRESULT LoadFile(BSTR bstrXML, LPCOLESTR pszElementName, DWORD dwFlags);
	HRESULT Load(IStream* pStream, LPCOLESTR pszElementName, DWORD dwFlags);

// Merge initialisation
	void SetFlags(DWORD dwFlags) { m_dwFlags=dwFlags; };
	DWORD GetFlags() { return m_dwFlags; };
	bool IsFlagSet(DWORD dwFlag) { return (m_dwFlags & dwFlag)!=0; };

// Save initialisation
	HRESULT InitNewDocument(LPCOLESTR pszDocumentElementName, DWORD dwFlags);
	HRESULT InitSave(IXMLDOMElement* pNode, DWORD dwFlags);

// Save finish operations
	HRESULT get_XML(BSTR* pVal);
	HRESULT Save(BSTR FileName);
	HRESULT Save(IStream* pStream);
	HRESULT FinishSave();

// Misc operations
	HRESULT GetResult() { return m_hrResult; };
	IXMLDOMElement*	CurrentElement();

// Element navigation
	HRESULT EnterElement(LPCOLESTR pszName, VARIANT_BOOL bOptional=FALSE);
	HRESULT EnterElement(IXMLDOMElement* pElement);
	HRESULT LeaveElement();


// Element support (DONT USE THIS! - use XX_Element helpers below instead of this function)
	HRESULT ExchangeElement(LPCOLESTR pszName, CComBSTR& bstrValue, LPCOLESTR pszDefault=NULL);

// Select nodes support
	HRESULT SelectElements(LPCOLESTR pszPattern, CXMLDOMElements& Elements);
	HRESULT GetChildElements(CXMLDOMElements& Elements);

	HRESULT SetContext(IUnknown* pUnk);
	HRESULT GetContext(IUnknown** pRetVal);

	IXMLDOMDocument* GetDocument();		
	HRESULT AppendText(LPCOLESTR psz);
	HRESULT AppendTabs(BOOL bCrLfFirst, int iCount);
	int CurrentDepth();
	HRESULT OpenChildElement(LPCOLESTR pszName, BOOL bOptional, IXMLDOMElement** ppElement);

// Implementation
protected:
// Attributes
	CComPtrVector<IXMLDOMElement*>		m_ElementStack;
	BOOL								m_bStoring;
	DWORD								m_dwFlags;
	CComPtr<IXMLDOMDocument>			m_spDoc;		// Always used GetDocument() 
	HRESULT								m_hrResult;
	int									m_iExternalNesting;
	IUnknown*							m_pContext;
	CXMLElementTarget					m_Elements;

// Operations
	HRESULT InitLoad(IXMLDOMDocument* pDocument, DWORD dwFlags, VARIANT_BOOL bOK, LPCOLESTR pszElementName);
	HRESULT LoadError(LPCOLESTR psz, HRESULT hr);
	HRESULT InitElementsVector(IXMLDOMNodeList* pNodeList, CXMLDOMElements& Elements);
};


template <class T>
class ATL_NO_VTABLE IPersistFileUsingXML : public IPersistFile
{
public:
// IPersist
	STDMETHOD(GetClassID)(CLSID *pClassID)
	{
		*pClassID = T::GetObjectCLSID();
		return S_OK;
	}

	STDMETHODIMP IsDirty(void )
	{
		return static_cast<T*>(this)->m_bRequiresSave ? S_OK : S_FALSE;
	};
	STDMETHODIMP Load(LPCOLESTR pszFileName, DWORD dwMode)
	{
		// Initialise archive
		CXMLArchive ar;
		RETURNIFFAILED(ar.LoadFile(CComBSTR(pszFileName), GetXMLRootElementName(false), PXF_ALL));

		// Load it
		Serialize(ar);

		// Done
		RETURNIFFAILED(ar.GetResult());

		// Store file name
		m_bstrFileName=pszFileName;

		return S_OK;
	};
	STDMETHODIMP Save(LPCOLESTR pszFileName, BOOL fRemember)
	{
		// Save to existing filename not set
		if (!pszFileName && IsEmptyString(m_bstrFileName))
			return E_UNEXPECTED;

		// Initialisate archive...
		CXMLArchive ar;
		RETURNIFFAILED(ar.InitNewDocument(GetXMLRootElementName(true), PXF_ALL));

		// Save it
		Serialize(ar);

		// Quit if failed...
		RETURNIFFAILED(ar.GetResult());

		// Write to file
		RETURNIFFAILED(ar.Save(pszFileName ? CComBSTR(pszFileName) : m_bstrFileName));

		// Clear dirty?
		if (pszFileName && fRemember)
			{
			static_cast<T*>(this)->m_bRequiresSave=false;
			m_bstrFileName=pszFileName;
			}

		// Done
		return S_OK;
	};
	STDMETHODIMP SaveCompleted(LPCOLESTR pszFileName)
	{
		if (pszFileName)
			m_bstrFileName=pszFileName;
		return S_OK;
	};
	STDMETHODIMP GetCurFile(LPOLESTR* ppszFileName)
	{
		CComBSTR bstrFileName;
		if (IsEmptyString(m_bstrFileName))
			{
			bstrFileName=GetFileSpec();
			}
		else
			{
			bstrFileName=m_bstrFileName;
			}

		int iLenBytes=sizeof(OLECHAR) * (wcslen(bstrFileName)+1);
		*ppszFileName=(LPOLESTR)CoTaskMemAlloc(iLenBytes);
		memcpy(*ppszFileName, bstrFileName.m_str, iLenBytes);

		return S_OK;
	};

	virtual HRESULT Serialize(CXMLArchive& ar)=0;
	virtual LPCOLESTR GetXMLRootElementName(bool bSave)=0;
	virtual LPCOLESTR GetFileSpec()=0;
	CComBSTR	m_bstrFileName;
};


template <class T>
class ATL_NO_VTABLE IPersistStreamInitUsingXML : public IPersistStreamInit
{
public:
// IPersist
	STDMETHOD(GetClassID)(CLSID *pClassID)
	{
		*pClassID = T::GetObjectCLSID();
		return S_OK;
	}

// IPersistStream
	STDMETHODIMP IsDirty(void )
	{
		return static_cast<T*>(this)->m_bRequiresSave ? S_OK : S_FALSE;
	};
	STDMETHODIMP Load(LPSTREAM pStm)
	{
		// Initialise archive
		CXMLArchive ar;
		RETURNIFFAILED(ar.Load(pStm, GetXMLRootElementName(false), PXF_ALL));

		// Load it
		Serialize(ar);

		// Done
		return ar.GetResult();
	};
	STDMETHODIMP Save(LPSTREAM pStm, BOOL fClearDirty)
	{
		// Initialisate archive...
		CXMLArchive ar;
		RETURNIFFAILED(ar.InitNewDocument(GetXMLRootElementName(true), PXF_ALL));

		// Save it
		Serialize(ar);

		// Quit if failed...
		RETURNIFFAILED(ar.GetResult());

		// Write it
		RETURNIFFAILED(ar.Save(pStm));

		// Clear dirty?
		if (fClearDirty)
			static_cast<T*>(this)->m_bRequiresSave=false;

		// Done
		return S_OK;
	};
	STDMETHODIMP GetSizeMax(ULARGE_INTEGER* pCbSize)
	{
		return E_NOTIMPL;
	};

// IPersistStreamInit
	STDMETHODIMP InitNew(void )
	{
		return S_OK;
	};

	virtual HRESULT Serialize(CXMLArchive& ar)=0;
	virtual LPCOLESTR GetXMLRootElementName(bool bSave)=0;
};


/////////////////////////////////////////////////////////////////////////////
// CPropertyBagOInXML support

#ifdef PROPERTYBAGONXML_SUPPORT


#endif			// PROPERTYBAGONXML_SUPPORT

/////////////////////////////////////////////////////////////////////////////
// Attribute exchange helpers

struct XMLENUMENTRY
{
	LPCOLESTR		pszName;
	unsigned int	iValue;
};

HRESULT XML_Item(CXMLTarget& ar, LPCOLESTR pszName, CUniString& strValue, LPCOLESTR pszDefault=NULL);
HRESULT XML_Item(CXMLTarget& ar, LPCOLESTR pszName, CComBSTR& bstrValue, LPCOLESTR pszDefault=NULL);
HRESULT XML_Item(CXMLTarget& ar, LPCOLESTR pszName, bool& bValue);
HRESULT XML_Item(CXMLTarget& ar, LPCOLESTR pszName, bool& bValue, bool bDefault);
HRESULT XML_Item(CXMLTarget& ar, LPCOLESTR pszName, int& iValue);
HRESULT XML_Item(CXMLTarget& ar, LPCOLESTR pszName, int& iValue, int iDefault);
HRESULT XML_Item(CXMLTarget& ar, LPCOLESTR pszName, unsigned int& iValue);
HRESULT XML_Item(CXMLTarget& ar, LPCOLESTR pszName, unsigned int& iValue, unsigned int iDefault);
HRESULT XML_Item(CXMLTarget& ar, LPCOLESTR pszName, long& iValue);
HRESULT XML_Item(CXMLTarget& ar, LPCOLESTR pszName, long& iValue, long iDefault);
HRESULT XML_Item(CXMLTarget& ar, LPCOLESTR pszName, unsigned long& iValue);
HRESULT XML_Item(CXMLTarget& ar, LPCOLESTR pszName, unsigned long& iValue, unsigned long iDefault);
HRESULT XML_Item(CXMLTarget& ar, LPCOLESTR pszName, double& dblValue);
HRESULT XML_Item(CXMLTarget& ar, LPCOLESTR pszName, double& dblValue, double dblDefault);
HRESULT XML_Item(CXMLTarget& ar, LPCOLESTR pszName, VARIANT* pvarValue, VARTYPE vt, VARIANT* pvarDefault);
HRESULT XML_Item(CXMLTarget& ar, LPCOLESTR pszName, IDispatch* pObject, DISPID dispid, VARTYPE vt, VARIANT* pvarDefault);
HRESULT XML_Item(CXMLTarget& ar, LPCOLESTR psz, CLSID& clsid);
//HRESULT XML_EnumItem(CXMLTarget& ar, LPCOLESTR pszName, unsigned int& iValue, unsigned int iDefault, LPCOLESTR* pszLookupTable, int iTableSize);
HRESULT XML_EnumItemInt(CXMLTarget& ar, LPCOLESTR pszName, unsigned int& iValue, XMLENUMENTRY* pEntries);
HRESULT XML_EnumItemInt(CXMLTarget& ar, LPCOLESTR pszName, unsigned int& iValue, XMLENUMENTRY* pEntries, unsigned int iDefault);
HRESULT XML_EnumItem(CXMLTarget& ar, LPCOLESTR pszName, IDispatch* pObject, DISPID dispid, XMLENUMENTRY* pEntries);
HRESULT XML_EnumItem(CXMLTarget& ar, LPCOLESTR pszName, IDispatch* pObject, DISPID dispid, XMLENUMENTRY* pEntries, unsigned int iDefault);

template <class T>
HRESULT XML_EnumItem(CXMLTarget& ar, LPCOLESTR pszName, T& iValue, XMLENUMENTRY* pEntries)
{
	ASSERT(sizeof(T)==sizeof(unsigned int));
	return XML_EnumItemInt(ar, pszName, reinterpret_cast<unsigned int&>(iValue), pEntries);
};

template <class T>
HRESULT XML_EnumItem(CXMLTarget& ar, LPCOLESTR pszName, T& iValue, XMLENUMENTRY* pEntries, T iDefault)
{
	ASSERT(sizeof(T)==sizeof(unsigned int));
	return XML_EnumItemInt(ar, pszName, reinterpret_cast<unsigned int&>(iValue), pEntries, iDefault);
};


/////////////////////////////////////////////////////////////////////////////
// Element exchange helpers


HRESULT XML_Element(CXMLArchive& ar, IUnknown* pObject);
HRESULT XML_Element(CXMLArchive& ar, LPCOLESTR pszName, IUnknown* pObject, VARIANT_BOOL bOptional);
HRESULT XML_Element(CXMLArchive& ar, IXMLDOMElement* pElement, IUnknown* pObject);
HRESULT XML_EmbeddedXML(CXMLArchive& ar, LPCOLESTR pszName, CComBSTR& bstrXML, LPCOLESTR psz);



/////////////////////////////////////////////////////////////////////////////
// Inline implementations of helper functions

inline HRESULT XML_Item(CXMLTarget& ar, LPCOLESTR pszName, CUniString& strValue, LPCOLESTR pszDefault)
{
	CComBSTR bstr;
	if (ar.IsStoring())
		bstr=strValue;
	HRESULT hr=ar.ExchangeAttribute(pszName, bstr, pszDefault);
	if (ar.IsLoading())
		strValue=bstr;
	return hr;
}

inline HRESULT XML_Item(CXMLTarget& ar, LPCOLESTR pszName, CComBSTR& bstrValue, LPCOLESTR pszDefault)
{
	return ar.ExchangeAttribute(pszName, bstrValue, pszDefault);
}


inline HRESULT XML_Item(CXMLTarget& ar, LPCOLESTR pszName, int& iValue)
{
	ASSERT(sizeof(int)==sizeof(long));
	return XML_Item(ar, pszName, (long&)iValue);
}

inline HRESULT XML_Item(CXMLTarget& ar, LPCOLESTR pszName, int& iValue, int iDefault)
{
	ASSERT(sizeof(int)==sizeof(long));
	return XML_Item(ar, pszName, (long&)iValue, (long)iDefault);
}

inline HRESULT XML_Item(CXMLTarget& ar, LPCOLESTR pszName, unsigned int& iValue)
{
	ASSERT(sizeof(unsigned int)==sizeof(unsigned long));
	return XML_Item(ar, pszName, (unsigned long&)iValue);
}

inline HRESULT XML_Item(CXMLTarget& ar, LPCOLESTR pszName, unsigned int& iValue, unsigned int iDefault)
{
	ASSERT(sizeof(unsigned int)==sizeof(unsigned long));
	return XML_Item(ar, pszName, (unsigned long&)iValue, (unsigned long)iDefault);
}


HRESULT SlxSaveObject(IUnknown* pObject, BSTR* pVal, LPCOLESTR pszDocElem, DWORD dwFlags=PXF_ALL);
HRESULT SlxLoadObject(IUnknown* pObject, BSTR newVal, LPCOLESTR pszDocElem, DWORD dwFlags=PXF_ALL);

}	// namespace Simple

#endif	// __XMLARCHIVE_H

