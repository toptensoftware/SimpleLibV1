//////////////////////////////////////////////////////////////////////////
// PropertyNotifyHandler.cpp - implementation of CPropertyNotifyHandler class

#include "stdafx.h"
#include "SimpleLibAtlComBuild.h"

#include "PropertyNotifyHandler.h"

#ifndef _WIN64

namespace Simple
{

//////////////////////////////////////////////////////////////////////////
// CPropertyNotifyHandler

// Constructor
CPropertyNotifyHandler::CPropertyNotifyHandler()
{
}

// Destructor
CPropertyNotifyHandler::~CPropertyNotifyHandler()
{
}

void CPropertyNotifyHandler::Init(void* pInstance, NOTIFYHANDLER pfn)
{
	m_pInstance=pInstance;
	m_pfn=pfn;
}

// Implementation of GetTypeInfoCount
STDMETHODIMP CPropertyNotifyHandler::GetTypeInfoCount(UINT* pctinfo)
{
	return E_NOTIMPL;
}

// Implementation of GetTypeInfo
STDMETHODIMP CPropertyNotifyHandler::GetTypeInfo(UINT iTInfo, LCID lcid, ITypeInfo** ppTInfo)
{
	return E_NOTIMPL;
}

// Implementation of GetIDsOfNames
STDMETHODIMP CPropertyNotifyHandler::GetIDsOfNames(REFIID riid, LPOLESTR* rgszNames, UINT cNames, LCID lcid, DISPID* rgDispId)
{
	return E_NOTIMPL;
}

static int CalcVariantStackSize(VARTYPE vt)
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


// Implementation of Invoke
STDMETHODIMP CPropertyNotifyHandler::Invoke(DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS* pDispParams, VARIANT* pVarResult, EXCEPINFO* pExcepInfo, UINT* puArgErr)
{
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
	DWORD dw=(DWORD)m_dwpfn;
	DWORD_PTR dwpThis=(DWORD_PTR)m_pInstance;
	_asm  
		{
		mov	ecx,dwpThis
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

#endif	// _WIN64
