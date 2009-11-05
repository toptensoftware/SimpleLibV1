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
// XMLArchive.cpp - implementation of XMLArchive

#include "stdafx.h"
#include "SimpleLibAtlComBuild.h"

#include "XMLArchive.h"
#include "DispatchProperty.h"
#include "PropertyBagOnXML.h"
#include <float.h>

namespace Simple
{

//////////////////////////////////////////////////////////////////////
// IPersistXMLImpl

STDMETHODIMP IPersistXMLImpl::Save(IXMLDOMElement* pElement, IUnknown* pContext, DWORD dwFlags)
{
	// Check parameters
	if (!pElement)
		return E_POINTER;

	// Initialise archive
	CXMLArchive ar;
	ar.SetContext(pContext);
	RETURNIFFAILED(ar.InitSave(pElement, dwFlags));

	// Call Serialize
	return Serialize(ar);
}

STDMETHODIMP IPersistXMLImpl::Load(IXMLDOMElement* pElement, IUnknown* pContext, DWORD dwFlags)
{
	// Check parameters
	if (!pElement)
		return E_POINTER;

	// Initialise archive
	CXMLArchive ar;
	ar.SetContext(pContext);
	RETURNIFFAILED(ar.Load(pElement, dwFlags));

	// Call Serialize
	return Serialize(ar);
}


HRESULT IPersistXMLImpl::Load(IStream* pStream, LPCOLESTR pszDocumentElementName, DWORD dwFlags)
{
	CXMLArchive ar;
	RETURNIFFAILED(ar.Load(pStream, pszDocumentElementName, dwFlags));
	return Serialize(ar);
}

HRESULT IPersistXMLImpl::Save(IStream* pStream, LPCOLESTR pszDocumentElementName, DWORD dwFlags)
{
	// Create a new document
	CXMLArchive ar;
	ar.InitNewDocument(pszDocumentElementName, dwFlags);

	// Serialize
	RETURNIFFAILED(Serialize(ar));

	// Write to a stream
	return ar.Save(pStream);
}



//////////////////////////////////////////////////////////////////////
// CXMLElementTarget

CXMLElementTarget::CXMLElementTarget(CXMLArchive& ar) :
	m_ar(ar)
{
};

BOOL CXMLElementTarget::IsStoring() 
{ 
	return m_ar.IsStoring(); 
};

BOOL CXMLElementTarget::IsLoading() 
{ 
	return m_ar.IsLoading(); 
};

HRESULT CXMLElementTarget::ExchangeAttribute(LPCOLESTR pszName, CComBSTR& bstrValue, LPCOLESTR pszDefault)
{
	if (pszDefault && m_ar.IsStoring() && !m_ar.SaveDefaults() && IsEqualString(bstrValue, pszDefault))
		return S_OK;
	return m_ar.ExchangeElement(pszName, bstrValue, pszDefault);
}

HRESULT CXMLElementTarget::SetResult(HRESULT hr)
{
	return m_ar.SetResult(hr);
}

void CXMLElementTarget::ClearError()
{
	m_ar.ClearError();
}

bool CXMLElementTarget::SaveDefaults()
{
	return m_ar.SaveDefaults();
}




//////////////////////////////////////////////////////////////////////
// CXMLArchive

// Constructor
CXMLArchive::CXMLArchive() :
	m_Elements(*this)
{
	m_hrResult=S_OK;
	m_bStoring=FALSE;
	m_dwFlags=0;
	m_iExternalNesting=0;
	m_pContext = NULL;
}

// Destructor
CXMLArchive::~CXMLArchive()
{
}

HRESULT CXMLArchive::InitNewDocument(LPCOLESTR pszDocumentElementName, DWORD dwFlags)
{
	// Create a new DOM document
	CComPtr<IXMLDOMDocument> spDoc;
	RETURNIFFAILED(spDoc.CoCreateInstance(L"MSXML.DomDocument"));

	// Create document element
	CComPtr<IXMLDOMElement> spDocElement;
	RETURNIFFAILED(spDoc->createElement(CComBSTR(pszDocumentElementName), &spDocElement));

	// Get the document element
	RETURNIFFAILED(spDoc->putref_documentElement(spDocElement));

	// Initialise for save
	RETURNIFFAILED(InitSave(spDocElement, dwFlags));
	return S_OK;
}

HRESULT CXMLArchive::Load(IXMLDOMElement* pNode, DWORD dwFlags)
{
	ASSERT(pNode!=NULL);

	// Clear node stack, store new node...
	m_ElementStack.RemoveAll();
	m_ElementStack.Add(pNode);

	m_bStoring=FALSE;
	m_dwFlags=dwFlags;

	return S_OK;
}

HRESULT CXMLArchive::InitLoad(IXMLDOMDocument* pDocument, DWORD dwFlags, VARIANT_BOOL bOK, LPCOLESTR pszElementName)
{
	// Check it loaded OK
	if (!bOK)
		return XMLARCHIVE_E_PARSEERROR;

	// Get document element
	CComPtr<IXMLDOMElement> spDocElement;
	RETURNIFFAILED(pDocument->get_documentElement(&spDocElement));

	CComPtr<IXMLDOMElement> spRootElement;
	
	if (pszElementName)
		{
		CComPtr<IXMLDOMNode> spDocParent;
		spDocElement->get_parentNode(&spDocParent);

		// Select the root node...
		CComPtr<IXMLDOMNode> spNode;
		RETURNIFFAILED(spDocParent->selectSingleNode(CComBSTR(pszElementName), &spNode));

		// Query for IXMLDOMElement
		CComQIPtr<IXMLDOMElement> spElement(spNode);
		if (!spElement)
			return XMLARCHIVE_E_PARSEERROR;

		spRootElement=spElement;
		}
	else
		spRootElement=spDocElement;

	// Initialise for load
	return Load(spRootElement, dwFlags);
}

HRESULT CXMLArchive::LoadXML(BSTR bstrXML, LPCOLESTR pszElementName, DWORD dwFlags)
{
	// Create a new DOM document
	CComPtr<IXMLDOMDocument> spDoc;
	RETURNIFFAILED(spDoc.CoCreateInstance(L"MSXML.DomDocument"));

	// Set synchronous mode
	spDoc->put_async(VARIANT_FALSE);

	// Load the XML
	VARIANT_BOOL bOK;
	RETURNIFFAILED(spDoc->loadXML(bstrXML, &bOK));

	return InitLoad(spDoc, dwFlags, bOK, pszElementName);

}

HRESULT CXMLArchive::LoadFile(BSTR bstrFileName, LPCOLESTR pszElementName, DWORD dwFlags)
{
	// Create a new DOM document
	CComPtr<IXMLDOMDocument> spDoc;
	RETURNIFFAILED(spDoc.CoCreateInstance(L"MSXML.DomDocument"));

	// Set synchronous mode
	spDoc->put_async(VARIANT_FALSE);

	// Load the XML
	VARIANT_BOOL bOK;
	RETURNIFFAILED(spDoc->load(CComVariant(bstrFileName), &bOK));

	return InitLoad(spDoc, dwFlags, bOK, pszElementName);
}

HRESULT CXMLArchive::InitSave(IXMLDOMElement* pNode, DWORD dwFlags)
{
	m_bStoring=TRUE;
	m_dwFlags=dwFlags;

	// Clear node stack, store new node...
	m_ElementStack.RemoveAll();
	m_ElementStack.Add(pNode);

	m_iExternalNesting=-1;

	CComPtr<IXMLDOMNode> spNode(pNode);
	while (TRUE)
		{
		// Get parent
		CComPtr<IXMLDOMNode> spParentNode;
		spNode->get_parentNode(&spParentNode);

		if (!spParentNode)
			break;

		m_iExternalNesting++;

		spNode=spParentNode;
		}

	if (m_iExternalNesting<0)
		m_iExternalNesting=0;

	return S_OK;
}

HRESULT CXMLArchive::get_XML(BSTR* pVal)
{
	RETURNIFFAILED(FinishSave());
	return m_ElementStack[0]->get_xml(pVal);
}

HRESULT CXMLArchive::Save(BSTR FileName)
{
	RETURNIFFAILED(FinishSave());

	if (m_ElementStack.GetSize()==0)
		return E_UNEXPECTED;

	AppendTabs(TRUE, CurrentDepth()-1);

	return GetDocument()->save(CComVariant(FileName));
}

HRESULT CXMLArchive::Load(IStream* pStream, LPCOLESTR pszElementName, DWORD dwFlags)
{
	CComBSTR bstr;
	RETURNIFFAILED(bstr.ReadFromStream(pStream));

	return LoadXML(bstr, pszElementName, dwFlags);
	
}

HRESULT CXMLArchive::Save(IStream* pStream)
{
	RETURNIFFAILED(FinishSave());

	CComBSTR bstr;
	m_ElementStack[0]->get_xml(&bstr);

	return bstr.WriteToStream(pStream);
}

HRESULT CXMLArchive::FinishSave()
{
	RETURNIFFAILED(m_hrResult);

	if (m_ElementStack.GetSize()==0)
		return E_UNEXPECTED;

	AppendTabs(TRUE, CurrentDepth()-1);

	return S_OK;
}


// Implementation of EnterElement
HRESULT CXMLArchive::EnterElement(LPCOLESTR pszName, VARIANT_BOOL bOptional)
{
	// Quit if something already failed...
	RETURNIFFAILED(m_hrResult);

	// Get current element
	IXMLDOMElement* pElement=CurrentElement();

	// Check have a current element
	if (!pElement)
		return SetResult(E_UNEXPECTED);

	// Storing?
	if (m_bStoring)
		{
		// Indent to next line
		AppendTabs(TRUE, CurrentDepth());

		// ...create a new element node
		CComPtr<IXMLDOMElement> spElement;
		RETURNIFFAILED(SetResult(GetDocument()->createElement(CComBSTR(pszName), &spElement)));

		// Append it to the current element
		RETURNIFFAILED(SetResult(pElement->appendChild(spElement, NULL)));

		// Add it to the stack
		m_ElementStack.Add(spElement);
		}
	else
		{
		// Open child element
		CComPtr<IXMLDOMElement> spElement;
		HRESULT hr=OpenChildElement(pszName, bOptional, &spElement);

		// Add it to the stack (unless optional and not found)
		if (hr==S_OK)
			{
			ASSERT(spElement!=NULL);
			m_ElementStack.Add(spElement);
			}

		// Done!
		return hr;
		}

	// Finished
	return S_OK;
}

// Enter a element
HRESULT CXMLArchive::EnterElement(IXMLDOMElement* pElement)
{
	ASSERT(IsLoading());
	m_ElementStack.Add(pElement);
	return S_OK;
}

// Implementation of LeaveElement
HRESULT CXMLArchive::LeaveElement()
{
	// Check can leave
	if (m_ElementStack.GetSize()<2)
		return E_UNEXPECTED;

	if (IsStoring())
		{
		VARIANT_BOOL bHasKids;
		CurrentElement()->hasChildNodes(&bHasKids);

		if (bHasKids)
			AppendTabs(TRUE, CurrentDepth()-1);
		}

	// Remove element from stack
	m_ElementStack.RemoveAt(m_ElementStack.GetSize()-1);

	// Finished
	return S_OK;
}

// Return reference to element on top of stack
IXMLDOMElement*	CXMLArchive::CurrentElement()
{
	ASSERT(m_ElementStack.GetSize()>0);
	if (m_ElementStack.GetSize()==0)
		return NULL;
	else
		return m_ElementStack[m_ElementStack.GetSize()-1];
}


// Implementation of ExchangeAttribute
HRESULT CXMLArchive::ExchangeAttribute(LPCOLESTR pszName, CComBSTR& bstrValue, LPCOLESTR pszDefault)
{
	RETURNIFFAILED(m_hrResult);

	if (pszDefault && IsStoring() && !SaveDefaults() && IsEqualString(bstrValue, pszDefault))
		return S_OK;

	if (IsStoring())
		{
		// Setup a temp variant
		VARIANT varTemp;
		V_VT(&varTemp)=VT_BSTR;
		V_BSTR(&varTemp)=bstrValue.m_str;

		// Set the attribute
		return SetResult(CurrentElement()->setAttribute(CComBSTR(pszName), varTemp));
		}
	else
		{
		// 
		CComVariant varValue;
		HRESULT hr=CurrentElement()->getAttribute(CComBSTR(pszName), &varValue);
		if (hr==S_FALSE)
			{
			if (pszDefault)
				{
				bstrValue=pszDefault;
				return S_FALSE;
				}
		
			CComBSTR bstr(L"Required attribute missing - \'");
			bstr.Append(pszName);
			bstr.Append(L"'");
			return LoadError(bstr, XMLARCHIVE_E_MISSINGATTRIBUTE);
			}

		// Setup return value
		bstrValue=V_BSTR(&varValue);
		return S_OK;
		}
}


// Implementation of ExchangeElement
HRESULT CXMLArchive::ExchangeElement(LPCOLESTR pszName, CComBSTR& bstrValue, LPCOLESTR pszDefault)
{
	RETURNIFFAILED(m_hrResult);

	if (pszDefault && IsStoring() && !SaveDefaults() && IsEqualString(bstrValue, pszDefault))
		return S_OK;

	if (m_bStoring)
		{
		// Create text node
		CComPtr<IXMLDOMText> spTextNode;
		RETURNIFFAILED(GetDocument()->createTextNode(bstrValue, &spTextNode));

		// Create a new element node
		CComPtr<IXMLDOMElement> spElement;
		RETURNIFFAILED(GetDocument()->createElement(CComBSTR(pszName), &spElement));

		AppendTabs(TRUE, CurrentDepth());

		// Set text node to element node
		RETURNIFFAILED(spElement->appendChild(spTextNode, NULL));

		// Append it to the current element
		RETURNIFFAILED(CurrentElement()->appendChild(spElement, NULL));

		// Get out
		return S_OK;
		}
	else
		{
		// Open child element
		CComPtr<IXMLDOMElement> spElement;
		HRESULT hr=OpenChildElement(pszName, pszDefault!=NULL, &spElement);

		if ((hr==S_FALSE) && (pszDefault!=NULL))
			{
			bstrValue=pszDefault;
			return S_FALSE;
			}

		// Quit if failed or optional and not found
		if (hr!=S_OK)
			return hr;

		// Get value of element
		CComVariant varValue;
		RETURNIFFAILED(SetResult(spElement->get_nodeTypedValue(&varValue)));

		ASSERT(V_VT(&varValue)==VT_BSTR);
		bstrValue=V_BSTR(&varValue);
		return S_OK;
		}
}

IXMLDOMDocument* CXMLArchive::GetDocument()
{
	m_spDoc.Release();
	CurrentElement()->get_ownerDocument(&m_spDoc);
	return m_spDoc;
}

HRESULT CXMLArchive::AppendText(LPCOLESTR psz)
{
	ASSERT(m_bStoring);

	// Create text node
	CComPtr<IXMLDOMText> spTextNode;
	RETURNIFFAILED(GetDocument()->createTextNode(CComBSTR(psz), &spTextNode));

	// Append text node
	return CurrentElement()->appendChild(spTextNode, NULL);
}

HRESULT CXMLArchive::AppendTabs(BOOL bCrLfFirst, int iCount)
{
	OLECHAR* psz=(OLECHAR*)_alloca(sizeof(OLECHAR) * (iCount+5));
	OLECHAR* p=psz;

	if (bCrLfFirst)
		{
		memcpy(p, L"\r\n", 2 * sizeof(OLECHAR));
		p+=2;
		}

	for (int i=0; i<iCount; i++)
		{
		(*p)=L'\t';
		p++;
		}

	(*p)=L'\0';

	// Append it...
	return AppendText(psz);
}

// Work out the depth of the current element
int CXMLArchive::CurrentDepth()
{
	return m_ElementStack.GetSize() + m_iExternalNesting;
}


HRESULT CXMLArchive::SetResult(HRESULT hr)
{
	if (hr==S_OK)
		return hr;

	if (m_hrResult==S_OK)
		m_hrResult=hr;

	return m_hrResult;
}

void CXMLArchive::ClearError()
{
	m_hrResult=S_OK;
};

HRESULT CXMLArchive::LoadError(LPCOLESTR psz, HRESULT hr)
{
	// Ignore subsequent errors
	if (m_hrResult!=S_OK)
		return m_hrResult;

	// Format error string
	CComBSTR bstr(L"Load error - ");
	bstr.Append(psz);

	return SetResult(hr);
}

// Open child element
HRESULT CXMLArchive::OpenChildElement(LPCOLESTR pszName, BOOL bOptional, IXMLDOMElement** ppElement)
{

	// Open first child element with specified name
	CComQIPtr<IXMLDOMNode> spNode;
	HRESULT hr=CurrentElement()->selectSingleNode(CComBSTR(pszName), &spNode);

	// Not found?
	if (hr==S_FALSE)
		{
		// Quit if element was optional
		if (bOptional)
			return S_FALSE;

		// Setup an error
		CComBSTR bstr(L"Required element missing - <");
		bstr.Append(pszName);
		bstr.Append(L">");
		return LoadError(bstr, XMLARCHIVE_E_MISSINGATTRIBUTE);
		}

	// Quit if failed
	RETURNIFFAILED(SetResult(hr));

	// Query for IXMLDOMElement
	CComQIPtr<IXMLDOMElement> spElement(spNode);
	if (!spNode)
		return SetResult(E_NOINTERFACE);

	return spElement.CopyTo(ppElement);
}

HRESULT CXMLArchive::SelectElements(LPCOLESTR pszPattern, CXMLDOMElements& Elements)
{
	ASSERT(IsLoading());

	// Quit if already failed...
	RETURNIFFAILED(m_hrResult);

	// Empty the list
	Elements.RemoveAll();

	// Select the nodes
	CComPtr<IXMLDOMNodeList> spNodeList;
	RETURNIFFAILED(CurrentElement()->selectNodes(CComBSTR(pszPattern), &spNodeList));

	return InitElementsVector(spNodeList, Elements);
}


// Get all child elements
HRESULT CXMLArchive::GetChildElements(CXMLDOMElements& Elements)
{
	ASSERT(IsLoading());

	// Quit if already failed...
	RETURNIFFAILED(m_hrResult);

	// Empty the list
	Elements.RemoveAll();

	// Select the nodes
	CComPtr<IXMLDOMNodeList> spNodeList;
	RETURNIFFAILED(CurrentElement()->get_childNodes(&spNodeList));

	return InitElementsVector(spNodeList, Elements);
}


HRESULT CXMLArchive::InitElementsVector(IXMLDOMNodeList* pNodeList, CXMLDOMElements& Elements)
{
	ASSERT(IsLoading());

	// Get count
	long lCount;
	pNodeList->get_length(&lCount);

	// Get each and add to Elements
	for (long i=0; i<lCount; i++)
		{
		// Get node interface
		CComPtr<IXMLDOMNode> spNode;
		if (SUCCEEDED(pNodeList->get_item(i, &spNode)))
			{
			// Query for element interface
			CComQIPtr<IXMLDOMElement> spElement(spNode);

			// Debug warning about non-element match...
			#ifdef _DEBUG
			if (!spElement)
				{
				ATLTRACE(_T("WARNING: Non-element node found in SelectElements!\n"));
				}
			#endif

			// Add to collection
			if (spElement)
				Elements.Add(spElement);
			}
		}

	// Done!
	return S_OK;
}

HRESULT CXMLArchive::SetContext(IUnknown* pUnk)
{
	m_pContext = pUnk;
	return S_OK;
}

HRESULT CXMLArchive::GetContext(IUnknown** pRetVal)
{
	ASSERT(pRetVal!=NULL);
	*pRetVal = m_pContext;
	if (*pRetVal)
		(*pRetVal)->AddRef();
	return S_OK;
}

//////////////////////////////////////////////////////////////////////
// XML_* helper function

bool BoolFromString(LPCOLESTR psz)
{
	if (_wcsicmp(psz, L"true")==0)	return true;
	if (_wcsicmp(psz, L"yes")==0)		return true;
	if (_wcsicmp(psz, L"-1")==0)		return true;
	if (_wcsicmp(psz, L"1")==0)		return true;
	return false;
}

LPCOLESTR StringFromBool(bool bValue)
{
	return bValue ? L"true" : L"false";
}

HRESULT XML_ItemHelper(CXMLTarget& ar, LPCOLESTR pszName, bool& bValue, bool bDefault, bool bOptional)
{
	CComBSTR bstr;

	if (ar.IsStoring())
		{
		if (!ar.SaveDefaults() && bValue==bDefault && bOptional)
			return S_OK;

		bstr=StringFromBool(bValue);
		}

	HRESULT hr=ar.ExchangeAttribute(pszName, bstr, bOptional ? L"" : NULL);
	if (FAILED(hr)) return hr;

	if (ar.IsLoading())
		{
		if (hr==S_FALSE)
			{
			bValue=bDefault;
			}
		else
			{
			bValue=BoolFromString(bstr);
			}
		}

	return S_OK;
}

HRESULT SIMPLEAPI XML_Item(CXMLTarget& ar, LPCOLESTR pszName, bool& bValue)
{
	return XML_ItemHelper(ar, pszName, bValue, false, false);
}

HRESULT SIMPLEAPI XML_Item(CXMLTarget& ar, LPCOLESTR pszName, bool& bValue, bool bDefault)
{
	return XML_ItemHelper(ar, pszName, bValue, bDefault, true);
}

HRESULT SIMPLEAPI XML_Item(CXMLTarget& ar, LPCOLESTR pszName, long& iValue)
{
	CComBSTR bstr;

	if (ar.IsStoring())
		{
		VarBstrFromInt(iValue, LCID_XML, 0, &bstr);
		}

	RETURNIFFAILED(ar.ExchangeAttribute(pszName, bstr, NULL));

	if (ar.IsLoading())
		{
		VarIntFromStr(bstr, LCID_XML, 0, &iValue);
		}

	return S_OK;
}

HRESULT SIMPLEAPI XML_Item(CXMLTarget& ar, LPCOLESTR pszName, long& iValue, long iDefault)
{
	CComBSTR bstr;

	if (ar.IsStoring())
		{
		if (iValue==iDefault && !ar.SaveDefaults())
			return S_OK;

		VarBstrFromInt(iValue, LCID_XML, 0,  &bstr);
		}

	HRESULT hr=ar.ExchangeAttribute(pszName, bstr, L"");
	RETURNIFFAILED(hr);

	if (ar.IsLoading())
		{
		if (hr==S_FALSE)
			{
			iValue=iDefault;
			return S_FALSE;
			}
		else
			{
			VarIntFromStr(bstr, LCID_XML, 0,  &iValue);
			}
		}

	return hr;
}

HRESULT SIMPLEAPI XML_Item(CXMLTarget& ar, LPCOLESTR pszName, unsigned long& iValue)
{
	CComBSTR bstr;

	if (ar.IsStoring())
		{
		VarBstrFromUI4(iValue, LCID_XML, 0,  &bstr);
		}

	RETURNIFFAILED(ar.ExchangeAttribute(pszName, bstr, NULL));

	if (ar.IsLoading())
		{
		VarUI4FromStr(bstr, LCID_XML, 0,  &iValue);
		}

	return S_OK;
}

HRESULT SIMPLEAPI XML_Item(CXMLTarget& ar, LPCOLESTR pszName, unsigned long& iValue, unsigned long iDefault)
{
	CComBSTR bstr;

	if (ar.IsStoring())
		{
		if (iValue==iDefault && !ar.SaveDefaults())
			return S_OK;

		VarBstrFromUI4(iValue, LCID_XML, 0,  &bstr);
		}

	HRESULT hr=ar.ExchangeAttribute(pszName, bstr, L"");
	RETURNIFFAILED(hr);

	if (ar.IsLoading())
		{
		if (hr==S_FALSE)
			{
			iValue=iDefault;
			return S_FALSE;
			}
		else
			{
			VarUI4FromStr(bstr, LCID_XML, 0,  &iValue);
			}
		}

	return hr;
}


HRESULT SIMPLEAPI XML_Item(CXMLTarget& ar, LPCOLESTR pszName, double& dblValue)
{
	CComBSTR bstr;

	if (ar.IsStoring())
		{
		VarBstrFromR8(dblValue, LCID_XML, 0,  &bstr);
		}

	RETURNIFFAILED(ar.ExchangeAttribute(pszName, bstr, NULL));

	if (ar.IsLoading())
		{
		VarR8FromStr(bstr, LCID_XML, 0,  &dblValue);
		}

	return S_OK;
}

HRESULT SIMPLEAPI XML_Item(CXMLTarget& ar, LPCOLESTR pszName, double& dblValue, double dblDefault)
{
	CComBSTR bstr;

	if (ar.IsStoring())
		{
		if (dblValue==dblDefault && _isnan(dblValue)==_isnan(dblDefault) && !ar.SaveDefaults() )
			return S_OK;

		VarBstrFromR8(dblValue, LCID_XML, 0,  &bstr);
		}

	HRESULT hr=ar.ExchangeAttribute(pszName, bstr, L"");
	RETURNIFFAILED(hr);

	if (ar.IsLoading())
		{
		if (hr==S_FALSE)
			{
			dblValue=dblDefault;
			return S_FALSE;
			}
		else
			{
			VarR8FromStr(bstr, LCID_XML, 0,  &dblValue);
			}
		}

	return hr;
}


HRESULT SIMPLEAPI XML_Item(CXMLTarget& ar, LPCOLESTR pszName, VARIANT* pvarValue, VARTYPE vt, VARIANT* pvarDefault)
{
USES_CONVERSION;

	CComBSTR bstr;

	if (ar.IsStoring())
		{
		if (!ar.SaveDefaults() && pvarDefault)
			{
			if (VarCmp(pvarValue, pvarDefault, LCID_XML)==VARCMP_EQ )
				return S_OK;
			}

		if (V_VT(pvarValue)!=VT_BOOL)
			{
			VARIANT varTemp;
			VariantInit(&varTemp);
			RETURNIFFAILED(VariantChangeTypeEx(&varTemp, pvarValue, LCID_XML, 0,  VT_BSTR));
			bstr.Attach(V_BSTR(&varTemp));
			}
		else
			{
			bstr=StringFromBool(!!V_BOOL(pvarValue));
			}
		}

	HRESULT hr=ar.ExchangeAttribute(pszName, bstr, pvarDefault ? L"" : NULL);
	RETURNIFFAILED(hr);

	if (ar.IsLoading())
		{
		if (hr==S_FALSE)
			{
			return VariantChangeTypeEx(pvarValue, pvarDefault, LCID_XML, 0,  vt);
			}
		else
			{
			if (vt!=VT_BOOL)
				{
				VARIANT varTemp;
				V_VT(&varTemp)=VT_BSTR;
				V_BSTR(&varTemp)=bstr;

				return VariantChangeTypeEx(pvarValue, &varTemp, LCID_XML, 0,  vt);
				}
			else
				{
				V_VT(pvarValue)=VT_BOOL;
				V_BOOL(pvarValue)=BoolFromString(bstr) ? VARIANT_TRUE : VARIANT_FALSE;
				}
			}
		}

	return hr;
}

HRESULT SIMPLEAPI XML_Item(CXMLTarget& ar, LPCOLESTR pszName, IDispatch* pObject, DISPID dispid, VARTYPE vt, VARIANT* pvarDefault)
{
	CComVariant varTemp;

	if (ar.IsStoring())
		{
		RETURNIFFAILED(SlxGetProperty(pObject, dispid, &varTemp));
		}

	RETURNIFFAILED(XML_Item(ar, pszName, &varTemp, vt, pvarDefault));

	if (ar.IsLoading())
		{
		RETURNIFFAILED(SlxPutProperty(pObject, dispid, varTemp));
		}

	return S_OK;
}


HRESULT SIMPLEAPI XML_Item(CXMLTarget& ar, LPCOLESTR psz, CLSID& clsid)
{
	CComBSTR bstr;

	// Convert to string
	if (ar.IsStoring())
		{
		OLECHAR szTemp[MAX_PATH];
		StringFromGUID2(clsid, szTemp, MAX_PATH);
		bstr=szTemp;
		}

	// Exchange it
	HRESULT hr=XML_Item(ar, psz, bstr, L"");
	if (hr==S_FALSE)
		return S_FALSE;

	if (SUCCEEDED(hr))
		{
		if (FAILED(CLSIDFromString(bstr, &clsid)))
			{
			return ar.SetResult(E_FAIL);
			}
		}

	return S_OK;
};


HRESULT SIMPLEAPI XML_EnumItemHelper(CXMLTarget& ar, LPCOLESTR pszName, unsigned int& iValue, XMLENUMENTRY* pEntries, unsigned int iDefault, bool bOptional)
{
	CComBSTR bstr;

	if (ar.IsStoring())
		{
		if (iValue==iDefault && bOptional && !ar.SaveDefaults())
			return S_OK;

		XMLENUMENTRY* pEntry;
		for (pEntry=pEntries; pEntry->pszName!=NULL; pEntry++)
			{
			if (pEntry->iValue==iValue)
				{
				bstr=pEntry->pszName;
				break;
				}
			}

		if (!pEntry->pszName)
			{
			ASSERT(FALSE);
			return E_UNEXPECTED;
			}
		}

	HRESULT hr=ar.ExchangeAttribute(pszName, bstr, bOptional ? L"" : NULL);
	RETURNIFFAILED(hr);

	if (ar.IsLoading())
		{
		if (hr==S_FALSE)
			{
			iValue=iDefault;
			return S_OK;
			}

		for (XMLENUMENTRY* pEntry=pEntries; pEntry->pszName!=NULL; pEntry++)
			{
			if (_wcsicmp(pEntry->pszName, bstr)==0)
				{
				iValue=pEntry->iValue;
				return S_OK;
				}
			}

		iValue=iDefault;
		}

	return hr;
};

HRESULT SIMPLEAPI XML_EnumItemInt(CXMLTarget& ar, LPCOLESTR pszName, unsigned int& iValue, XMLENUMENTRY* pEntries)
{
	return XML_EnumItemHelper(ar, pszName, iValue, pEntries, 0, false);
}

HRESULT SIMPLEAPI XML_EnumItemInt(CXMLTarget& ar, LPCOLESTR pszName, unsigned int& iValue, XMLENUMENTRY* pEntries, unsigned int iDefault)
{
	return XML_EnumItemHelper(ar, pszName, iValue, pEntries, iDefault, true);
}

HRESULT SIMPLEAPI XML_Item(CXMLTarget& ar, LPCOLESTR pszName, IDispatch* pObject, DISPID dispid, XMLENUMENTRY* pEntries)
{
	CComVariant varTemp;
	if (ar.IsStoring())
		{
		RETURNIFFAILED(SlxGetProperty(pObject, dispid, &varTemp));
		RETURNIFFAILED(varTemp.ChangeType(VT_UINT));
		}

	RETURNIFFAILED(XML_EnumItemHelper(ar, pszName, V_UINT(&varTemp), pEntries, 0, false));

	if (ar.IsLoading())
		{
		RETURNIFFAILED(SlxPutProperty(pObject, dispid, varTemp));
		}

	return S_OK;
}

HRESULT SIMPLEAPI XML_Item(CXMLTarget& ar, LPCOLESTR pszName, IDispatch* pObject, DISPID dispid, XMLENUMENTRY* pEntries, unsigned int iDefault)
{
	CComVariant varTemp;
	if (ar.IsStoring())
		{
		RETURNIFFAILED(SlxGetProperty(pObject, dispid, &varTemp));
		RETURNIFFAILED(varTemp.ChangeType(VT_UINT));
		}

	RETURNIFFAILED(XML_EnumItemHelper(ar, pszName, V_UINT(&varTemp), pEntries, iDefault, true));

	if (ar.IsLoading())
		{
		V_VT(&varTemp)=VT_UINT;
		RETURNIFFAILED(SlxPutProperty(pObject, dispid, varTemp));
		}

	return S_OK;
}

HRESULT SIMPLEAPI XML_Element(CXMLArchive& ar, IUnknown* pObject)
{
	// Query the object for IPersistXML
	CComQIPtr<IPersistXML> spPersistXML(pObject);
	if (spPersistXML)
		{
		HRESULT hr = S_OK;

		CComPtr<IUnknown> spContext;
		ar.GetContext(&spContext);

		if (ar.IsLoading())
			hr=ar.SetResult(spPersistXML->Load(ar.CurrentElement(), spContext, ar.GetFlags()));

		if (ar.IsStoring())
			hr=ar.SetResult(spPersistXML->Save(ar.CurrentElement(), spContext, ar.GetFlags()));

		return hr;
		}

#ifdef PROPERTYBAGONXML_SUPPORT
	// Query the object for IPersistPropertyBag
	CComQIPtr<IPersistPropertyBag> spPersistPropertyBag(pObject);
	if (spPersistPropertyBag)
		{
		// Setup a propertybag on XML Object
		CPropertyBagOnXML pb;

		// Initialise the property bag
		RETURNIFFAILED(pb.Init(&ar));

		if (ar.IsLoading())
			{
			if (ar.GetFlags() & PXF_INITNEW)
				{
				ar.SetResult(spPersistPropertyBag->InitNew());
				}

			if (SUCCEEDED(ar.GetResult()))
				{
				ar.SetResult(spPersistPropertyBag->Load(&pb, &pb));
				}
			}

		if (ar.IsStoring())
			ar.SetResult(spPersistPropertyBag->Save(&pb, (ar.GetFlags() & PXF_CLEARDIRTY)!=0, FALSE));

		return ar.GetResult();
		}

#endif

	// Query for IPersistStreamInit
	CComQIPtr<IPersistStream> spPersistStream(pObject);
	if (!spPersistStream)
		{
		pObject->QueryInterface(IID_IPersistStreamInit, (void**)&spPersistStream.p);
		}
	if (spPersistStream)
		{
		CUniString str;
		if (ar.IsStoring())
			{
			// Create a stream
			CComPtr<IStream> spStream;
			RETURNIFFAILED(ar.SetResult(CreateStreamOnHGlobal(NULL, TRUE, &spStream)));

			// Save the object
			RETURNIFFAILED(ar.SetResult(spPersistStream->Save(spStream, (ar.GetFlags() & PXF_CLEARDIRTY)!=0)));

			// Get length of stream
			STATSTG stat;
			spStream->Stat(&stat, STATFLAG_NONAME);

			LPBYTE pTempBuffer=(LPBYTE)malloc((int)stat.cbSize.QuadPart);

			// Back to start
			LARGE_INTEGER li={0};
			spStream->Seek(li, STREAM_SEEK_SET, NULL);

			// read the entire string (put at end of buffer)
			spStream->Read(pTempBuffer, (int)stat.cbSize.QuadPart, NULL);

			// Format binary data
			str=FormatBinaryData(pTempBuffer, stat.cbSize.LowPart, 0);

			free(pTempBuffer);
			}

		// Exchange binary data
		if (XML_Item(ar, L"binary", str, L"")==S_OK)
			{
			if (ar.IsLoading())
				{
				// Create a stream
				CComPtr<IStream> spStream;
				RETURNIFFAILED(ar.SetResult(CreateStreamOnHGlobal(NULL, TRUE, &spStream)));

				// Read binary data
				unsigned char* pData;
				size_t cbData;
				if (UnformatBinaryData(str, (void**)&pData, &cbData))
					{
					// Write it to stream...
					spStream->Write(pData, (ULONG)cbData, NULL);
					free(pData);
					}

				// Back to start
				LARGE_INTEGER li={0};
				spStream->Seek(li, STREAM_SEEK_SET, NULL);

				// Have the object load
				RETURNIFFAILED(ar.SetResult(spPersistStream->Load(spStream)));
				}
			}
		}

	return S_OK;
}

// Serialize an object by querying it for IPersistXML
HRESULT SIMPLEAPI XML_Element(CXMLArchive& ar, LPCOLESTR pszName, IUnknown* pObject, VARIANT_BOOL bOptional)
{
	if (!pObject)
		return E_POINTER;

	// Enter element
	HRESULT hr = ar.EnterElement(pszName, bOptional);
	if(hr != S_OK)
		return hr;

	// Serialize the element
	hr=XML_Element(ar, pObject);

	// Leave element
	ar.LeaveElement();

	// Done
	return hr;
}

HRESULT SIMPLEAPI XML_Element(CXMLArchive& ar, IXMLDOMElement* pElement, IUnknown* pObject)
{
	if (!pObject)
		return E_POINTER;
	ASSERT(ar.IsLoading());

	// Query the object for IPersistXML
	CComQIPtr<IPersistXML> spPersistXML(pObject);
	if (!spPersistXML) return E_NOINTERFACE;

	CComPtr<IUnknown> spContext;
	ar.GetContext(&spContext);

	// Load/merge it
	return ar.SetResult(spPersistXML->Load(pElement, spContext, ar.GetFlags()));
}

HRESULT SIMPLEAPI XML_EmbeddedXML(CXMLArchive& ar, LPCOLESTR pszName, CComBSTR& bstrXML, LPCOLESTR psz)
{
	if (ar.IsStoring())
		{
		CComBSTR bstrTemp;
		bstrTemp=L"<";
		bstrTemp.Append(pszName);
		bstrTemp.Append(L">");
		bstrTemp.Append(bstrXML);
		bstrTemp.Append(L"</");
		bstrTemp.Append(pszName);
		bstrTemp.Append(L">");

		CXMLArchive arTemp;
		RETURNIFFAILED(ar.SetResult(arTemp.LoadXML(bstrTemp, pszName, PXF_ALL)));

		// Clone it
		CComPtr<IXMLDOMNode> spNode;
		arTemp.CurrentElement()->cloneNode(VARIANT_TRUE, &spNode);

#ifdef _DEBUG
		CComBSTR bstrX;
		spNode->get_xml(&bstrX);
#endif

		// Add to main document
		RETURNIFFAILED(ar.SetResult(ar.CurrentElement()->appendChild(spNode, NULL)));
		}

	if (ar.IsLoading())
		{
		HRESULT hr=ar.EnterElement(pszName, psz!=NULL);
		if (hr==S_FALSE)
			{
			bstrXML=psz;
			return S_OK;
			}

		// Get sub nodes		
		CComPtr<IXMLDOMNodeList> spSubNodes;
		ar.CurrentElement()->get_childNodes(&spSubNodes);

#ifdef _DEBUG
		CComBSTR bstrX;
		ar.CurrentElement()->get_xml(&bstrX);
#endif

		long lSubNodes=0;
		spSubNodes->get_length(&lSubNodes);

		bstrXML=L"";
		for (long i=0; i<lSubNodes; i++)
			{
			CComPtr<IXMLDOMNode> spSubNode;
			spSubNodes->get_item(i, &spSubNode);

			CComBSTR bstrTemp;
			spSubNode->get_xml(&bstrTemp);

			bstrXML.Append(bstrTemp);
			}

		ar.LeaveElement();
		}

	return ar.GetResult();
}

HRESULT SIMPLEAPI SlxSaveObject(IUnknown* pObject, BSTR* pVal, LPCOLESTR pszDocElem, DWORD dwFlags)
{
	// Query the object for IPersistXML
	CComQIPtr<IPersistXML> spPersistXML(pObject);
	if (spPersistXML)
		{
		// Create an archive
		CXMLArchive ar;
		ar.InitNewDocument(pszDocElem, dwFlags);

		// Save it
		RETURNIFFAILED(spPersistXML->Save(ar.CurrentElement(), NULL, ar.GetFlags()));

		// Return string
		return ar.get_XML(pVal);
		}

	// Query the object for IPersistPropertyBag
	CComQIPtr<IPersistPropertyBag> spPersistPropertyBag(pObject);
	if (spPersistPropertyBag)
		{
		// Setup a propertybag on XML Object
		CPropertyBagOnXML pb;

		// Create an archive
		CXMLArchive ar;
		ar.InitNewDocument(pszDocElem, dwFlags);

		// Initialise the property bag
		RETURNIFFAILED(pb.Init(&ar));

		ar.EnterElement(L"PropertyBag");
	
		// Save it
		RETURNIFFAILED(spPersistPropertyBag->Save(&pb, (ar.GetFlags() & PXF_CLEARDIRTY)!=0, FALSE));

		ar.LeaveElement();

		return ar.get_XML(pVal);
		}

	// Query for IPersistStreamInit
	CComQIPtr<IPersistStream> spPersistStream(pObject);
	if (!spPersistStream)
		{
		pObject->QueryInterface(IID_IPersistStreamInit, (void**)&spPersistStream.p);
		}
	if (spPersistStream)
		{
		// Create a stream
		CComPtr<IStream> spStream;
		RETURNIFFAILED(CreateStreamOnHGlobal(NULL, TRUE, &spStream));

		// Save the object
		RETURNIFFAILED(spPersistStream->Save(spStream, dwFlags & PXF_CLEARDIRTY));

		// Get length of stream
		STATSTG stat;
		spStream->Stat(&stat, STATFLAG_NONAME);

		LPBYTE pTempBuffer=(LPBYTE)malloc((int)stat.cbSize.QuadPart);

		// Back to start
		LARGE_INTEGER li={0};
		spStream->Seek(li, STREAM_SEEK_SET, NULL);

		// read the entire string (put at end of buffer)
		spStream->Read(pTempBuffer, (int)stat.cbSize.QuadPart, NULL);

		// Format binary data
		CUniString str=FormatBinaryData(pTempBuffer, stat.cbSize.LowPart, 0);
		*pVal=SysAllocString(str);

		free(pTempBuffer);
		return S_OK;
		}

	return S_OK;
}

HRESULT SIMPLEAPI SlxLoadObject(IUnknown* pObject, BSTR newVal, LPCOLESTR pszDocElem, DWORD dwFlags)
{
	ASSERT(pObject!=NULL);

	// Attempt to load from XML
	CXMLArchive ar;
	if (SUCCEEDED(ar.LoadXML(newVal, pszDocElem, dwFlags)))
		{
		// Saved as a property bag?
		if (ar.EnterElement(L"PropertyBag", TRUE)==S_OK)
			{
			CComQIPtr<IPersistPropertyBag> spPersistPropertyBag(pObject);
			if (!spPersistPropertyBag)
				return E_NOINTERFACE;

			// Setup a propertybag on XML Object
			CPropertyBagOnXML pb;

			// Initialise the property bag
			RETURNIFFAILED(pb.Init(&ar));

			// InitNew?
			if (ar.GetFlags() & PXF_INITNEW)
				{
				ar.SetResult(spPersistPropertyBag->InitNew());
				}

			// Load it
			if (SUCCEEDED(ar.GetResult()))
				{
				ar.SetResult(spPersistPropertyBag->Load(&pb, &pb));
				}

			return ar.GetResult();
			}
		else
			{
			// IPersistXML?
			CComQIPtr<IPersistXML> spPersistXML(pObject);
			if (spPersistXML)
				{
				RETURNIFFAILED(spPersistXML->Load(ar.CurrentElement(), NULL, ar.GetFlags()));
				return S_OK;
				}
			}
		}

	// Query for IPersistStreamInit
	CComQIPtr<IPersistStream> spPersistStream(pObject);
	if (!spPersistStream)
		{
		pObject->QueryInterface(IID_IPersistStreamInit, (void**)&spPersistStream.p);
		}
	if (!spPersistStream)
		return E_NOINTERFACE;

	// Create a stream
	CComPtr<IStream> spStream;
	RETURNIFFAILED(ar.SetResult(CreateStreamOnHGlobal(NULL, TRUE, &spStream)));

	// Read binary data
	unsigned char* pData;
	size_t cbData;
	if (UnformatBinaryData(newVal, (void**)&pData, &cbData))
		{
		// Write it to stream...
		spStream->Write(pData, (ULONG)cbData, NULL);
		free(pData);
		}

	// Back to start
	LARGE_INTEGER li={0};
	spStream->Seek(li, STREAM_SEEK_SET, NULL);

	// Have the object load
	return spPersistStream->Load(spStream);
}

}