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
// PropertyBagOnXML.cpp - implementation of PropertyBagOnXML

#include "stdafx.h"
#include "SimpleLibAtlComBuild.h"

#include "PropertyBagOnXML.h"

namespace Simple
{

//////////////////////////////////////////////////////////////////////////
// PropertyBagOnXML


// Constructor
CPropertyBagOnXML::CPropertyBagOnXML()
{
}

// Destructor
CPropertyBagOnXML::~CPropertyBagOnXML()
{
}


// Initialisation
HRESULT CPropertyBagOnXML::Init(CXMLArchive* pArchive)
{
	// Check params
	if (!pArchive)
		return E_POINTER;

	// Store element
	m_pArchive=pArchive;

	// Preload all param elements
	if (m_pArchive->IsLoading())
		{
		m_pArchive->SelectElements(CComBSTR(L"PARAM"), m_LoadParams);
		m_iLastMatchPos=0;
		}


	return S_OK;
}


bool CPropertyBagOnXML::DoesMatch(int iPos, LPCOLESTR pszPropName)
{
	// Get name attribute
	CComVariant varName;
	m_LoadParams[iPos]->getAttribute(CComBSTR(L"NAME"), &varName);
	if (V_VT(&varName)!=VT_BSTR)
		return false;

	// Same?
	return _wcsicmp(pszPropName, V_BSTR(&varName))==0;
}

/////////////////////////////////////////////////////////////////////////////
// IPropertyBag

STDMETHODIMP_(ULONG) CPropertyBagOnXML::AddRef()
{
	return 2;
}

STDMETHODIMP_(ULONG) CPropertyBagOnXML::Release()
{
	return 1;
}

STDMETHODIMP CPropertyBagOnXML::QueryInterface(REFIID riid, void** ppvObject)
{
	if (riid==IID_IUnknown || riid==IID_IPropertyBag)
		{
		*ppvObject=static_cast<IPropertyBag*>(this);
		AddRef();
		return S_OK;
		}

	if (riid==IID_IErrorLog)
		{
		*ppvObject=static_cast<IErrorLog*>(this);
		AddRef();
		return S_OK;
		}

	return E_NOINTERFACE;
}

/////////////////////////////////////////////////////////////////////////////
// IPropertyBag

// Implementation of Read
STDMETHODIMP CPropertyBagOnXML::Read(LPCOLESTR pszPropName, VARIANT* pVar, IErrorLog* pErrorLog)
{
	// Check have target element
	if (!m_pArchive)
		return E_UNEXPECTED;

	// Check params
	if (!pszPropName || !pVar)
		return E_POINTER;

	if (m_iLastMatchPos>=m_LoadParams.GetSize() || !DoesMatch(m_iLastMatchPos, pszPropName))
		{
		for (m_iLastMatchPos=0; m_iLastMatchPos<m_LoadParams.GetSize(); m_iLastMatchPos++)
			{
			if (DoesMatch(m_iLastMatchPos, pszPropName))
				break;
			}
		}

	// Not found?
	if (m_iLastMatchPos>=m_LoadParams.GetSize())
		{
		return E_INVALIDARG;
		}

	// Enter the element
	m_pArchive->EnterElement(m_LoadParams[m_iLastMatchPos]);

	// Next time check next element first
	m_iLastMatchPos++;


	// Try to load binary data...
	CComBSTR bstrBinary;
	if (XML_Item(m_pArchive->Elements(), L"BINARY", bstrBinary, L"")==S_OK)
		{
		unsigned char* pData;
		size_t cbData;
		if (UnformatBinaryData(bstrBinary, &pData, &cbData))
			{
			V_VT(pVar)=VT_BSTR;
			V_BSTR(pVar)=SysAllocStringByteLen((char*)pData, DWORD(cbData));
			free(pData);
			m_pArchive->LeaveElement();
			return S_OK;
			}

		}


	// If type not specified, check if property was a saved object
	if (V_VT(pVar)==VT_EMPTY)
		{
		// Load CLSID
		CLSID clsid;
		if (XML_Item(*m_pArchive, L"classid", clsid)==S_OK)
			{
			V_VT(pVar)=VT_UNKNOWN;
			V_UNKNOWN(pVar)=NULL;
			}
		}


	// Convert to a string value...
	switch (V_VT(pVar))
		{
		case VT_EMPTY:
			// If didn't specify a type, return as a string...
			V_VT(pVar)=VT_BSTR;
			V_BSTR(pVar)=NULL;
			break;;

		case VT_NULL:
			return S_OK;

		case VT_I2:
		case VT_I4:
		case VT_R4:
		case VT_R8:
		case VT_CY:
		case VT_DATE:
		case VT_BSTR:
		case VT_BOOL:
		case VT_DECIMAL:
		case VT_I1:
		case VT_UI1:
		case VT_UI2:
		case VT_UI4:
		case VT_I8:
		case VT_UI8:
		case VT_INT:
		case VT_UINT:
		case VT_LPSTR:
		case VT_LPWSTR:
		case VT_HRESULT:
		case VT_CLSID:
			break;

		case VT_DISPATCH:
		case VT_UNKNOWN:
			{
			// Create the object	
			if (!V_UNKNOWN(pVar))
				{
				CLSID clsid;
				XML_Item(*m_pArchive, L"classid", clsid);
				HRESULT hr=CoCreateInstance(clsid, NULL, CLSCTX_ALL, IID_IUnknown, (void**)&V_UNKNOWN(pVar));
				if (FAILED(hr))
					{
					m_pArchive->LeaveElement();
					return hr;
					}
				}
				
			// Persist the object
			XML_Element(*m_pArchive, V_UNKNOWN(pVar));

			m_pArchive->LeaveElement();
			return m_pArchive->GetResult();
			}
			break;

		default:
			m_pArchive->LeaveElement();
			// Unsupported variant type...
			return E_FAIL;
		}

	
	// Get value
	CComBSTR bstrValue;
	if (XML_Item(*m_pArchive, L"VALUE", bstrValue, L"")==S_FALSE)
		{
		m_pArchive->LeaveElement();
		return E_INVALIDARG;
		}
	
	m_pArchive->LeaveElement();

	// Convert to required type
	CComVariant varTemp(bstrValue);
	RETURNIFFAILED(VariantChangeTypeEx(&varTemp, &varTemp, LCID_XML, 0, V_VT(pVar)));

	// Return it
	return varTemp.Detach(pVar);
}

// Implementation of Write
STDMETHODIMP CPropertyBagOnXML::Write(LPCOLESTR pszPropName, VARIANT* pVar)
{
	ASSERT(m_pArchive->IsStoring());

	// Check have target element
	if (!m_pArchive)
		return E_UNEXPECTED;

	// Check params
	if (!pszPropName || !pVar)
		return E_POINTER;

	// Convert to a string value...
	CComBSTR bstrValue;	
	switch (V_VT(pVar))
		{
		case VT_EMPTY:
			bstrValue="";
			break;

		case VT_NULL:
			bstrValue="null";
			break;

		case VT_BSTR:
			if (V_BSTR(pVar))
				{
				// Check for binary value
				DWORD dwLen=SysStringLen(V_BSTR(pVar));
				BOOL bDefCharUsed=FALSE;
				WideCharToMultiByte(CP_ACP, 0, V_BSTR(pVar), dwLen, NULL, 0, NULL, &bDefCharUsed);
				if (bDefCharUsed)
					{
					// Its a binary blob or non-ansi data, save as hex encoded data...
					CUniString strBinary=FormatBinaryData((LPBYTE)V_BSTR(pVar), SysStringByteLen(V_BSTR(pVar)), 32);

					// Enter param element
					m_pArchive->EnterElement(L"PARAM");

					// Save name
					CComBSTR bstrName(pszPropName);
					XML_Item(*m_pArchive, L"NAME", bstrName);

					// Save binary data...
					XML_Item(m_pArchive->Elements(), L"BINARY", strBinary);

					// Leave
					m_pArchive->LeaveElement();
					return m_pArchive->GetResult();
					}
				}


		case VT_I2:
		case VT_I4:
		case VT_R4:
		case VT_R8:
		case VT_CY:
		case VT_DATE:
		case VT_BOOL:
		case VT_DECIMAL:
		case VT_I1:
		case VT_UI1:
		case VT_UI2:
		case VT_UI4:
		case VT_I8:
		case VT_UI8:
		case VT_INT:
		case VT_UINT:
		case VT_LPSTR:
		case VT_LPWSTR:
		case VT_HRESULT:
		case VT_CLSID:
			{
			// Convert to a string
			VARIANT varTemp;
			V_VT(&varTemp)=VT_EMPTY;
			RETURNIFFAILED(VariantChangeTypeEx(&varTemp, pVar, LCID_XML, 0, VT_BSTR));
			bstrValue.Attach(V_BSTR(&varTemp));
			}
			break;

		case VT_DISPATCH:
		case VT_UNKNOWN:
			{
			// Check have object
			if (!V_UNKNOWN(pVar))	
				{
				bstrValue=L"null";
				break;
				}

			// QI for IPersist to get CLSID
			CComQIPtr<IPersist> spPersist(V_UNKNOWN(pVar));
			if (!spPersist)
				{
				if (FAILED(V_UNKNOWN(pVar)->QueryInterface(IID_IPersistStream, (void**)&spPersist)))
					if (FAILED(V_UNKNOWN(pVar)->QueryInterface(IID_IPersistStreamInit, (void**)&spPersist)))
						return E_NOINTERFACE;
				}

			// Get CLSID
			CLSID clsid;
			spPersist->GetClassID(&clsid);

			m_pArchive->EnterElement(L"PARAM");

			// Save name
			CComBSTR bstrName(pszPropName);
			XML_Item(*m_pArchive, L"NAME", bstrName);

			// Save clsid
			XML_Item(*m_pArchive, L"classid", clsid);
		
			// Save object
			XML_Element(*m_pArchive, V_UNKNOWN(pVar));

			m_pArchive->LeaveElement();
			return m_pArchive->GetResult();
			}
			break;

		default:
			// Unsupported variant type...
			return E_FAIL;
		}

	// Should have string by now...
	if (!bstrValue.m_str)
		return E_FAIL;

	// Enter param element
	m_pArchive->EnterElement(L"PARAM");

	// Save name and value
	CComBSTR bstrName(pszPropName);
	XML_Item(*m_pArchive, L"NAME", bstrName);
	XML_Item(*m_pArchive, L"VALUE", bstrValue);

	// Leave
	m_pArchive->LeaveElement();

	return m_pArchive->GetResult();
}


/////////////////////////////////////////////////////////////////////////////
// IPropertyBag2

/*
// Implementation of Read
STDMETHODIMP CPropertyBagOnXML::Read(ULONG cProperties, PROPBAG2* pPropBag, IErrorLog* pErrLog, VARIANT* pvarValue, HRESULT* phrError)
{
	return E_NOTIMPL;
}

// Implementation of Write
STDMETHODIMP CPropertyBagOnXML::Write(ULONG cProperties, PROPBAG2* pPropBag, VARIANT* pvarValue)
{
	return E_NOTIMPL;
}

// Implementation of CountProperties
STDMETHODIMP CPropertyBagOnXML::CountProperties(ULONG* pcProperties)
{
	return E_NOTIMPL;
}

// Implementation of GetPropertyInfo
STDMETHODIMP CPropertyBagOnXML::GetPropertyInfo(ULONG iProperty, ULONG cProperties, PROPBAG2* pPropBag, ULONG* pcProperties)
{
	return E_NOTIMPL;
}

// Implementation of LoadObject
STDMETHODIMP CPropertyBagOnXML::LoadObject(LPCOLESTR pstrName, DWORD dwHint, IUnknown* pUnkObject, IErrorLog* pErrLog)
{
	return E_NOTIMPL;
}

*/


/////////////////////////////////////////////////////////////////////////////
// IErrorLog

STDMETHODIMP CPropertyBagOnXML::AddError(LPCOLESTR pszPropName, EXCEPINFO *pExcepInfo)
{
	return S_OK;
}




}	// namespace Simple
