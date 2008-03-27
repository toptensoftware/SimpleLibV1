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
// ObjectIdentity.cpp - implementation of ObjectIdentity

#include "stdafx.h"
#include "SimpleLibAtlComBuild.h"
#include "ObjectIdentity.h"

namespace Simple
{


// Compare two object for same identity
BOOL IsSameObject(IUnknown* pObject1, IUnknown* pObject2)
{
	// Both NULL?
	if (!pObject1 && !pObject2)
		return TRUE;

	// One NULL?
	if (!pObject1 || !pObject2)
		return FALSE;

	// Query both for IUnknown
	CComPtr<IUnknown> pUnk1;
	if (FAILED(pObject1->QueryInterface(IID_IUnknown, (void**)&pUnk1)))
		return FALSE;

	CComPtr<IUnknown> pUnk2;
	if (FAILED(pObject2->QueryInterface(IID_IUnknown, (void**)&pUnk2)))
		return FALSE;

	// Compare and done...
	return pUnk1==pUnk2;
}


}	// namespace Simple
