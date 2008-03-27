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
// PropertyBagOnXML.h - declaration of PropertyBagOnXML

#ifndef __PROPERTYBAGONXML_H
#define __PROPERTYBAGONXML_H

#include "XMLArchive.h"

namespace Simple
{

// PropertyBagOnXML Class
class CPropertyBagOnXML : 
	public IPropertyBag,
//	public IPropertyBag2,
	public IErrorLog
{
public:
// Construction
			CPropertyBagOnXML();
	virtual ~CPropertyBagOnXML();

// Attributes
	HRESULT Init(CXMLArchive* pArchive);

// Operations

// Implementation
protected:
// Attributes
	CXMLArchive* m_pArchive;
	CXMLDOMElements	m_LoadParams;
	int				m_iLastMatchPos;

	bool DoesMatch(int iPos, LPCOLESTR pszPropName);

// Operations

// IUnknown
	STDMETHODIMP_(ULONG) AddRef();
	STDMETHODIMP_(ULONG) Release();
	STDMETHODIMP QueryInterface(REFIID riid, void** ppvObject);

// IPropertyBag
	STDMETHODIMP Read(LPCOLESTR pszPropName, VARIANT* pVar, IErrorLog* pErrorLog);
	STDMETHODIMP Write(LPCOLESTR pszPropName, VARIANT* pVar);

// IPropertyBag2
/*
	STDMETHODIMP Read(ULONG cProperties, PROPBAG2* pPropBag, IErrorLog* pErrLog, VARIANT* pvarValue, HRESULT* phrError);
	STDMETHODIMP Write(ULONG cProperties, PROPBAG2* pPropBag, VARIANT* pvarValue);
	STDMETHODIMP CountProperties(ULONG* pcProperties);
	STDMETHODIMP GetPropertyInfo(ULONG iProperty, ULONG cProperties, PROPBAG2* pPropBag, ULONG* pcProperties);
	STDMETHODIMP LoadObject(LPCOLESTR pstrName, DWORD dwHint, IUnknown* pUnkObject, IErrorLog* pErrLog);
*/
// IErrorLog
    STDMETHODIMP AddError(LPCOLESTR pszPropName, EXCEPINFO *pExcepInfo);
};


}	// namespace Simple

#endif	// __PROPERTYBAGONXML_H

