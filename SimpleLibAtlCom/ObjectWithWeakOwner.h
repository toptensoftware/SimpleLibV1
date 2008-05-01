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
// ObjectWithWeakOwner.h - declaration of ObjectWithWeakOwner

#ifndef __OBJECTWITHWEAKOWNER_H
#define __OBJECTWITHWEAKOWNER_H

namespace Simple
{

// IObjectWithWeakOwner interface declaration
MIDL_INTERFACE("3F0CBD46-FEDB-11D3-8F7F-00A0C9E4F3AA")
IObjectWithWeakOwner : public IUnknown
{
public:
	virtual HRESULT SetWeakOwner(IUnknown* pOwner) = 0;
	virtual HRESULT GetWeakOwner(IUnknown** ppOwner) = 0;
};


#define E_NOOWNER	E_UNEXPECTED

// Implementation of IObjectWithWeakOwner interface
class ATL_NO_VTABLE IObjectWithWeakOwnerImpl : public IObjectWithWeakOwner
{
public:
// Construction
	IObjectWithWeakOwnerImpl();

// IObjectWithWeakOwner
	virtual HRESULT SetWeakOwner(IUnknown* pOwner);
	virtual HRESULT GetWeakOwner(IUnknown** ppOwner);

// Query owner for an interface
	HRESULT QueryOwnerInterface(REFIID riid, void** ppvObject, BOOL bAllowNull);

// Template version of QueryOwnerInterface
	template <class Itf>
	HRESULT QueryOwnerInterface(Itf** ppItf)
	{
		return QueryOwnerInterface(__uuidof(Itf), (void**)ppItf, FALSE);
	};

	template <class Itf>
	HRESULT QueryOptionalOwnerInterface(Itf** ppItf)
	{
		return QueryOwnerInterface(__uuidof(Itf), (void**)ppItf, TRUE);
	};

	bool DoesHaveOwner() { return m_pWeakOwner!=NULL; };

	template <class Itf>
	HRESULT GetWeakOwnerOfType(Itf** ppv)
		{ return Simple::GetWeakOwnerOfType(this, __uuidof(Itf), (void**)ppv); }
	template <class Itf>
	HRESULT GetWeakOwnerOfTypeEx(Itf** ppv, bool bIncludeSelf)
		{ return Simple::GetWeakOwnerOfTypeEx(this, __uuidof(Itf), (void**)ppv, bIncludeSelf); }



// Attributes
private:
	IUnknown*	m_pWeakOwner;
};


// Macro to check have an owner...
#define RETURNIFNOOWNER() if (!m_pWeakOwner) return E_NOOWNER;


// Helper to get the owner of an object using it's IObjectWithWeakOwner interface
HRESULT SIMPLEAPI GetWeakOwnerOfObject(IUnknown* pObject, REFIID riid, void** ppvOwner);
HRESULT SIMPLEAPI SetWeakOwnerOfObject(IUnknown* pObject, IUnknown* pOwner);

// Template version of above....
template <class Itf>
HRESULT SIMPLEAPI GetWeakOwnerOfObject(IUnknown* pObject, Itf** ppOwner)
{
	return GetWeakOwnerOfObject(pObject, __uuidof(Itf), (void**)ppOwner);
}

// Helper to check if one is child of another
bool SIMPLEAPI IsWeakChild(IUnknown* pParent, IUnknown* pChild);

// Helper to get to the top of the object heirarchy...
HRESULT SIMPLEAPI GetTopWeakOwner(IUnknown* pObject, bool bAllowSelf, REFIID riid, void** ppvOwner);
template <class Itf>
HRESULT SIMPLEAPI GetTopWeakOwner(IUnknown* pObject, bool bAllowSelf, Itf** ppOwner)
{
	return GetTopWeakOwner(pObject, __uuidof(Itf), (void**)ppOwner);
}


// Walk weak owner chain to find an owner supporting a particular interface
HRESULT SIMPLEAPI GetWeakOwnerOfType(IUnknown* pObject, REFIID riid, void** ppvObject);
template <class Itf>
HRESULT SIMPLEAPI GetWeakOwnerOfType(IUnknown* pObject, Itf** ppv)
	{ return GetWeakOwnerOfType(pObject, __uuidof(Itf), (void**)ppv); }
HRESULT SIMPLEAPI GetWeakOwnerOfTypeEx(IUnknown* pObject, REFIID riid, void** ppvObject, bool bIncludeSelf);
template <class Itf>
HRESULT SIMPLEAPI GetWeakOwnerOfTypeEx(IUnknown* pObject, Itf** ppv, bool bIncludeSelf)
	{ return GetWeakOwnerOfTypeEx(pObject, __uuidof(Itf), (void**)ppv, bIncludeSelf); }



// Pointer to a weakly owned object
class CWeakOwnerPtr
{
public:
// Construction
			CWeakOwnerPtr();
	virtual ~CWeakOwnerPtr();

// Operations
	HRESULT Init(IUnknown* pWeakOwner, IUnknown* pObject);
	void Release();

// Attributes
	IUnknown*	p;
};


template <class Itf>
class CTypedWeakOwnerPtr : public CWeakOwnerPtr
{
public:
// Operations
	HRESULT Init(IUnknown* pWeakOwner, Itf* pObject)
	{
		return CWeakOwnerPtr::Init(pWeakOwner, pObject);		
	}

// Inline operations
	operator Itf*() const
	{
		return (Itf*)p;
	}
	Itf& operator*() const
	{
		ATLASSERT(p!=NULL);
		return *p;
	}
	//The assert on operator& usually indicates a bug.  If this is really
	//what is needed, however, take the address of the p member explicitly.
	Itf** operator&()
	{
		ATLASSERT(p==NULL);
		return &p;
	}
	_NoAddRefReleaseOnCComPtr<Itf>* operator->() const
	{
		ATLASSERT(p!=NULL);
		return (_NoAddRefReleaseOnCComPtr<Itf>*)p;
	}
	HRESULT CopyTo(Itf** ppT)
	{
		ATLASSERT(ppT != NULL);
		if (ppT == NULL)
			return E_POINTER;
		*ppT = (Itf*)p;
		if (p)
			p->AddRef();
		return S_OK;
	}

/*
	HRESULT Create(IUnknown* pOwner)
	{
		// Create new instance
		CComObject<T>* pNew;
		RETURNIFFAILED(CComObject<T>::CreateInstance(&pNew));

		// Store pointer
		Init(pOwner, pNew);
	}
*/
};

}	// namespace Simple

#endif	// __OBJECTWITHWEAKOWNER_H

