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
// ObjectWithNamedValues.cpp - implementation of ObjectWithNamedValues

#include "stdafx.h"
#include "SimpleLibAtlComBuild.h"

#include "ObjectWithNamedValues.h"



namespace Simple
{

/////////////////////////////////////////////////////////////////////////
// CNamedValue

// Constructor
IObjectWithNamedValuesImpl::CNamedValue::CNamedValue(LPCOLESTR pszName, VARIANT_BOOL bPersist) : 
	m_bstrName(pszName),
	m_bPersist(bPersist)
{
	
}

//////////////////////////////////////////////////////////////////////////
// IObjectWithNamedValuesImpl

// Constructor
IObjectWithNamedValuesImpl::IObjectWithNamedValuesImpl()
{
}

// Destructor
IObjectWithNamedValuesImpl::~IObjectWithNamedValuesImpl()
{
}


// Find a named value by name
IObjectWithNamedValuesImpl::CNamedValue* IObjectWithNamedValuesImpl::Find(LPCOLESTR pszName)
{
	for (int i=0; i<m_Items.GetSize(); i++)
		{
		if (_wcsicmp(m_Items[i]->m_bstrName, pszName)==0)
			return m_Items[i];
		}

	return NULL;
}


STDMETHODIMP IObjectWithNamedValuesImpl::Reset()
{
	m_Items.RemoveAll();
	for (int i=0; i<m_Listeners.GetSize(); i++)
		{
		m_Listeners[i]->m_pListener->OnNamedValueChanged(NULL, NULL);
		}
	return S_OK;
}

STDMETHODIMP IObjectWithNamedValuesImpl::SetNamedValue(LPCOLESTR pszName, VARIANT_BOOL bPersist, VARIANT newVal)
{
	CNamedValue* pEntry=Find(pszName);
	if (V_VT(&newVal)==VT_EMPTY)
		{
		if (pEntry)
			m_Items.Remove(pEntry);
		}
	else
		{
		if (!pEntry)
			{
			pEntry=new CNamedValue(pszName, bPersist);
			m_Items.Add(pEntry);
			}

		pEntry->m_bPersist=bPersist;
		VariantCopy(&pEntry->m_varValue, &newVal);
		}

	for (int i=0; i<m_Listeners.GetSize(); i++)
		{
		if (IsEmptyString(m_Listeners[i]->m_bstrName) || 
			IsEqualStringI(m_Listeners[i]->m_bstrName, pszName))
			{
			m_Listeners[i]->m_pListener->OnNamedValueChanged(pszName, &newVal);
			}
		}

	return S_OK;
}

STDMETHODIMP IObjectWithNamedValuesImpl::GetNamedValue(LPCOLESTR pszName, VARIANT* pVal)
{
	CNamedValue* pEntry=Find(pszName);
	if (!pEntry)
		{
		return VariantClear(pVal);
		}
	else
		{
		return VariantCopy(pVal, &pEntry->m_varValue);
		}
}

STDMETHODIMP IObjectWithNamedValuesImpl::GetCount(int* pVal)
{
	*pVal=m_Items.GetSize();
	return S_OK;
}

STDMETHODIMP IObjectWithNamedValuesImpl::GetItem(int iIndex, BSTR* pbstrName, VARIANT_BOOL* pbPersisted, VARIANT* pvarValue)
{
	if (iIndex<0 || iIndex>m_Items.GetSize())
		return E_INVALIDARG;

	CNamedValue* pValue=m_Items[iIndex];
	if (pbstrName)
		*pbstrName=pValue->m_bstrName.Copy();
	if (pbPersisted)
		*pbPersisted=pValue->m_bPersist;
	if (pvarValue)
		VariantCopy(pvarValue, &pValue->m_varValue);

	return S_OK;
}

STDMETHODIMP IObjectWithNamedValuesImpl::AddListener(LPCOLESTR pszName, VARIANT_BOOL bStrongRef, IObjectWithNamedValuesListener* pListener)
{
	// Add a listener
	m_Listeners.Add(new CListener(pszName, !!bStrongRef, pListener));
	return S_OK;
}

STDMETHODIMP IObjectWithNamedValuesImpl::RemoveListener(LPCOLESTR pszName, IObjectWithNamedValuesListener* pListener)
{
	// Remove a listener
	for (int i=m_Listeners.GetSize()-1; i>=0; i--)
		{
		if (m_Listeners[i]->m_pListener==pListener &&
			IsEqualStringI(m_Listeners[i]->m_bstrName, pszName))
			{
			m_Listeners.RemoveAt(i);
			}
		}

	return S_OK;
}



/////////////////////////////////////////////////////////////////////////////
// Helpers for working with named values

// Copy all named values from one named values collection to another...
HRESULT SIMPLEAPI CopyNamedValues(IObjectWithNamedValues* pDest, IObjectWithNamedValues* pSource, bool bAll)
{
	// Get other collection count
	int iCount;
	pSource->GetCount(&iCount);

	// Enumerate all items
	for (int i=0; i<iCount; i++)
		{
		CComBSTR bstrName;
		VARIANT_BOOL bIsPersisted;
		CComVariant varValue;
		if (SUCCEEDED(pSource->GetItem(i, &bstrName, &bIsPersisted, &varValue)) && (bIsPersisted || bAll))
			{
			pDest->SetNamedValue(bstrName, bIsPersisted, varValue);
			}
		}

	return S_OK;

}

// Check if any named value in the collection should be persisted...
bool SIMPLEAPI AnyPersistedNamedValues(IObjectWithNamedValues* pNamedValues)
{
	// Get other collection count
	int iCount;
	pNamedValues->GetCount(&iCount);

	// Enumerate all items
	for (int i=0; i<iCount; i++)
		{
		VARIANT_BOOL bIsPersisted;
		if (SUCCEEDED(pNamedValues->GetItem(i, NULL, &bIsPersisted, NULL)))
			{
			if (bIsPersisted)
				return true;
			}
		}

	return false;
}


// Load/save all the values in a named values collection
HRESULT SIMPLEAPI SerializeNamedValues(IObjectWithNamedValues* pNamedValues, CXMLArchive& ar, LPCOLESTR pszExcludeOnLoad, LPCOLESTR pszElementName)
{
	// Quit if saving and nothing to save...
	if (ar.IsStoring() && !AnyPersistedNamedValues(pNamedValues))
		return S_OK;

	// Using a child element?
	if (pszElementName)
		{
		// Enter the element, quit if doesn't exist
		if (ar.EnterElement(pszElementName, VARIANT_TRUE)!=S_OK)
			return S_OK;
		}

	if (ar.IsLoading())
		{
		// Work out a list of items to ignore
		CVector<CUniString> Exclude;
		SplitString(pszExcludeOnLoad, L";", Exclude);

		CComPtr<IXMLDOMNode> spChild;
		ar.CurrentElement()->get_firstChild(&spChild);

		while (spChild)
			{
			// Is it an attribute?
			DOMNodeType type;
			spChild->get_nodeType(&type);
			if (type==NODE_ELEMENT)
				{
				// Get its name
				CComBSTR bstrName;
				spChild->get_nodeName(&bstrName);

				// Is it to be excluded?
				if (Exclude.Find(bstrName.m_str)<0)
					{
					// Get its value
					CComVariant varValue;
					spChild->get_nodeTypedValue(&varValue);

					// Get its VT
					CComVariant varType;
					if (SUCCEEDED(CComQIPtr<IXMLDOMElement>(spChild)->getAttribute(L"type", &varType)))
						{
						if (SUCCEEDED(varType.ChangeType(VT_I4)))
							{
							varValue.ChangeType((VARTYPE)(V_I4(&varType)));
							}
						}

					// Set it
					pNamedValues->SetNamedValue(bstrName, VARIANT_TRUE, varValue);
					}
				}

			// Get next node...
			CComPtr<IXMLDOMNode> spNext;
			spChild->get_nextSibling(&spNext);
			spChild=spNext;
			}
		}
	else
		{
		int iCount;
		pNamedValues->GetCount(&iCount);

		for (int i=0; i<iCount; i++)
			{
			CComBSTR bstrName;
			VARIANT_BOOL bPersist;
			CComVariant varValue;
			pNamedValues->GetItem(i, &bstrName, &bPersist, &varValue);

			if (bPersist)
				{
				CComVariant varTemp;
				if (SUCCEEDED(varTemp.ChangeType(VT_BSTR, &varValue)))
					{
					// Save it...
					CComBSTR bstr=V_BSTR(&varTemp);
					XML_Item(ar.Elements(), bstrName, bstr);

					// Save type also...
					if (V_VT(&varValue)!=VT_BSTR)
						{
						// Get the last element
						CComPtr<IXMLDOMNode> spLastNode;
						ar.CurrentElement()->get_lastChild(&spLastNode);
						CComQIPtr<IXMLDOMElement> spLastElement(spLastNode);
						ASSERT(spLastElement!=NULL);
						
						// Store the VT
						spLastElement->setAttribute(CComBSTR("type"), CComVariant((int)V_VT(&varValue)));
						}
					}
				}
			}
		}

	// Leave the element
	if (pszElementName)
		{
		ar.LeaveElement();
		}

	return S_OK;
}



}	// namespace Simple
