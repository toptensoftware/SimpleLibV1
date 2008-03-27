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
// ObjectWithWeakOwner.cpp - implementation of ObjectWithWeakOwner

#include "stdafx.h"
#include "SimpleLibAtlComBuild.h"

#include "ObjectWithWeakOwner.h"


namespace Simple
{

// Constructor
IObjectWithWeakOwnerImpl::IObjectWithWeakOwnerImpl() 
{ 
	m_pWeakOwner=NULL; 
};

// SetWeakOwner implementation
HRESULT IObjectWithWeakOwnerImpl::SetWeakOwner(IUnknown* pOwner) 
{ 
	m_pWeakOwner=pOwner; 
	return S_OK;
};

// GetWeakOwner implementation
HRESULT IObjectWithWeakOwnerImpl::GetWeakOwner(IUnknown** ppOwner)
{
	if (!ppOwner) return E_POINTER;
	*ppOwner=m_pWeakOwner;
	if (m_pWeakOwner) m_pWeakOwner->AddRef();
	return S_OK;
};


// Query owner object for a specific interface
HRESULT IObjectWithWeakOwnerImpl::QueryOwnerInterface(REFIID riid, void** ppvObject, BOOL bAllowNull)
{
	// Check out param pointer
	if (!ppvObject)
		return E_POINTER;

	// Check have an owner
	if (!m_pWeakOwner)	
		{
		ppvObject=NULL;
		return bAllowNull ? S_OK : E_NOOWNER;
		}

	// Query owner...
	return m_pWeakOwner->QueryInterface(riid, ppvObject);
}


// Get the weak owner of a 
HRESULT GetWeakOwnerOfObject(IUnknown* pObject, REFIID riid, void** ppvOwner)
{
	// Get IUnknownWithWeakOwner
	CComQIPtr<IObjectWithWeakOwner> spObjectWithWeakOwner(pObject);
	if (!spObjectWithWeakOwner)
		return E_NOINTERFACE;

	// Get owner...
	CComPtr<IUnknown> spUnkOwner;
	spObjectWithWeakOwner->GetWeakOwner(&spUnkOwner);

	if (!spUnkOwner)
		return S_FALSE;

	// Query for desired interface...
	return spUnkOwner->QueryInterface(riid, ppvOwner);
}

// Helper to set an object's weak owner.
HRESULT SetWeakOwnerOfObject(IUnknown* pObject, IUnknown* pOwner)
{
	CComQIPtr<IObjectWithWeakOwner> sp(pObject);
	if (!sp)
		return E_NOINTERFACE;

	return sp->SetWeakOwner(pOwner);
}

// Walk weak owner chain to find an owner supporting a particular interface
HRESULT GetWeakOwnerOfType(IUnknown* pObject, REFIID riid, void** ppvObject)
{
	return GetWeakOwnerOfTypeEx(pObject, riid, ppvObject, true);
}

HRESULT GetWeakOwnerOfTypeEx(IUnknown* pObject, REFIID riid, void** ppvObject, bool bIncludeSelf)
{
	// Query for requested interface
	if (bIncludeSelf)
		{
		HRESULT hr=pObject->QueryInterface(riid, ppvObject);
		if (SUCCEEDED(hr))
			return hr;
		}

	// Query for IObjectWithWeakOwner
	CComQIPtr<IObjectWithWeakOwner> spOWWO(pObject);
	if (!spOWWO)
		return E_NOOWNER;

	// Get the owner
	CComPtr<IUnknown> spOwner;
	spOWWO->GetWeakOwner(&spOwner);

	// Quit if no owner
	if (!spOwner)
		{
		*ppvObject=NULL;
		return E_NOOWNER;
		}

	// Recurse up the chain
	return GetWeakOwnerOfTypeEx(spOwner, riid, ppvObject, true);
}


bool IsWeakChild(IUnknown* pParent, IUnknown* pChild)
{
	if (!pParent || !pChild)
		return false;

	CComPtr<IUnknown> spChild;
	GetWeakOwnerOfObject(pChild, &spChild);
	while (spChild)
		{
		if (spChild.IsEqualObject(pParent))
			return true;

		CComPtr<IUnknown> spParent;
		GetWeakOwnerOfObject(spChild, &spParent);
		spChild=spParent;
		}

	return false;
}


HRESULT GetTopWeakOwner(IUnknown* pObject, bool bAllowSelf, REFIID riid, void** ppvOwner)
{
	if (!pObject)
		return E_NOOWNER;

	// Work out where to start...
	CComPtr<IUnknown> spObject;
	if (bAllowSelf)
		spObject=pObject;
	else
		GetWeakOwnerOfObject(pObject, &spObject);

	while (spObject)
		{
		// Get parent...
		CComPtr<IUnknown> spParent;
		GetWeakOwnerOfObject(spObject, &spParent);

		// No more parents?
		if (!spParent)
			{
			return spObject->QueryInterface(riid, ppvOwner);
			}

		spObject=spParent;
		}

	return E_NOOWNER;
}

// Constructor
CWeakOwnerPtr::CWeakOwnerPtr()
{
	p=NULL;
}

// Destructor
CWeakOwnerPtr::~CWeakOwnerPtr()
{
	Release();
}

// Init
HRESULT CWeakOwnerPtr::Init(IUnknown* pWeakOwner, IUnknown* pObject)
{
	ASSERT(pWeakOwner!=NULL);
	ASSERT(pObject!=NULL);

	// Store the pointer
	p=pObject;
	p->AddRef();

	// Set weak owner...
	HRESULT hr=SetWeakOwnerOfObject(pObject, pWeakOwner);
	if (FAILED(hr))
		return hr;

	// Done
	return S_OK;
}

void CWeakOwnerPtr::Release()
{
	// Clear owner pointer
	if (p)
		{
		SetWeakOwnerOfObject(p, NULL);
		p->Release();
		}

	p=NULL;
}





}	// namespace Simple
