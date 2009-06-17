//////////////////////////////////////////////////////////////////////////
// PropertyNotifyHandler.cpp - implementation of CPropertyNotifyHandler class

#include "stdafx.h"
#include "SimpleLibAtlComBuild.h"

#include "PropertyNotifyHandler.h"
#include "SimpleEventSink.h"

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

// Implementation of Invoke
STDMETHODIMP CPropertyNotifyHandler::Invoke(DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS* pDispParams, VARIANT* pVarResult, EXCEPINFO* pExcepInfo, UINT* puArgErr)
{
	return _invoke_c_this(m_pInstance, pDispParams->cArgs, pDispParams->rgvarg, m_pvfn);
}


}	// namespace Simple

