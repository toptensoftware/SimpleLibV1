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
// ComObjectOwned.h - declaration of CommandLineParser

#ifndef __COMOBJECTOWNED_H
#define __COMOBJECTOWNED_H

namespace Simple
{

template <class Base, class TOwnerClass>
class CComObjectOwned : public Base
{
public:
	// Constructor
	CComObjectOwned(TOwnerClass* pOwner)
	{
		_Module.Lock();
		_ASSERTE(pOwner!=NULL);
		m_pOwner=pOwner;		// Base class must define a m_pOwner attribute
		m_dwRef=0;
	}
	~CComObjectOwned()
	{
		_Module.Unlock();
	}

	//If InternalAddRef or InteralRelease is undefined then your class
	//doesn't derive from CComObjectRoot
	STDMETHOD_(ULONG, AddRef)() 
	{
		return m_pOwner->GetUnknown()->AddRef();
	}
	STDMETHOD_(ULONG, Release)() 
	{
		return m_pOwner->GetUnknown()->Release();
	}
	STDMETHOD(QueryInterface)(REFIID iid, void ** ppvObject)
	{
		HRESULT hr=_InternalQueryInterface(iid, ppvObject);
		return hr;
	}

	template <class Itf>
	HRESULT CopyTo(Itf** ppVal)
	{
		return QueryInterface(__uuidof(Itf), (void**)ppVal);
	}
};

}	// namespace Simple

#endif		// __COMOBJECTOWNED_H

