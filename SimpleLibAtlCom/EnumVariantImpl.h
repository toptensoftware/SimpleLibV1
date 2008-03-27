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
// EnumVariantImpl.h - declaration of EnumVariantImpl

#ifndef __ENUMVARIANTIMPL_H
#define __ENUMVARIANTIMPL_H

namespace Simple
{

// Forward declaration
class IEnumVariantImpl;

// Need to provide implementation of this for IEnumVariantImpl to work!
class ATL_NO_VTABLE IEnumVariantOwner
{
public:
	virtual HRESULT GetSize(int* piSize) const=0;
	virtual HRESULT GetItem(int iItem, VARIANT* pvarItem) const=0;
	virtual void AddEnumerator(IEnumVariantImpl* pEnum)=0;
	virtual void RemoveEnumerator(IEnumVariantImpl* pEnum)=0;
};

// Enumerator for CComPtrVector
class IEnumVariantImpl: public IEnumVARIANT  
{
public:
// Construction
			IEnumVariantImpl(IEnumVariantOwner* pVector, int iPos = 0);
	virtual ~IEnumVariantImpl();

// IUnknown
	STDMETHOD_(ULONG, AddRef)();
	STDMETHOD_(ULONG, Release)();
	STDMETHOD(QueryInterface)(REFIID iid, void** ppvObject);

// IEnumVARIANT
	STDMETHOD(Next)(ULONG celt, VARIANT* rgelt, ULONG* pceltFetched);
	STDMETHOD(Skip)(ULONG celt);
	STDMETHOD(Reset)();
	STDMETHOD(Clone)(IEnumVARIANT** ppenum);

// Operations
	void SyncInsert(int iPos);
	void SyncDelete(int iPos);
	void OwnerDestroyed();

	IEnumVariantOwner*	m_pOwner;
	DWORD				m_dwRef;
	int					m_iPos;
};

// Holder of variant enumerators
class IEnumVariantHolder : public IEnumVariantOwner
{
public:
// Construction
			IEnumVariantHolder();
	virtual ~IEnumVariantHolder();

// Operations
	HRESULT CreateEnumerator(IUnknown** ppEnum);

// Operations
	void SyncInsert(int iPos);
	void SyncDelete(int iPos);

protected:
// Overrides
	virtual void AddEnumerator(IEnumVariantImpl* pEnum);
	virtual void RemoveEnumerator(IEnumVariantImpl* pEnum);

// Attributes
	CVector<IEnumVariantImpl*>	m_Enumerators;
};

/*  UNTESTED!!!

template <class theClass, class TItem, class TIndexType=int, class TCountType=TIndexType, int iIndexBase=0>
class CQuickEnumerator : public IEnumVariantHolder
{
public:
	virtual int GetSize()
	{
		TCountType iCount;
		static_cast<theClass>(this)->get_Count(&iCount);
		return iCount;
	};

	virtual void GetItem(int iItem, VARIANT* pvarItem)
	{
		// Get the item
		TItem Item;
		static_cast<theClass>(this)->get_Item(TIndexType(iItem+iIndexBase), &Item);

		// Return it
		V_VT(pvarItem)=VT_EMPTY;
		*((CComVariant*)pvarItem)=Item;
	};
};

*/


}	// namespace Simple

#endif	// __ENUMVARIANTIMPL_H

