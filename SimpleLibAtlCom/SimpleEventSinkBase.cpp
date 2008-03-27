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
// SimpleEventSinkBase.cpp - implementation of SimpleEventSinkBase

#include "stdafx.h"
#include "SimpleLibAtlComBuild.h"

#include "SimpleEventSinkBase.h"



namespace Simple
{


// Get source interface for an object
HRESULT GetObjectSourceInterface(IUnknown* pObject, IID* piidEvents, ITypeInfo** ppEventInfo)
{
	// Get dispatch interface
	CComQIPtr<IDispatch> spDispSource(pObject);
	if (!spDispSource)
		return E_NOINTERFACE;

	// Get type info
	CComPtr<ITypeInfo> spTypeInfo;
	spDispSource->GetTypeInfo(0, LOCALE_USER_DEFAULT, &spTypeInfo);
	if (!spTypeInfo)
		return E_FAIL;

	// Get containing type library
	CComPtr<ITypeLib> spTypeLib;
	UINT uIndex;
	RETURNIFFAILED(spTypeInfo->GetContainingTypeLib(&spTypeLib, &uIndex));
	
	// Can object provide source interface?
	*piidEvents=IID_NULL;
	CComQIPtr<IProvideClassInfo2> spPCI2(pObject);
	if (spPCI2)
		{
		if (SUCCEEDED(spPCI2->GetGUID(GUIDKIND_DEFAULT_SOURCE_DISP_IID, piidEvents)))
			{
			if (SUCCEEDED(spTypeLib->GetTypeInfoOfGuid(*piidEvents, ppEventInfo)))
				return S_OK;
			}
		}

	// See if object can provide class info
	CComPtr<ITypeInfo> spClassInfo;
	CComQIPtr<IProvideClassInfo> spPCI(pObject);
	if (spPCI)
		spPCI->GetClassInfo(&spClassInfo);

	// If still no class info, get object's class ID from IPersist, then get
	// class info from containing type library
	if (!spClassInfo)
		{
		CComQIPtr<IPersist> spPersist(pObject);
		if (spPersist)
			{
			CLSID clsid;
			RETURNIFFAILED(spPersist->GetClassID(&clsid));
			RETURNIFFAILED(spTypeLib->GetTypeInfoOfGuid(clsid, &spClassInfo));
			}
		}

	// Object doesn't support enough to find out default source interface
	if (!spClassInfo)
		return E_NOINTERFACE;

	// How many interface?
	TYPEATTR* pAttr=NULL;
	RETURNIFFAILED(spClassInfo->GetTypeAttr(&pAttr));
	int iInterfaces=pAttr->cImplTypes;
	spClassInfo->ReleaseTypeAttr(pAttr);

	// Find default source interface
	for (int i=0; i<iInterfaces; i++)
		{
		// Is it the default source interface?
		int nType;
		RETURNIFFAILED(spClassInfo->GetImplTypeFlags(i, &nType));
		if (nType == (IMPLTYPEFLAG_FDEFAULT | IMPLTYPEFLAG_FSOURCE))
			{
			// Get type info for the interface
			HREFTYPE hRefType;
			RETURNIFFAILED(spClassInfo->GetRefTypeOfImplType(i, &hRefType));
			CComPtr<ITypeInfo> spEventInfo;
			RETURNIFFAILED(spClassInfo->GetRefTypeInfo(hRefType, &spEventInfo));

			// Get interface GUID
			TYPEATTR* pAttr;
			RETURNIFFAILED(spEventInfo->GetTypeAttr(&pAttr));
			*piidEvents=pAttr->guid;
			spEventInfo->ReleaseTypeAttr(pAttr);

			// Done!
			return spEventInfo.CopyTo(ppEventInfo);
			}
		}

	return E_NOINTERFACE;
};


}	// namespace Simple
