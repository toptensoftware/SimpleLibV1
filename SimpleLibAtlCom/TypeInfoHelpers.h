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
// TypeInfoHelpers.h - declaration of TypeInfoHelpers

#ifndef __TYPEINFOHELPERS_H
#define __TYPEINFOHELPERS_H

namespace Simple
{

class CFuncDesc
{
public:
	CFuncDesc()
	{
		m_pFuncDesc=NULL;
	}
	~CFuncDesc()
	{
		Release();
	}

	HRESULT Init(ITypeInfo* pTypeInfo, int iIndex)
	{
		Release();
		ASSERT(pTypeInfo);
		RETURNIFFAILED(pTypeInfo->GetFuncDesc(iIndex, &m_pFuncDesc));
		m_spTypeInfo=pTypeInfo;
		return S_OK;
	}
	HRESULT Init(ITypeInfo* pTypeInfo, MEMBERID memid, INVOKEKIND invKind)
	{
		ASSERT(pTypeInfo);
		CComQIPtr<ITypeInfo2> spTI2(pTypeInfo);
		if (!spTI2)
			return E_NOINTERFACE;
			
		UINT iIndex;
		RETURNIFFAILED(spTI2->GetFuncIndexOfMemId(memid, invKind, &iIndex));

		return Init(pTypeInfo, iIndex);
	}
	HRESULT Init(ITypeInfo* pTypeInfo, LPCOLESTR pszName, INVOKEKIND invKind)
	{
		ASSERT(pTypeInfo);
		LPOLESTR p=const_cast<LPOLESTR>(pszName);
		MEMBERID id;
		RETURNIFFAILED(pTypeInfo->GetIDsOfNames(&p, 1, &id));
		return Init(pTypeInfo, id, invKind);
	}
	void Release()
	{
		if (m_pFuncDesc)
			m_spTypeInfo->ReleaseFuncDesc(m_pFuncDesc);
		m_pFuncDesc=NULL;
		m_spTypeInfo.Release();
	}

	TYPEDESC* FindReturnType();

	operator FUNCDESC* () { return m_pFuncDesc; };
	FUNCDESC* operator->() { return m_pFuncDesc; };

	CComPtr<ITypeInfo>	m_spTypeInfo;
	FUNCDESC*			m_pFuncDesc;
};

class CVarDesc
{
public:
	CVarDesc()
	{
		m_pVarDesc=NULL;
	}
	~CVarDesc()
	{
		Release();
	}

	HRESULT Init(ITypeInfo* pTypeInfo, int iIndex)
	{
		Release();
		RETURNIFFAILED(pTypeInfo->GetVarDesc(iIndex, &m_pVarDesc));
		m_spTypeInfo=pTypeInfo;
		return S_OK;
	}
	void Release()
	{
		if (m_pVarDesc)
			m_spTypeInfo->ReleaseVarDesc(m_pVarDesc);
		m_pVarDesc=NULL;
		m_spTypeInfo.Release();
	}

	operator VARDESC* () { return m_pVarDesc; };
	VARDESC* operator->() { return m_pVarDesc; };

	CComPtr<ITypeInfo>	m_spTypeInfo;
	VARDESC*			m_pVarDesc;
};

class CTypeAttr
{
public:
	CTypeAttr()
	{
		m_pTypeAttr=NULL;
	}
	~CTypeAttr()
	{
		Release();
	}

	HRESULT Init(ITypeInfo* pTypeInfo)
	{
		RETURNIFFAILED(pTypeInfo->GetTypeAttr(&m_pTypeAttr));
		m_spTypeInfo=pTypeInfo;
		return S_OK;
	}
	void Release()
	{
		if (m_pTypeAttr)
			m_spTypeInfo->ReleaseTypeAttr(m_pTypeAttr);
		m_pTypeAttr=NULL;
		m_spTypeInfo.Release();
	}

	operator TYPEATTR* () { return m_pTypeAttr; };
	TYPEATTR* operator->() { return m_pTypeAttr; };

	CComPtr<ITypeInfo>	m_spTypeInfo;
	TYPEATTR*			m_pTypeAttr;
};

class CLibAttr
{
public:
	CLibAttr()
	{
		m_pLibAttr=NULL;
	}
	~CLibAttr()
	{
		Release();
	}

	HRESULT Init(ITypeLib* pTypeLib)
	{
		RETURNIFFAILED(pTypeLib->GetLibAttr(&m_pLibAttr));
		m_spTypeLib=pTypeLib;
		return S_OK;
	}
	void Release()
	{
		if (m_pLibAttr)
			m_spTypeLib->ReleaseTLibAttr(m_pLibAttr);
		m_pLibAttr=NULL;
		m_spTypeLib.Release();
	}

	operator TLIBATTR* () { return m_pLibAttr; };
	TLIBATTR* operator->() { return m_pLibAttr; };

	CComPtr<ITypeLib>	m_spTypeLib;
	TLIBATTR*			m_pLibAttr;
};

// Type info helper
class CTypeInfo : public CComPtr<ITypeInfo>
{
public:
// Constructor
	CTypeInfo(ITypeInfo* pTypeInfo=NULL) 
		: CComPtr<ITypeInfo>(pTypeInfo) 
	{
	};

// Attributes
	HRESULT GetImplTypeInfo(int iRequiredFlags, ITypeInfo** pVal);
	HRESULT GetAllMembers(CVector<CUniString>& Members, bool bIncludeHidden=false);
	HRESULT GetParameterNames(MEMBERID memid, CVector<CUniString>& Params);
	HRESULT GetIDOfName(LPCOLESTR pszName, MEMBERID* pMemID);
};

class CType
{
public:
// Constructor
	CType();
	CType(VARTYPE vt);
	CType(TYPEDESC& td, ITypeInfo* pParentTypeInfo, bool bResolveAlias);

	HRESULT Init(VARTYPE vt);
	HRESULT Init(TYPEDESC& td, ITypeInfo* pParentTypeInfo, bool bResolveAlias);

// Copy constructor
	CType(const CType& Other)
	{
		*this=Other;
	}

// Assignment operator
	CType& operator=(const CType& Other)
	{
		m_vt=Other.m_vt;
		m_spTypeInfo=Other.m_spTypeInfo;
		m_iIndirection=Other.m_iIndirection;
		return *this;
	}

// Equality comparison
	bool IsEqual(const CType& Other) const;
	bool IsPointer() const { return m_iIndirection>0; };
	bool IsEnumeration() const;
	bool IsColor() const;
	GUID GetGUID() const;

	ITypeInfo* GetTypeInfo() const { return m_spTypeInfo; };
	VARTYPE GetVarType() const { return m_vt; };
	int GetIndirection() const { return m_iIndirection; };

protected:
// Attributes
	VARTYPE m_vt;
	int m_iIndirection;
	CComPtr<ITypeInfo> m_spTypeInfo;
};

inline bool SIMPLEAPI operator==(const CType& a, const CType& b)
{
	return a.IsEqual(b);
}

inline bool SIMPLEAPI operator!=(const CType& a, const CType& b)
{
	return !a.IsEqual(b);
}

inline bool SIMPLEAPI operator==(const CType& a, VARTYPE vt)
{
	return a.GetVarType()==vt && !a.IsPointer();
}

inline bool SIMPLEAPI operator!=(const CType& a, VARTYPE vt)
{
	return !(a==vt);
}

}	// namespace Simple

#endif	// __TYPEINFOHELPERS_H

