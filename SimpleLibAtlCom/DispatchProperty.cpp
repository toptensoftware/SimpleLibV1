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
// DispatchProperty.cpp - implementation of DispatchProperty

#include "stdafx.h"
#include "SimpleLibAtlComBuild.h"

#include "DispatchProperty.h"
#include "VariantParams.h"


namespace Simple
{

HRESULT SIMPLEAPI SlxGetIDsOfName(IDispatch* pDisp, LPCOLESTR psz, DISPID* pVal)
{
	ASSERT(pDisp!=NULL);
	return pDisp->GetIDsOfNames(IID_NULL, const_cast<LPOLESTR*>(&psz), 1, LOCALE_SYSTEM_DEFAULT, pVal);
}


HRESULT SIMPLEAPI SlxGetProperty(IDispatch* pDisp, const CDISPID& DispID, VARIANT* pVar)
{
	DISPID dwDispID;
	RETURNIFFAILED(DispID.Resolve(pDisp, &dwDispID));

	DISPPARAMS dispparamsNoArgs = {NULL, NULL, 0, 0};
	return pDisp->Invoke(dwDispID, IID_NULL,
			LOCALE_USER_DEFAULT, DISPATCH_PROPERTYGET,
			&dispparamsNoArgs, pVar, NULL, NULL);
}

HRESULT SIMPLEAPI SlxPutProperty(IDispatch* pDisp, const CDISPID& DispID, VARIANT newVal)
{
	DISPID dwDispID;
	RETURNIFFAILED(DispID.Resolve(pDisp, &dwDispID));

	DISPPARAMS dispparams = {NULL, NULL, 1, 1};
	dispparams.rgvarg = &newVal;
	DISPID dispidPut = DISPID_PROPERTYPUT;
	dispparams.rgdispidNamedArgs = &dispidPut;

	if (newVal.vt == VT_UNKNOWN || newVal.vt == VT_DISPATCH || 
		(newVal.vt & VT_ARRAY) || (newVal.vt & VT_BYREF))
	{
		HRESULT hr = pDisp->Invoke(dwDispID, IID_NULL,
			LOCALE_USER_DEFAULT, DISPATCH_PROPERTYPUTREF,
			&dispparams, NULL, NULL, NULL);
		if (SUCCEEDED(hr))
			return hr;
	}

	return pDisp->Invoke(dwDispID, IID_NULL,
			LOCALE_USER_DEFAULT, DISPATCH_PROPERTYPUT,
			&dispparams, NULL, NULL, NULL);
}

HRESULT SIMPLEAPI SlxInitProperty(IDispatch* pDisp, const CDISPID& DispID, VARIANT newVal)
{
	if (IsParameterSpecified(newVal))
		return SlxPutProperty(pDisp, DispID, newVal);
	else
		return S_OK;
}


}	// namespace Simple
