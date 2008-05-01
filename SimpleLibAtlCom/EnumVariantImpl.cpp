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
// EnumVariantImpl.cpp - implementation of EnumVariantImpl

#include "stdafx.h"
#include "SimpleLibAtlComBuild.h"

#include "EnumVariantImpl.h"


namespace Simple
{

// Constructor
IEnumVariantImpl::IEnumVariantImpl(IEnumVariantOwner* pOwner, int iPos)
{
	_ASSERTE(pOwner!= NULL);

	// Init members
	m_dwRef = 1;
	m_iPos = iPos;
	m_pOwner = pOwner;

	// Add self...
	pOwner->AddEnumerator(this);
}

// Destructor
IEnumVariantImpl::~IEnumVariantImpl()
{
	// Remove self from Vectors list of enumerators
	if (m_pOwner)
		m_pOwner->RemoveEnumerator(this);
}

STDMETHODIMP_(ULONG) IEnumVariantImpl::AddRef()
{
	m_dwRef++;
	return m_dwRef;
}

STDMETHODIMP_(ULONG) IEnumVariantImpl::Release()
{
	_ASSERTE(m_dwRef > 0);

	if (m_dwRef == 1)
	{
		m_dwRef = 0;
		delete this;
		return 0;
	}

	return --m_dwRef;
}

STDMETHODIMP IEnumVariantImpl::QueryInterface(REFIID iid, void** ppvObject)
{
	// Check params
	if (!ppvObject)
		return E_INVALIDARG;

	// Only support IUnknown and IEnumVARIANT
	if (iid == IID_IUnknown || iid == IID_IEnumVARIANT)
	{
		AddRef();
		*ppvObject = this;
		return S_OK;
	}

	*ppvObject = NULL;
	return E_NOINTERFACE;
}

STDMETHODIMP IEnumVariantImpl::Next(ULONG celt, VARIANT* rgelt, ULONG* pceltFetched)
{
	// Check params
	if (!celt) return S_OK;
	if (celt != 1 && !pceltFetched) return E_INVALIDARG;
	if (!rgelt) return E_INVALIDARG;
	if (!m_pOwner) return E_UNEXPECTED;

	// Get count of items available
	int iCount = 0;
	RETURNIFFAILED(m_pOwner->GetSize(&iCount));
	
	ULONG i;
	for (i = 0; i < celt; i++, m_iPos++)
		{
		// Check have more!
		if (m_iPos >= iCount)
			break;

		// Get variant item from the owner
		RETURNIFFAILED(m_pOwner->GetItem(m_iPos, &rgelt[i]));
		}

	// Return number fetched
	if (pceltFetched)
		*pceltFetched = i;
	return i == celt ? S_OK : S_FALSE;
}

STDMETHODIMP IEnumVariantImpl::Skip(ULONG celt)
{
	// Check vector hasn't been destroyed
	if (!m_pOwner) return E_UNEXPECTED;

	// Update position
	m_iPos += celt;

	int iSize;
	RETURNIFFAILED(m_pOwner->GetSize(&iSize));

	// Past end?
	if (m_iPos >= iSize)
		return S_FALSE;
	return S_OK;
}

STDMETHODIMP IEnumVariantImpl::Reset()
{
	// Check vector hasn't been destroyed
	if (!m_pOwner) return E_UNEXPECTED;

	// Reset to start
	m_iPos = 0;
	return S_OK;
}

STDMETHODIMP IEnumVariantImpl::Clone(IEnumVARIANT** ppenum)
{
	// Check vector hasn't been destroyed
	if (!m_pOwner) return E_UNEXPECTED;

	// Check params
	if (!ppenum)
		return E_INVALIDARG;

	// Create new enumerator
	if (!(*ppenum = new IEnumVariantImpl(m_pOwner, m_iPos)))
		return E_OUTOFMEMORY;
	return S_OK;
}

void IEnumVariantImpl::SyncInsert(int iPos)
{
	// If inserted before current position, move current position forward by one
	if (iPos < m_iPos)
		m_iPos++;
}	

void IEnumVariantImpl::SyncDelete(int iPos)
{
	// If deleted before current position, move current position back by one
	if (iPos < m_iPos)
		m_iPos--;
}	

void IEnumVariantImpl::OwnerDestroyed()
{
	m_pOwner=NULL;
}



//////////////////////////////////////////////////////////////////////
// IEnumVariantHolder


// Constructor
IEnumVariantHolder::IEnumVariantHolder()
{
}

// Destructor
IEnumVariantHolder::~IEnumVariantHolder()
{
	// Tell all enumerators that owner has been destroyed
	for (int i=0; i<m_Enumerators.GetSize(); i++)
		m_Enumerators[i]->OwnerDestroyed();
}


// Implementation of AddEnumerator
void IEnumVariantHolder::AddEnumerator(IEnumVariantImpl* pEnum)
{
	m_Enumerators.Add(pEnum);
}

// Implementation of RemoveEnumerator
void IEnumVariantHolder::RemoveEnumerator(IEnumVariantImpl* pEnum)
{
	ASSERT(m_Enumerators.Find(pEnum)!=-1);
	m_Enumerators.RemoveAt(m_Enumerators.Find(pEnum));
}

// Implementation of SyncInsert
void IEnumVariantHolder::SyncInsert(int iPos)
{
	for (int i=0; i<m_Enumerators.GetSize(); i++)
		m_Enumerators[i]->SyncInsert(iPos);
}

// Implementation of SyncDelete
void IEnumVariantHolder::SyncDelete(int iPos)
{
	for (int i=0; i<m_Enumerators.GetSize(); i++)
		m_Enumerators[i]->SyncDelete(iPos);
}

// Create a new enumerator
HRESULT IEnumVariantHolder::CreateEnumerator(IUnknown** ppEnum)
{
	if (!ppEnum)
		return E_INVALIDARG;
	if (!(*ppEnum = new IEnumVariantImpl(this)))
		return E_OUTOFMEMORY;
	return S_OK;
}


}	// namespace Simple
