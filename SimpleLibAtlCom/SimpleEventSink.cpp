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
// SimpleEventSink.cpp - implementation of SimpleEventSink

#include "stdafx.h"
#include "SimpleLibAtlComBuild.h"

#include "SimpleEventSink.h"

#ifndef _WIN64

namespace Simple
{


int CalcVariantStackSize(VARTYPE vt)
{
	// Vector or array just push ptr size..
	if (vt & (VT_ARRAY|VT_BYREF))
		return sizeof(void*);

	switch (vt)
		{
		case VT_I1:
		case VT_UI1:
		case VT_UI2:
		case VT_UI4:
		case VT_I2:
		case VT_I4:
		case VT_R4:
		case VT_BSTR:
		case VT_ERROR:
		case VT_BOOL:
			return 4;

		case VT_DISPATCH:
		case VT_UNKNOWN:
			return sizeof(IUnknown*);

		case VT_R8:
		case VT_DATE:
			return sizeof(double);

		case VT_CY:
			return sizeof(CY);

		case VT_INT:
		case VT_UINT:
			return sizeof(int);

		case VT_I8:
		case VT_UI8:
			return 8;

		case VT_HRESULT:
			return sizeof(HRESULT);
		}

	return 0;
}

HRESULT SIMPLEAPI InvokeEventHandler(void* pThis, EVENTENTRY<CDummy>* pEventMap, ITypeInfo* pEventInfo, DISPID dispid, DISPPARAMS* pDispParams)
{
	// Check no named args...
	if (pDispParams->cNamedArgs)
		return E_NOTIMPL;

	// Find handler using dispid
	bool bHaveNamedEntries=false;
	EVENTENTRY<CDummy>* p;
	for (p=pEventMap; p->m_dwpfn!=NULL; p++)
		{
		if (p->m_pszEventName)
			bHaveNamedEntries=true;

		// Match?
		if (!p->m_pszEventName && p->m_iEventID==dispid)
			 break;
		}

	// If couldn't find by ID, use name
	if (!p->m_dwpfn && bHaveNamedEntries)
		{
		// What is the event's name
		CComBSTR bstrEventName;
		pEventInfo->GetDocumentation(dispid, &bstrEventName, NULL, NULL, NULL);
		
		// Find handler
		for (p=pEventMap; p->m_dwpfn!=NULL; p++)
			{
			// Match?
			if (p->m_pszEventName && _wcsicmp(bstrEventName, p->m_pszEventName)==0) 
				 break;
			}
		}

	// Quit if not interested
	if (!p->m_dwpfn)
		return S_OK;

	// Save old stack pointer
	#ifdef _DEBUG
	DWORD dwESPOld;
	_asm mov dwESPOld,esp
	#endif

	// Pack parameters onto stack...
	HRESULT hr;
	for (UINT i=0; i<pDispParams->cArgs; i++)
		{
		// Work out how much to push onto stack
		DWORD cbPush=CalcVariantStackSize(pDispParams->rgvarg[i].vt);
		ASSERT(cbPush!=0);

		// Push it...
		void* pSrc=&pDispParams->rgvarg[i].bVal;
		_asm 
			{
			mov ecx,cbPush
			sub esp,ecx
			mov edi,esp
			mov esi,pSrc
			rep movsb
			}
		}

	// Call it
	DWORD dw=p->m_dwpfn;
	_asm  
		{
		mov	ecx,pThis
		call dw
		mov hr,eax;
		}

	// Check handler fixed up the stack OK...
	#ifdef _DEBUG
	_asm mov dw,esp
	ASSERT(dw==dwESPOld);
	#endif

	// Finished...
	return hr;
}



}	// namespace Simple

#endif		// _WIN64
