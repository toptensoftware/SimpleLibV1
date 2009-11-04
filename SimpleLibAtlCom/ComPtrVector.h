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
// ComPtrVector.h - declaration of ComPtrVector

#ifndef __COMPTRVECTOR_H
#define __COMPTRVECTOR_H

#include "DispatchProperty.h"

#include "EnumVariantImpl.h"
#include "ObjectWithWeakOwner.h"

namespace Simple
{

class SComPtr
{
public:
	template <class T, class TOwner>
	static const T& OnAdd(const T& val, TOwner* pOwner)
	{ 
		if (val)
			val->AddRef();
		if (((CComPtrVector<T>*)pOwner)->m_pWeakOwner)
			SetWeakOwnerOfObject(val, ((CComPtrVector<T>*)pOwner)->m_pWeakOwner);
		return val;
	}

	template <class T, class TOwner>
	static void OnRemove(T& val, TOwner* pOwner)
	{ 
		if (val)
			val->Release();
		if (((CComPtrVector<T>*)pOwner)->m_pWeakOwner)
			SetWeakOwnerOfObject(val, NULL);
	}
	
	template <class T, class TOwner>
	static void OnDetach(T& val, TOwner* pOwner)
	{ 
		if (((CComPtrVector<T>*)pOwner)->m_pWeakOwner)
			SetWeakOwnerOfObject(val, NULL);
	}
	
	template <class T>
	static bool IsEqual(const T& a, const T& b)
	{ 
		return IsSameObject(a,b); 
	}

	template <class T>
	static int Compare(const T& a, const T& b)
	{ 
		return SValue::Compare(a,b); 
	}
};

// CComPtrVector
template <class T>
class CComPtrVector : 
	public CVector<T, SComPtr>, 
	public IEnumVariantHolder
{
	typedef CVector<T, SComPtr> baseclass;
public:
// Construction
	CComPtrVector()
	{
		m_pWeakOwner=NULL;
	}
	virtual ~CComPtrVector()
	{
		for (int i=0; i<m_Enumerators.GetSize(); i++)
			m_Enumerators[i]->OwnerDestroyed();
		RemoveAll();
	}

// Operations
	int GetSize() const
	{
		return baseclass::GetSize();
	}
	T GetAt(int iPosition) const
	{
		return baseclass::GetAt(iPosition);
	}
	HRESULT GetAt(int iPosition, T* ppUnk) const
	{
		*ppUnk=GetAt(iPosition);
		if (*ppUnk)
			(*ppUnk)->AddRef();
		return S_OK;
	}
	HRESULT QueryAt(int iPosition, REFIID riid, void** ppv) const
	{
		ASSERT(GetAt(iPosition)!=NULL);
		return GetAt(iPosition)->QueryInterface(riid, ppv);
	}

	template <class Itf>
	HRESULT QueryAt(int iPosition, Itf** ppv) const
	{
		return QueryAt(iPosition, __uuidof(Itf), (void**)ppv);
	}

// Attributes
	// Set the weak owner of all elements in this vector
	void CComPtrVector::SetWeakOwner(IUnknown* pWeakOwner)
	{
		// Store weak owner pointer
		m_pWeakOwner=pWeakOwner;

		// Tell all members
		for (int i=0; i<GetSize(); i++)
			{
			SetWeakOwnerOfObject(GetAt(i), m_pWeakOwner);
			}
	}

	IUnknown* m_pWeakOwner;

// Enumerator Attributes
	virtual HRESULT GetSize(int* piSize) const
	{
		*piSize=GetSize();
		return S_OK;
	}

	virtual HRESULT GetItem(int iItem, VARIANT* pvarItem) const
	{
		CComQIPtr<IDispatch> spDisp(GetAt(iItem));
		if (spDisp)
			{
			V_VT(pvarItem)=VT_DISPATCH;
			V_DISPATCH(pvarItem)=spDisp.Detach();
			}
		else
			{
			V_VT(pvarItem)=VT_UNKNOWN;
			V_UNKNOWN(pvarItem)=GetAt(iItem);
			if (V_UNKNOWN(pvarItem))
				V_UNKNOWN(pvarItem)->AddRef();
			}

		return S_OK;
	}

	HRESULT SetIndex(int* piIndex, int iVal) const
	{
		if (piIndex)
			piIndex[0]=iVal;
		return S_OK;

	}

	HRESULT EvaluateSimpleIndex(int* piIndex, VARIANT Index) const
	{
		// Evaluate index
		int iIndex=-1;
		CComBSTR bstrIndex;
		RETURNIFFAILED(VariantToStringOrInt(Index, iIndex, bstrIndex));
		if (iIndex<0 || iIndex>=GetSize())
			return E_INVALIDARG;

		return SetIndex(piIndex, iIndex);
	}

	HRESULT EvaluateIndex(int* piIndex, LPCOLESTR pszName, const CDISPID& dispid=DISPID_VALUE) const
	{
		// Find it...
		for (int i=0; i<GetSize(); i++)
			{
			// Get dispatch
			CComPtr<IDispatch> spDispatch;
			QueryAt(i, &spDispatch);

			// Check dispatch supported
			ASSERT((spDispatch!=NULL || GetAt(i)==NULL) && "CComPtrVector::EvaluateIndex used on non-dispatch collection");

			if (spDispatch)
				{
				// Get property value
				CComVariant varProperty;
				if (SUCCEEDED(SlxGetProperty(spDispatch, dispid, &varProperty)))
					{
					// Convert to a string
					if (SUCCEEDED(varProperty.ChangeType(VT_BSTR)))
						{
						// Does it match?
						if (IsEqualStringI(V_BSTR(&varProperty), pszName))
							{
							return SetIndex(piIndex, i);
							}
						}
					else
						{
						ASSERT(FALSE && "CComPtrVector::EvaluateIndex failed to convert property to string");
						}
					}
				else
					{
					ASSERT(FALSE && "CComPtrVector::EvaluateIndex failed to get property value");
					}
				}
			}

		return E_INVALIDARG;
	}


	HRESULT EvaluateIndex(int* piIndex, VARIANT Index, bool bForInsert, const CDISPID& dispid=DISPID_VALUE) const
	{
		if (bForInsert && !IsParameterSpecified(Index))
			{
			return SetIndex(piIndex, GetSize());
			}

		// Evaluate index
		int iIndex=-1;
		CComBSTR bstrIndex;
		CComPtr<IUnknown> spIndex;
		RETURNIFFAILED(VariantToStringOrIntOrObject(Index, &iIndex, &bstrIndex, &spIndex));

		// Object reference?
		if (spIndex)
			{
			// Find it
			iIndex=Find(spIndex);

			// Valid?
			if (iIndex<0)
				return E_INVALIDARG;

			return SetIndex(piIndex, iIndex);
			}

		// String field name?
		if (bstrIndex)
			{
			return EvaluateIndex(piIndex, bstrIndex, dispid);
			}

		if (iIndex<0 && bForInsert)
			iIndex=GetSize();

		// Must be integer index
		if (iIndex<0 || iIndex>=(GetSize() + (bForInsert ? 1 : 0)))
			return E_INVALIDARG;

		// Return it
		return SetIndex(piIndex, iIndex);
	}

	HRESULT EvaluateIndex(int* piIndex, VARIANT Index, T* pVal, const CDISPID& dispid=DISPID_VALUE) const
	{
		int iIndex;
		if (!piIndex)
			piIndex=&iIndex;

		RETURNIFFAILED(EvaluateIndex(piIndex, Index, false, dispid));
		return GetAt(piIndex[0], pVal);
	}


	HRESULT RemoveIndex(int* piIndex, VARIANT Index, const CDISPID& dispid=DISPID_VALUE)
	{
		int iIndex;
		if (!piIndex)
			piIndex=&iIndex;

		RETURNIFFAILED(EvaluateIndex(piIndex, Index, false, dispid));
		return RemoveAt(piIndex[0]);
	}

	HRESULT InsertIndex(int* piIndex, VARIANT Index, T* pObject, IUnknown** pVal, const CDISPID& dispid=DISPID_VALUE)
	{
		int iIndex;
		if (!piIndex)
			piIndex=&iIndex;

		if (IsParameterSpecified(Index))
			{
			RETURNIFFAILED(EvaluateIndex(piIndex, Index, true, dispid));
			}
		else
			{
			SetIndex(piIndex, GetSize());
			}
		return InsertAt(piIndex[0], pObject, pVal);
	}


	HRESULT InsertAt(int iIndex, T* pObject, T** pVal=NULL)
	{
		baseclass::InsertAt(iIndex, pObject);
		if (pVal)
			{
			*pVal=pObject;
			if (*pVal)
				(*pVal)->AddRef();
			}
		return S_OK;
	}

	// Initial this vector from a safe array
	HRESULT InitFromSafeArray(SAFEARRAY* pArray)
	{
		// Check its a variant safe array
		VARTYPE vt;
		RETURNIFFAILED(SafeArrayGetVartype(pArray, &vt));
		if (vt!=VT_VARIANT && vt!=VT_UNKNOWN)
			return E_UNEXPECTED;

		// Check 1 dimensional
		UINT nDims=SafeArrayGetDim(pArray);
		if (nDims!=1)
			return E_INVALIDARG;

		// Get U & L Bound
		LONG lUbound;
		RETURNIFFAILED(SafeArrayGetUBound(pArray, 1, &lUbound));
		LONG lLbound;
		RETURNIFFAILED(SafeArrayGetLBound(pArray, 1, &lLbound));

		// Add each item
		for (LONG j=lLbound; j<=lUbound; j++)
		{
			if (vt==VT_VARIANT)
			{
				CComVariant var;
				if (SUCCEEDED(SafeArrayGetElement(pArray, &j, &var)) && SUCCEEDED(var.ChangeType(VT_UNKNOWN)))
				{
					T p=NULL;

					if (V_UNKNOWN(&var))
					{
						V_UNKNOWN(&var)->QueryInterface(__uuidof(*p), (void**)&p);
					}
					Add(p);
					if (p)
						p->Release();
				}
			}
			else
			{
				CComPtr<IUnknown> spUnk;
				if (SUCCEEDED(SafeArrayGetElement(pArray, &j, &spUnk)))
				{
					T p=NULL;

					if (spUnk)
					{
						spUnk->QueryInterface(__uuidof(*p), (void**)&p);
					}
					Add(p);
					if (p)
						p->Release();
				}
			}
		}

		return S_OK;
	}

	HRESULT CreateSafeArray(SAFEARRAY** pVal)
	{
		// Create a safe array
		SAFEARRAYBOUND bounds={Vector.GetSize(), 0};
		SAFEARRAY* pArray=SafeArrayCreate(VT_VARIANT, 1, &bounds);
		if (!pArray)
			return E_OUTOFMEMORY;

		// Access data
		VARIANT* pvar;
		SafeArrayAccessData(pArray, (void**)&pvar);

		for (int i=0; i<Vector.GetSize(); i++)
			{
			V_VT(pvar)=VT_DISPATCH;
			V_DISPATCH(pvar)=CComQIPtr<IDispatch>(Vector[i]).Detach();
			pvar++;
			}

		SafeArrayUnaccessData(pArray);

		*pVal=pArray;
		return S_OK;
	}

};

}	// namespace Simple

#endif	// __COMPTRVECTOR_H

