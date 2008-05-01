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
// ObjectWithNamedValues.h - declaration of ObjectWithNamedValues

#ifndef __OBJECTWITHNAMEDVALUES_H
#define __OBJECTWITHNAMEDVALUES_H

#include "XMLArchive.h"

namespace Simple
{

MIDL_INTERFACE("4A1FDC10-B5E4-4F5D-B5E2-F7215EFA895F")
IObjectWithNamedValuesListener : IUnknown
{
	// Will be call with NULL, NULL if RemoveAll called
	STDMETHOD(OnNamedValueChanged)(LPCOLESTR pszName, VARIANT* pNewValue);
};

// Internal IObjectWithNamedValues interface
MIDL_INTERFACE("C7F301DC-B350-429E-9314-AA8A7A8BD5C7")
IObjectWithNamedValues : IUnknown
{
	STDMETHOD(Reset)() PURE;
	STDMETHOD(SetNamedValue)(LPCOLESTR pszName, VARIANT_BOOL bPersist, VARIANT varValue) PURE;
	STDMETHOD(GetNamedValue)(LPCOLESTR pszName, VARIANT* pVal) PURE;
	STDMETHOD(GetCount)(int* pVal) PURE;
	STDMETHOD(GetItem)(int iIndex, BSTR* pbstrName, VARIANT_BOOL* pbPersisted, VARIANT* pvarValue) PURE;
	STDMETHOD(AddListener)(LPCOLESTR pszName, VARIANT_BOOL bStrongRef, IObjectWithNamedValuesListener* pListener);
	STDMETHOD(RemoveListener)(LPCOLESTR pszName, IObjectWithNamedValuesListener* pListener);
};


// Helper functions for working with named value collection
HRESULT SIMPLEAPI CopyNamedValues(IObjectWithNamedValues* pDest, IObjectWithNamedValues* pSource, bool bAll);
bool SIMPLEAPI AnyPersistedNamedValues(IObjectWithNamedValues* pNamedValues);
HRESULT SIMPLEAPI SerializeNamedValues(IObjectWithNamedValues* pNamedValues, CXMLArchive& ar, LPCOLESTR pszExcludeOnLoad, LPCOLESTR pszElementName);

class ATL_NO_VTABLE IObjectWithNamedValuesImpl : public IObjectWithNamedValues
{
public:
// Construction
			IObjectWithNamedValuesImpl();
	virtual ~IObjectWithNamedValuesImpl();

// IObjectWithNamedValues
	STDMETHODIMP Reset();
	STDMETHODIMP SetNamedValue(LPCOLESTR pszName, VARIANT_BOOL bPersist, VARIANT newVal);
	STDMETHODIMP GetNamedValue(LPCOLESTR pszName, VARIANT* pVal);
	STDMETHODIMP GetCount(int* pVal);
	STDMETHODIMP GetItem(int iIndex, BSTR* pbstrName, VARIANT_BOOL* pbPersisted, VARIANT* pvarValue);
	STDMETHODIMP AddListener(LPCOLESTR pszName, VARIANT_BOOL bStrongRef, IObjectWithNamedValuesListener* pListener);
	STDMETHODIMP RemoveListener(LPCOLESTR pszName, IObjectWithNamedValuesListener* pListener);

// Operations
	HRESULT Serialize(CXMLArchive& ar, LPCOLESTR pszExcludeOnLoad, LPCOLESTR pszElementName=NULL)
	{
		return Simple::SerializeNamedValues(this, ar, pszExcludeOnLoad, pszElementName);
	}
	bool AnyPersistedNamedValues()
	{
		return Simple::AnyPersistedNamedValues(this);
	}
	HRESULT CopyFrom(IObjectWithNamedValues* pOther, bool bAll)
	{
		return CopyNamedValues(this, pOther, bAll);
	}


// Implementation
protected:
// Attributes

// Types
	class CNamedValue
	{
	public:
	// Constructor
		CNamedValue(LPCOLESTR pszName, VARIANT_BOOL bSave);

	// Attributes
		CComBSTR		m_bstrName;
		VARIANT_BOOL	m_bPersist;
		CComVariant		m_varValue;
	};

	class CListener
	{
	public:
		CListener(LPCOLESTR pszName, bool bStrongRef, IObjectWithNamedValuesListener* pListener) : 
			m_bstrName(pszName),
			m_bStrongRef(bStrongRef),
			m_pListener(pListener)
		{
			if (bStrongRef)
				m_pListener->AddRef();
		}
		~CListener()
		{
			if (m_bStrongRef)
				m_pListener->Release();
		}

		CComBSTR	m_bstrName;
		IObjectWithNamedValuesListener* m_pListener;
		bool		m_bStrongRef;
	};

// Operations
	CNamedValue* Find(LPCOLESTR pszName);
	
// Attributes
	CVector<CNamedValue*, SOwnedPtr> m_Items;
	CVector<CListener*, SOwnedPtr> m_Listeners;
};



}	// namespace Simple

#endif	// __OBJECTWITHNAMEDVALUES_H

