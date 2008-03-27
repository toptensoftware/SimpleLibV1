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
// IsControlInDesignMode.cpp - implementation of IsControlInDesignMode

#include "stdafx.h"
#include "SimpleLibAtlComBuild.h"

#include "IsControlInDesignMode.h"
#include "DispatchProperty.h"

namespace Simple
{

bool IsControlInDesignMode(IUnknown* pControl)
{
	// QI for IOleObject
	CComQIPtr<IOleObject> spOleObject(pControl);
	if (!spOleObject)
		return false;

	// Get client site
	CComPtr<IOleClientSite> spClientSite;
	spOleObject->GetClientSite(&spClientSite);
	
	// QI Client site for IDispatch
	CComQIPtr<IDispatch> spAmbient(spClientSite);
	if (!spAmbient)
		return false;

	CComVariant varUserMode;
	if (SUCCEEDED(SlxGetProperty(spAmbient, DISPID_AMBIENT_USERMODE, &varUserMode)) && 
		SUCCEEDED(varUserMode.ChangeType(VT_BOOL)))
		{
		return !V_BOOL(&varUserMode);
		}

	return false;
}





}	// namespace Simple
