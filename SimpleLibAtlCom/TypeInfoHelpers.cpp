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
// TypeInfoHelpers.cpp - implementation of TypeInfoHelpers

#include "stdafx.h"
#include "SimpleLibAtlComBuild.h"

#include "TypeInfoHelpers.h"
#include "ObjectIdentity.h"

namespace Simple
{

//////////////////////////////////////////////////////////////////////////
// CFuncDesc

TYPEDESC* CFuncDesc::FindReturnType()
{
	// Find out param?
	for (short i=0; i<m_pFuncDesc->cParams; i++)
		{
		if (m_pFuncDesc->lprgelemdescParam[i].paramdesc.wParamFlags & PARAMFLAG_FRETVAL)
			{
			ASSERT(m_pFuncDesc->lprgelemdescParam[i].tdesc.vt==VT_PTR);
			return m_pFuncDesc->lprgelemdescParam[i].tdesc.lptdesc;
			}
		}

	return &m_pFuncDesc->elemdescFunc.tdesc;
}

//////////////////////////////////////////////////////////////////////////
// CTypeInfo

HRESULT CTypeInfo::GetImplTypeInfo(int iRequiredFlags, ITypeInfo** pVal)
{
	if (!p)
		return E_UNEXPECTED;

	// Get type attributes
	CTypeAttr ta;
	ta.Init(p);

	// Find default source info
	for (UINT i=0; i<ta->cImplTypes; i++)
		{
        // Get the implementation type for this interface
		int iFlags;
        if (SUCCEEDED(p->GetImplTypeFlags(i, &iFlags)))
			{
			// Correct type?
            if (iFlags != iRequiredFlags)
				continue;

			// Get reference to interface
            HREFTYPE hRefType=NULL;
            RETURNIFFAILED(p->GetRefTypeOfImplType(i, &hRefType));

			// Get interface
            return p->GetRefTypeInfo(hRefType, pVal);
			}
		}

	return E_FAIL;
}

HRESULT CTypeInfo::GetAllMembers(CVector<CUniString>& Members, bool bIncludeHidden)
{
	if (!p)
		return E_UNEXPECTED;


	// Get type attributes
	CTypeAttr ta;
	ta.Init(p);


	// Get all functions
	WORD i;
	for (i=0; i<ta->cFuncs; i++)
		{
		// Get function description
		CFuncDesc pfd;
		RETURNIFFAILED(pfd.Init(p, i));

		// Ignore hidden
		if ((pfd->wFuncFlags & FUNCFLAG_FHIDDEN)!=0 && !bIncludeHidden)
			continue;

		// Get its name
		CComBSTR bstrName;
		p->GetDocumentation(pfd->memid, &bstrName, NULL, NULL, NULL);

		if (!IsEmptyString(bstrName))
			Members.Add(bstrName.m_str);
		}

	// Get all variables
	for (i=0; i<ta->cVars; i++)	
		{
		// Get function description
		CVarDesc pfd;
		RETURNIFFAILED(pfd.Init(p, i));

		// Ignore hidden
		if ((pfd->wVarFlags & FUNCFLAG_FHIDDEN)!=0 && !bIncludeHidden)
			continue;

		// Get its name
		CComBSTR bstrName;
		p->GetDocumentation(pfd->memid, &bstrName, NULL, NULL, NULL);

		// Release info
		p->ReleaseVarDesc(pfd);

		if (!IsEmptyString(bstrName))
			Members.Add(bstrName.m_str);
		}

	// Done
	return S_OK;	
}

HRESULT CTypeInfo::GetIDOfName(LPCOLESTR pszName, MEMBERID* pMemID)
{
	if (!p)
		return E_UNEXPECTED;

	LPOLESTR psz=const_cast<LPOLESTR>(pszName);
	return p->GetIDsOfNames(&psz, 1, pMemID);
}

HRESULT CTypeInfo::GetParameterNames(MEMBERID memid, CVector<CUniString>& Params)
{
	if (!p)
		return E_UNEXPECTED;

	// Get all member names
	BSTR bstrNames[100];
	memset(bstrNames, 0, sizeof(bstrNames));
	UINT cNames=0;
	p->GetNames(memid, bstrNames, 100, &cNames);

	if (cNames>0)
		SysFreeString(bstrNames[0]);

	for (UINT i=1; i<cNames; i++)
		{
		Params.Add(bstrNames[i]);
		SysFreeString(bstrNames[i]);
		}

	return S_OK;
}



//////////////////////////////////////////////////////////////////////////
// CType

// Constructor
CType::CType()
{
	m_iIndirection=0;
	m_vt=VT_VOID;
}

// Constructor
CType::CType(VARTYPE vt)
{
	Init(vt);
}

// Constructor
CType::CType(TYPEDESC& tdIn, ITypeInfo* pParentTypeInfo, bool bResolveAlias)
{
	Init(tdIn, pParentTypeInfo, bResolveAlias);
}

// Initialisation
HRESULT CType::Init(VARTYPE vt)
{
	m_iIndirection=0;
	m_vt=vt;
	return S_OK;
}

// Initialisation
HRESULT CType::Init(TYPEDESC& tdIn, ITypeInfo* pParentTypeInfo, bool bResolveAlias)
{
	m_spTypeInfo.Release();

	// Work out indirection level...
	m_iIndirection=0;
	TYPEDESC* ptd=&tdIn;
	while (ptd->vt==VT_PTR)
		{
		m_iIndirection++;
		ptd=ptd->lptdesc;
		}

	// Store type...
	m_vt=ptd->vt;
	if (m_vt==VT_USERDEFINED)	
		{
		RETURNIFFAILED(pParentTypeInfo->GetRefTypeInfo(ptd->hreftype, &m_spTypeInfo));

		if (bResolveAlias)
			{
			CTypeAttr attr;
			attr.Init(m_spTypeInfo);
			if (attr->typekind==TKIND_ALIAS && attr->tdescAlias.vt==VT_USERDEFINED)
				{
				CComPtr<ITypeInfo> spTypeInfo(m_spTypeInfo);
				m_spTypeInfo.Release();
				int iIndirection=m_iIndirection;
				RETURNIFFAILED(Init(attr->tdescAlias, spTypeInfo, false));
				m_iIndirection+=iIndirection;
				return S_OK;
				}
			}
		}

	return S_OK;
}

bool CType::IsEnumeration() const
{
	// Ignore pointer
	if (m_iIndirection>0)
		return false;

	// Must be a user defined type
	if (m_vt!=VT_USERDEFINED)
		return false;
		
	// Get type info attributes
	CTypeAttr attr;
	if (FAILED(attr.Init(m_spTypeInfo)))
		return false;

	// Enumeration
	return attr->typekind==TKIND_ENUM;
}

class __declspec(uuid("66504301-BE0F-101A-8BBB-00AA00300CAB")) COleColor;

bool CType::IsColor() const
{
	// Ignore pointer
	if (m_iIndirection>0)
		return false;

	if (m_vt!=VT_USERDEFINED)
		return false;

	// Get type info attributes
	CTypeAttr attr;
	if (FAILED(attr.Init(m_spTypeInfo)))
		return false;

	// Color?
	return attr->typekind==TKIND_ALIAS && attr->guid==__uuidof(COleColor);
}


GUID CType::GetGUID() const
{
	// Must be user defined
	if (m_vt!=VT_USERDEFINED)
		return CLSID_NULL;

	// Get type info attributes
	CTypeAttr attr;
	if (FAILED(attr.Init(m_spTypeInfo)))
		return CLSID_NULL;

	// Return it
	return attr->guid;
}



bool AreTypeInfosEqual(ITypeInfo* pType1, ITypeInfo* pType2)
{
	// Both NULL
	if (!pType1 && !pType2)
		return true;

	// One NULL
	if (!pType1 || !pType2)
		return false;
	
	// Same object reference?
	if (IsSameObject(pType1, pType2))
		return true;

	// Get attributes of each
	CTypeAttr attr1, attr2;
	attr1.Init(pType1);
	attr2.Init(pType2);

	// Guids different?
	if (attr1->guid!=attr2->guid)
		return false;

	// Different versions
	if (attr1->wMajorVerNum != attr2->wMajorVerNum)
		return false;
	if (attr1->wMinorVerNum != attr2->wMinorVerNum)
		return false;

	// Both GUID's non-NULL?
	if (attr1->guid!=GUID_NULL)
		{
		// Valid guids, same version assume same type
		return true;
		}

	// Hardest case, GUIDs both NULL and both in type lib of same version...
	// Need to check if in same type lib and then compare name...

	CComPtr<ITypeLib> spTypeLib1, spTypeLib2;
	UINT nIndex1, nIndex2;
	pType1->GetContainingTypeLib(&spTypeLib1, &nIndex1);
	pType2->GetContainingTypeLib(&spTypeLib2, &nIndex2);
	
	if (nIndex1!=nIndex2)
		return false;

	CLibAttr lattr1, lattr2;
	lattr1.Init(spTypeLib1);
	lattr2.Init(spTypeLib2);

	return (lattr1->guid==lattr2->guid && 
			lattr1->wMajorVerNum==lattr2->wMajorVerNum &&
			lattr1->wMinorVerNum==lattr2->wMinorVerNum);
}

bool CType::IsEqual(const CType& Other) const
{
	// Check types match
	if (m_vt!=Other.m_vt)
		return false;

	// Check indirection matches
	if (m_iIndirection!=Other.m_iIndirection)
		return false;

	return AreTypeInfosEqual(m_spTypeInfo, Other.m_spTypeInfo);
}

}	// namespace Simple
